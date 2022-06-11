#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>

#include "calc_pmp/calc_pmp_phase.h"
#include "time.h"
#include "yolox.h"

int YOLOX_TEST(const int cap_idx) {
  cv::VideoCapture cap(cap_idx + cv::CAP_DSHOW);
  cap.open(cap_idx);
  cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
  cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, 360);
  cap.set(cv::CAP_PROP_FPS, 30);
  if (!cap.isOpened()) {
    std::cout << "open camera failed. " << std::endl;
    return -1;
  }
  cv::Mat frame;
  YoloX yolox(0.4f, false);
  while (true) {
    cap >> frame;
    if (!frame.empty()) {
      std::vector<Object> objects;
      yolox.Detect(frame, objects);
      yolox.Draw(frame, objects);
      cv::imshow("camera", frame);
    }

    if (cv::waitKey(1) > 0) {
      break;
    }
  }
}
#ifdef __COMMON_CALC_PMP_CODE__

int COMMON_TEST() {
  int cycle = 10;
  int shifts = 3;
  cv::Size img_size = cv::Size(1920, 1080);
  std::vector<cv::Mat> phase_shift_patterns;
  zjlib::zjbase::PhasePattern::GeneratePhaseShiftPattern(
      cycle, shifts, img_size, &phase_shift_patterns,
      zjlib::zjbase::PHASE_PATTERN_MODE::RANDOM);
  cv::normalize(phase_shift_patterns[0], phase_shift_patterns[0], 0, 255,
                cv::NormTypes::NORM_MINMAX);
  cv::normalize(phase_shift_patterns[1], phase_shift_patterns[1], 0, 255,
                cv::NormTypes::NORM_MINMAX);
  cv::normalize(phase_shift_patterns[2], phase_shift_patterns[2], 0, 255,
                cv::NormTypes::NORM_MINMAX);
  cv::Mat merge_phase, warpped_phase, merge_phase2, warpped_phase2;
  cv::merge(phase_shift_patterns, merge_phase);
  merge_phase.convertTo(merge_phase, CV_8UC3);
  cv::Mat scene = cv::imread(
      "D:\\github\\CV_lib_Cmake_framework\\modules\\common\\test\\calc_"
      "pmp\\scene.jpg");
  cv::resize(scene, scene, merge_phase.size(), 0, 0,
             cv::InterpolationFlags::INTER_AREA);
  merge_phase.convertTo(merge_phase, CV_32FC3);
  scene.convertTo(scene, CV_32FC3);
  merge_phase = (merge_phase + scene) / 2;

  merge_phase = cv::imread(
      "D:\\github\\CV_lib_Cmake_framework\\modules\\common\\test\\calc_"
      "pmp\\tele01.jpg");
  merge_phase2 = cv::imread(
      "D:\\github\\CV_lib_Cmake_framework\\modules\\common\\test\\calc_"
      "pmp\\tele02.jpg");
  merge_phase.convertTo(merge_phase, CV_32FC3);
  merge_phase2.convertTo(merge_phase2, CV_32FC3);
  cv::normalize(merge_phase, merge_phase, 0, 1, cv::NormTypes::NORM_MINMAX);
  zjlib::zjbase::PhasePattern::CalcWarppedPhase(merge_phase, &warpped_phase);
  cv::normalize(merge_phase2, merge_phase2, 0, 1, cv::NormTypes::NORM_MINMAX);
  zjlib::zjbase::PhasePattern::CalcWarppedPhase(merge_phase2, &warpped_phase2);
  int a = 0;
  std::cout << "happy_birds" << std::endl;
  return 0;
}
#endif

int main(int argc, char *argv[]) {
  std::cout << "OpenCV version : " << CV_VERSION << std::endl;
  std::cout << "Major version : " << CV_MAJOR_VERSION << std::endl;
  std::cout << "Minor version : " << CV_MINOR_VERSION << std::endl;
  std::cout << "Subminor version : " << CV_SUBMINOR_VERSION << std::endl;
  int cap_idx = 0;
  if (argc > 1) {
    cap_idx = std::stoi(argv[1]);
  }

#ifdef __COMMON_CALC_PMP_CODE__
  COMMON_TEST();
#else
  YOLOX_TEST(cap_idx);
#endif

  return 0;
}
