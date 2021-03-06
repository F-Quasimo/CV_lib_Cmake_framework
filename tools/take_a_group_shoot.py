# -*- coding: utf-8 -*-
import os
import re
import cv2
import numpy as np
import platform
import threading
import time

class SingleCam:
    def __init__(self, cam_id=0, cam_size=(1920, 1080), cam_mode=cv2.CAP_DSHOW, cam_fps=30, exposure=-6, bright=0):
        self.cam_id = cam_id
        self.cap = None
        self.cam_size = cam_size
        self.cap_open_mode = cam_mode
        self.cam_fps = cam_fps
        self.exposure = exposure
        self.bright = bright

        self.frame_buffer = [None, None]
        self.frame_idx = 0
        self.thread_alive = False
        self.videocap_thread = None

    def _thread_cam(self):
        while self.thread_alive:
            ret, snap = self.cap.read()
            self.frame_buffer[self.frame_idx] = snap
            self.frame_idx = (self.frame_idx + 1) % 2
            # print('DEBUG FPS: ', self.cap.get(cv2.CAP_PROP_FPS), ' exp ', self.cap.get(cv2.CAP_PROP_EXPOSURE),
            #       'frame_size: ', self.cap.get(cv2.CAP_PROP_FRAME_HEIGHT), ' ', self.cap.get(cv2.CAP_PROP_FRAME_WIDTH))

    def OpenCam(self, time_sleep=0):
        try:
            self.Close()
        except:
            print('CLOSE IN OpenCam ERROR')
            return False
        if time_sleep > 0:
            time.sleep(time_sleep)
        print('OpenCam: Single ', self.cam_id, ' mode: ', self.cap_open_mode)
        self.cap = cv2.VideoCapture(self.cam_id + self.cap_open_mode)
        self.cap.open(self.cam_id)
        self.cap.set(cv2.CAP_PROP_FOURCC,
                     cv2.VideoWriter_fourcc('M', 'J', 'P', 'G'))
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, self.cam_size[0])
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, self.cam_size[1])
        self.cap.set(cv2.CAP_PROP_FPS, self.cam_fps)
        # self.cap.set(cv2.CAP_PROP_AUTO_EXPOSURE,1)
        # self.cap.set(cv2.CAP_PROP_EXPOSURE,-4)
        self.cap.set(cv2.CAP_PROP_BRIGHTNESS, self.bright)
        
        self.cap.set(cv2.CAP_PROP_AUTO_EXPOSURE, 0.25)
        self.cap.set(cv2.CAP_PROP_EXPOSURE, self.exposure)
        print('DEBUG SingleCam OpenCam')
        if self.cap.isOpened():
            self.thread_alive = True
            self.videocap_thread = threading.Thread(target=self._thread_cam)
            self.videocap_thread.start()
            return True
        else:
            self.cap = None
            print('DEBUG SingleCam OpenCam FAIL')
            return False

    def SnapShoot(self):
        if self.thread_alive:
            snap = self.frame_buffer[(self.frame_idx + 1) % 2]
            while snap is None:
                snap = self.frame_buffer[(self.frame_idx + 1) % 2]
            return snap
        else:
            return None

    def Close(self):
        self.thread_alive = False
        if self.cap is None:
            return
        if self.cap.isOpened():
            self.cap.release()
            self.cap = None


