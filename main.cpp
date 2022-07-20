#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>

#include "base_op/base_op.h"
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
  cv::Mat w10_rgb = cv::imread(
      "D:\\github\\CV_lib_Cmake_framework\\modules\\common\\doc\\calc_"
      "pmp\\snap\\mems\\test\\w10.png");
  cv::Mat w20_rgb = cv::imread(
      "D:\\github\\CV_lib_Cmake_framework\\modules\\common\\doc\\calc_"
      "pmp\\snap\\mems\\test\\w20.png");
  cv::Mat phase_w10_rgb = cv::imread(
      "D:\\github\\CV_lib_Cmake_framework\\modules\\common\\doc\\calc_"
      "pmp\\snap\\mems\\test\\phase_w11_remap.png");
  cv::Mat phase_w20_rgb = cv::imread(
      "D:\\github\\CV_lib_Cmake_framework\\modules\\common\\doc\\calc_"
      "pmp\\snap\\mems\\test\\phase_w21_remap.png");
  cv::Mat rgb_w10_rgb = cv::imread(
      "D:\\github\\CV_lib_Cmake_framework\\modules\\common\\doc\\calc_"
      "pmp\\snap\\mems\\test\\rgb_w11_remap.png");
  cv::Mat rgb_w20_rgb = cv::imread(
      "D:\\github\\CV_lib_Cmake_framework\\modules\\common\\doc\\calc_"
      "pmp\\snap\\mems\\test\\rgb_w21_remap.png");
  if (0) {
    // ������֤��3֡�Ķ�Ƶ����ڽ���ʱ�򲻿���
    cv::Mat merge_phase, warpped_phase, merge_phase2, warpped_phase2,
        merge_phase3, warpped_phase3;
    int cycle = 10;
    int shifts = 3;
    cv::Size img_size = cv::Size(1920, 1080);
    std::vector<cv::Mat> phase_shift_patterns;
    phase_shift_patterns.clear();
    zjlib::zjbase::PhasePattern::GeneratePhaseShiftPattern(
        53, shifts, img_size, &phase_shift_patterns,
        zjlib::zjbase::PHASE_PATTERN_MODE::ALIGN_X,
        zjlib::zjbase::PHASE_PATTERN_NORM::ORG_OUT);
    zjlib::zjbase::PhasePattern::CalcWarppedPhase(phase_shift_patterns,
                                                  &warpped_phase);

    phase_shift_patterns.clear();
    zjlib::zjbase::PhasePattern::GeneratePhaseShiftPattern(
        47, shifts, img_size, &phase_shift_patterns,
        zjlib::zjbase::PHASE_PATTERN_MODE::ALIGN_X,
        zjlib::zjbase::PHASE_PATTERN_NORM::ORG_OUT);
    zjlib::zjbase::PhasePattern::CalcWarppedPhase(phase_shift_patterns,
                                                  &warpped_phase2);

    phase_shift_patterns.clear();
    zjlib::zjbase::PhasePattern::GeneratePhaseShiftPattern(
        43, shifts, img_size, &phase_shift_patterns,
        zjlib::zjbase::PHASE_PATTERN_MODE::ALIGN_X,
        zjlib::zjbase::PHASE_PATTERN_NORM::ORG_OUT);
    zjlib::zjbase::PhasePattern::CalcWarppedPhase(phase_shift_patterns,
                                                  &warpped_phase3);
    std::vector<cv::Mat> phases;
    phases.push_back(warpped_phase);
    phases.push_back(warpped_phase2);
    phases.push_back(warpped_phase3);
    cv::Mat unwarpped_phase;
    zjlib::zjbase::PhasePattern::MultiFreq3Unwarp(
        phases, &unwarpped_phase, zjlib::zjbase::PHASE_UNWARP_MODE::BASE);
    int a = 0;
  }
  {
    std::string base_path =
        "D:\\github\\CV_lib_Cmake_framework\\modules\\common\\doc\\calc_"
        "pmp\\snap\\mems\\";
    cv::Mat w00 = cv::imread(base_path + "w00.jpg");
    cv::Mat w01 = cv::imread(base_path + "w01.jpg");
    cv::Mat w10 = cv::imread(base_path + "w10.jpg");
    cv::Mat w11 = cv::imread(base_path + "w11.jpg");
    cv::Mat w20 = cv::imread(base_path + "w20.jpg");
    cv::Mat w21 = cv::imread(base_path + "w21.jpg");

    cv::resize(w00, w00, cv::Size(0, 0), 0.4, 0.4);
    cv::resize(w10, w10, cv::Size(0, 0), 0.4, 0.4);
    cv::resize(w01, w01, cv::Size(0, 0), 0.4, 0.4);
    cv::resize(w11, w11, cv::Size(0, 0), 0.4, 0.4);
    cv::resize(w20, w20, cv::Size(0, 0), 0.4, 0.4);
    cv::resize(w21, w21, cv::Size(0, 0), 0.4, 0.4);

    cv::imwrite(base_path + "test\\w00.png", w00);
    cv::imwrite(base_path + "test\\w01.png", w01);
    cv::imwrite(base_path + "test\\w10.png", w10);
    cv::imwrite(base_path + "test\\w11.png", w11);
    cv::imwrite(base_path + "test\\w20.png", w20);
    cv::imwrite(base_path + "test\\w21.png", w21);

    w00.convertTo(w00, CV_32FC3);
    w01.convertTo(w01, CV_32FC3);
    w20.convertTo(w20, CV_32FC3);
    w21.convertTo(w21, CV_32FC3);
    w10.convertTo(w10, CV_32FC3);
    w11.convertTo(w11, CV_32FC3);
    std::vector<cv::Mat> ww00, ww01, ww10, ww11, ww20, ww21;
    cv::split(w00, ww00);
    cv::split(w01, ww01);
    cv::split(w10, ww10);
    cv::split(w11, ww11);
    cv::split(w20, ww20);
    cv::split(w21, ww21);
    cv::Mat warpped_phase_w00, warpped_phase_w01, warpped_phase_w10,
        warpped_phase_w11, warpped_phase_w20, warpped_phase_w21;
    zjlib::zjbase::PhasePattern::CalcWarppedPhase(ww00, &warpped_phase_w00);
    zjlib::zjbase::PhasePattern::CalcWarppedPhase(ww01, &warpped_phase_w01);
    zjlib::zjbase::PhasePattern::CalcWarppedPhase(ww10, &warpped_phase_w10);
    zjlib::zjbase::PhasePattern::CalcWarppedPhase(ww11, &warpped_phase_w11);
    zjlib::zjbase::PhasePattern::CalcWarppedPhase(ww20, &warpped_phase_w20);
    zjlib::zjbase::PhasePattern::CalcWarppedPhase(ww21, &warpped_phase_w21);
    warpped_phase_w00 = warpped_phase_w00 / CV_PI * 255;
    warpped_phase_w01 = warpped_phase_w01 / CV_PI * 255;
    warpped_phase_w20 = warpped_phase_w20 / CV_PI * 255;
    warpped_phase_w21 = warpped_phase_w21 / CV_PI * 255;
    warpped_phase_w10 = warpped_phase_w10 / CV_PI * 255;
    warpped_phase_w11 = warpped_phase_w11 / CV_PI * 255;
    warpped_phase_w00.convertTo(warpped_phase_w00, CV_8UC1);
    warpped_phase_w21.convertTo(warpped_phase_w21, CV_8UC1);
    warpped_phase_w01.convertTo(warpped_phase_w01, CV_8UC1);
    warpped_phase_w10.convertTo(warpped_phase_w10, CV_8UC1);
    warpped_phase_w11.convertTo(warpped_phase_w11, CV_8UC1);
    warpped_phase_w20.convertTo(warpped_phase_w20, CV_8UC1);
    cv::imwrite(base_path + "test\\w00_phase.png", warpped_phase_w00);
    cv::imwrite(base_path + "test\\w01_phase.png", warpped_phase_w01);
    cv::imwrite(base_path + "test\\w10_phase.png", warpped_phase_w10);
    cv::imwrite(base_path + "test\\w11_phase.png", warpped_phase_w11);
    cv::imwrite(base_path + "test\\w20_phase.png", warpped_phase_w20);
    cv::imwrite(base_path + "test\\w21_phase.png", warpped_phase_w21);
    int a = 0;
  }
  {
    cv::Mat merge_phase, warpped_phase, merge_phase2, warpped_phase2,
        merge_phase3, warpped_phase3, warpped_phase_camouflage,
        warpped_phase_camouflage2, merge_phase_camouflage,
        merge_phase_camouflage2, merge_phase_speckle, warpped_phase_speckle;
    int cycle = 10;
    int shifts = 4;
    cv::Size img_size = cv::Size(1280, 720);
    std::vector<cv::Mat> phase_shift_patterns, phase_shift_patterns2,
        phase_shift_patterns3, phase_shift_patterns_camouflage,
        phase_shift_speckle, patterns_all;

    // make camouflage pattern
    zjlib::zjbase::PhasePattern::GeneratePhaseShiftPattern(
        70, 3, img_size, &phase_shift_patterns_camouflage,
        zjlib::zjbase::PHASE_PATTERN_MODE::CAMOUFLAGE,
        zjlib::zjbase::PHASE_PATTERN_NORM::NORM_MINMAX_0_255, false,
        img_size / 10);
    zjlib::zjbase::PhasePattern::QuantRoundFloat2Float(
        &phase_shift_patterns_camouflage);
    zjlib::zjbase::PhasePattern::CalcWarppedPhase(
        phase_shift_patterns_camouflage, &warpped_phase_camouflage);
    cv::merge(phase_shift_patterns_camouflage, merge_phase_camouflage);
    zjlib::zjbase::BaseOP::MatVecAppend(phase_shift_patterns_camouflage,
                                        &patterns_all);
    std::vector<cv::Mat> merges_camouflage;
    zjlib::zjbase::BaseOP::A32Merge(phase_shift_patterns_camouflage,
                                    &merges_camouflage);
    zjlib::zjbase::BaseOP::MatVecAppend(merges_camouflage, &patterns_all);
    // make camouflage pattern2
    phase_shift_patterns_camouflage.clear();
    zjlib::zjbase::PhasePattern::GeneratePhaseShiftPattern(
        70, 3, img_size, &phase_shift_patterns_camouflage,
        zjlib::zjbase::PHASE_PATTERN_MODE::CAMOUFLAGE,
        zjlib::zjbase::PHASE_PATTERN_NORM::NORM_MINMAX_0_255, false,
        img_size / 5);
    zjlib::zjbase::PhasePattern::QuantRoundFloat2Float(
        &phase_shift_patterns_camouflage);
    zjlib::zjbase::PhasePattern::CalcWarppedPhase(
        phase_shift_patterns_camouflage, &warpped_phase_camouflage2);
    cv::merge(phase_shift_patterns_camouflage, merge_phase_camouflage2);
    zjlib::zjbase::BaseOP::MatVecAppend(phase_shift_patterns_camouflage,
                                        &patterns_all);
    std::vector<cv::Mat> merges_camouflage2;
    zjlib::zjbase::BaseOP::A32Merge(phase_shift_patterns_camouflage,
                                    &merges_camouflage2);
    zjlib::zjbase::BaseOP::MatVecAppend(merges_camouflage2, &patterns_all);

    // make speckle pattern
    phase_shift_speckle.clear();
    zjlib::zjbase::PhasePattern::GeneratePhaseShiftPattern(
        59, 3, img_size, &phase_shift_speckle,
        zjlib::zjbase::PHASE_PATTERN_MODE::RANDOM,
        zjlib::zjbase::PHASE_PATTERN_NORM::NORM_MINMAX_0_255);
    zjlib::zjbase::PhasePattern::QuantRoundFloat2Float(&phase_shift_speckle);
    zjlib::zjbase::PhasePattern::CalcWarppedPhase(phase_shift_speckle,
                                                  &warpped_phase_speckle);
    cv::merge(phase_shift_speckle, merge_phase_speckle);
    zjlib::zjbase::BaseOP::MatVecAppend(phase_shift_speckle, &patterns_all);
    patterns_all.push_back(merge_phase_speckle);

    // make multi_freq1 pattern
    phase_shift_patterns.clear();
    zjlib::zjbase::PhasePattern::GeneratePhaseShiftPattern(
        70, shifts, img_size, &phase_shift_patterns,
        zjlib::zjbase::PHASE_PATTERN_MODE::ALIGN_X,
        zjlib::zjbase::PHASE_PATTERN_NORM::NORM_MINMAX_0_255);
    zjlib::zjbase::PhasePattern::QuantRoundFloat2Float(&phase_shift_patterns);
    zjlib::zjbase::PhasePattern::CalcWarppedPhase(phase_shift_patterns,
                                                  &warpped_phase);
    cv::merge(phase_shift_patterns, merge_phase);
    zjlib::zjbase::BaseOP::MatVecAppend(phase_shift_patterns, &patterns_all);
    // make multi_freq2 pattern
    phase_shift_patterns2.clear();
    zjlib::zjbase::PhasePattern::GeneratePhaseShiftPattern(
        64, shifts, img_size, &phase_shift_patterns2,
        zjlib::zjbase::PHASE_PATTERN_MODE::ALIGN_X,
        zjlib::zjbase::PHASE_PATTERN_NORM::NORM_MINMAX_0_255);
    zjlib::zjbase::PhasePattern::QuantRoundFloat2Float(&phase_shift_patterns2);
    zjlib::zjbase::PhasePattern::CalcWarppedPhase(phase_shift_patterns2,
                                                  &warpped_phase2);
    cv::merge(phase_shift_patterns2, merge_phase2);
    zjlib::zjbase::BaseOP::MatVecAppend(phase_shift_patterns2, &patterns_all);
    // make multi_freq3 pattern
    phase_shift_patterns3.clear();
    zjlib::zjbase::PhasePattern::GeneratePhaseShiftPattern(
        59, shifts, img_size, &phase_shift_patterns3,
        zjlib::zjbase::PHASE_PATTERN_MODE::ALIGN_X,
        zjlib::zjbase::PHASE_PATTERN_NORM::NORM_MINMAX_0_255);
    zjlib::zjbase::PhasePattern::QuantRoundFloat2Float(&phase_shift_patterns3);
    zjlib::zjbase::PhasePattern::CalcWarppedPhase(phase_shift_patterns3,
                                                  &warpped_phase3);
    cv::merge(phase_shift_patterns3, merge_phase3);
    zjlib::zjbase::BaseOP::MatVecAppend(phase_shift_patterns3, &patterns_all);

    std::vector<cv::Mat> phases;
    phases.push_back(warpped_phase);
    phases.push_back(warpped_phase2);
    phases.push_back(warpped_phase3);
    cv::Mat unwarpped_phase;
    zjlib::zjbase::PhasePattern::MultiFreq3Unwarp(
        phases, &unwarpped_phase, zjlib::zjbase::PHASE_UNWARP_MODE::BASE);

    zjlib::zjbase::BaseOP::MatVecSave(
        patterns_all, "D:\\github\\CV_lib_Cmake_framework\\out\\",
        "_pattern.png");
    int a = 0;
  }

  std::cout << "happy_birds" << std::endl;
  return 0;
}
#endif

int main(int argc, char* argv[]) {
  std::cout << "OpenCV version : " << CV_VERSION << std::endl;
  std::cout << "Major version : " << CV_MAJOR_VERSION << std::endl;
  std::cout << "Minor version : " << CV_MINOR_VERSION << std::endl;
  std::cout << "Subminor version : " << CV_SUBMINOR_VERSION << std::endl;
  int cap_idx = 0;
  if (argc > 1) {
    cap_idx = std::stoi(argv[1]);
  }
#ifdef __COMMON_CALC_PMP_CODE__
  // COMMON_TEST();

#else
  YOLOX_TEST(cap_idx);
#endif

  return 0;
}
