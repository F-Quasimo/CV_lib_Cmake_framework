# Copyright (c) 2022 ZJ.Bai All rights reserved
# @author  : ZJ.Bai
# @Date     : 2022-06-09
# @Description

import argparse

parser = argparse.ArgumentParser(description='videoio_tools')
parser.add_argument('--imgpath', default='', help='path to imgs')
parser.add_argument('--imgkeys', default='.jpg', help='filter the files')
parser.add_argument('--save_folder', default='', help='end with .mp4')
parser.add_argument('--fps', default=30, type=int, help='fps in video')

class VideoGen:
    def __init__(self, img_list)