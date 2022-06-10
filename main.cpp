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

int COMMON_TEST() {
  int cycle = 10;
  int shifts = 4;
  cv::Size img_size = cv::Size(1920, 1080);
  std::vector<cv::Mat> phase_shift_patterns;
  zjlib::zjbase::PhasePattern::GeneratePhaseShiftPattern(
      cycle, shifts, img_size, &phase_shift_patterns,
      zjlib::zjbase::PHASE_PATTERN_MODE::RANDOM);
  int a = 0;
  std::cout << "happy_birds" << std::endl;
  return 0;
}

int main(int argc, char *argv[]) {
  std::cout << "OpenCV version : " << CV_VERSION << std::endl;
  std::cout << "Major version : " << CV_MAJOR_VERSION << std::endl;
  std::cout << "Minor version : " << CV_MINOR_VERSION << std::endl;
  std::cout << "Subminor version : " << CV_SUBMINOR_VERSION << std::endl;
  int cap_idx = 0;
  if (argc > 1) {
    cap_idx = std::stoi(argv[1]);
  }
  // YOLOX_TEST(cap_idx);
  COMMON_TEST();
  return 0;
}