class StereoCam:
    def __init__(self,
                 cam_id0,
                 cam_id1,
                 cam_size=(1920, 1080),
                 cam_mode=cv2.CAP_DSHOW,
                 cam_fps=30,
                 exposure=-6,
                 bright=0):
        self.cam_id0 = cam_id0
        self.cam_id1 = cam_id1
        self.cap0 = None
        self.cap1 = None
        self.cam_size = cam_size
        self.cap_open_mode = cam_mode
        self.cam_fps = cam_fps
        self.exposure = exposure
        self.bright = bright

        self.frame_buffer = [None, None, None, None]
        self.frame_idx = 0
        self.thread_alive = False
        self.videocap_thread = None

    def _thread_cam(self):
        while self.thread_alive:
            ret, snap0 = self.cap0.read()
            ret, snap1 = self.cap1.read()
            self.frame_buffer[self.frame_idx] = snap0
            self.frame_buffer[self.frame_idx + 1] = snap1
            self.frame_idx = (self.frame_idx + 2) % 4
            # print('DEBUG FPS: ',
            #       self.cap0.get(cv2.CAP_PROP_FPS), ' brigh ', self.cap0.get(cv2.CAP_PROP_BRIGHTNESS), ' exp ',
            #       self.cap0.get(cv2.CAP_PROP_EXPOSURE), 'frame_size: ', self.cap0.get(cv2.CAP_PROP_FRAME_HEIGHT), ' ',
            #       self.cap0.get(cv2.CAP_PROP_FRAME_WIDTH), 'DEBUG2 FPS: ', self.cap1.get(cv2.CAP_PROP_FPS), ' brigh ',
            #       self.cap1.get(cv2.CAP_PROP_BRIGHTNESS), ' exp ', self.cap1.get(cv2.CAP_PROP_EXPOSURE), 'frame_size: ',
            #       self.cap1.get(cv2.CAP_PROP_FRAME_HEIGHT), ' ', self.cap1.get(cv2.CAP_PROP_FRAME_WIDTH), ' iso ',
            #       self.cap1.get(cv2.CAP_PROP_ISO_SPEED))

    def OpenCam(self, time_sleep=0):
        try:
            # print('TRY CLOSE')
            self.Close()
            # print('STEREO TRY CLOSE OK')
        except:
            print('CLOSE IN OpenCam ERROR')
            return False
        if time_sleep > 0:
            # print('SLEEP START')
            time.sleep(time_sleep)
            # print('SLEEP OVER')
        self.cap0 = cv2.VideoCapture(self.cam_id0 + self.cap_open_mode)
        self.cap0.open(self.cam_id0)
        self.cap0.set(cv2.CAP_PROP_FOURCC,
                      cv2.VideoWriter_fourcc('M', 'J', 'P', 'G'))
        self.cap0.set(cv2.CAP_PROP_FRAME_WIDTH, self.cam_size[0])
        self.cap0.set(cv2.CAP_PROP_FRAME_HEIGHT, self.cam_size[1])
        self.cap0.set(cv2.CAP_PROP_BRIGHTNESS, self.bright)
        self.cap0.set(cv2.CAP_PROP_EXPOSURE, self.exposure)
        self.cap0.set(cv2.CAP_PROP_FPS, self.cam_fps)
        self.cap1 = cv2.VideoCapture(self.cam_id1 + self.cap_open_mode)
        self.cap1.open(self.cam_id1)
        self.cap1.set(cv2.CAP_PROP_FOURCC,
                      cv2.VideoWriter_fourcc('M', 'J', 'P', 'G'))
        self.cap1.set(cv2.CAP_PROP_FRAME_WIDTH, self.cam_size[0])
        self.cap1.set(cv2.CAP_PROP_FRAME_HEIGHT, self.cam_size[1])
        self.cap1.set(cv2.CAP_PROP_BRIGHTNESS, self.bright)
        self.cap1.set(cv2.CAP_PROP_EXPOSURE, self.exposure)
        self.cap1.set(cv2.CAP_PROP_FPS, self.cam_fps)
        if self.cap0.isOpened() and self.cap1.isOpened():
            self.thread_alive = True
            self.videocap_thread = threading.Thread(target=self._thread_cam)
            self.videocap_thread.start()
            return True
        else:
            self.Close()
            return False

    def SnapShoot(self):
        if self.thread_alive:
            snap0 = self.frame_buffer[(self.frame_idx + 2) % 4]
            snap1 = self.frame_buffer[(self.frame_idx + 3) % 4]
            while snap0 is None or snap1 is None:
                snap0 = self.frame_buffer[(self.frame_idx + 2) % 4]
                snap1 = self.frame_buffer[(self.frame_idx + 3) % 4]
            return snap0, snap1
        else:
            return None, None

    def Close(self):
        self.thread_alive = False
        if self.cap0 is not None:
            if self.cap0.isOpened():
                self.cap0.release()
                self.cap0 = None
        if self.cap1 is not None:
            if self.cap1.isOpened():
                self.cap1.release()
                self.cap1 = None


if __name__ == '__main__':
    
    single_cam=SingleCam(cam_id=0,exposure=-6,bright=64)
    single_cam.OpenCam()
    while True:
        if not single_cam.cap.isOpened():
            print('not open')
            break
        snap=single_cam.SnapShoot()
        snap0=snap
        cv2.imshow('snap0', snap0)
        cv2.waitKey(1)
    exit()
    stereo_cam = StereoCam(cam_id0=2,
                           cam_id1=3,
                           cam_size=(640, 360),
                           cam_mode=cv2.CAP_DSHOW,
                           cam_fps=30,
                           bright=0,
                           exposure=-2)
    stereo_cam.OpenCam()
    while True:
        if not stereo_cam.cap1.isOpened():
            cv2.waitKey(3)
            print('wait...\n')
            continue
        snap0, snap1 = stereo_cam.SnapShoot()
        snap0 = cv2.resize(snap0, (512, 288))
        snap1 = cv2.resize(snap1, (512, 288))
        
        cv2.imshow('snap0', snap0)
        cv2.imshow('snap1', snap1)
        cv2.waitKey(1)