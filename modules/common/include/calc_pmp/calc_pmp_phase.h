/*
 * Copyright (c) 2022 ZJ.Bai All rights reserved
 * @author  : ZJ.Bai
 * @Date     : 2022-06-09
 * @Description
 */
#ifndef __COMMON_CALC_PMP__
#define __COMMON_CALC_PMP__
#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

#include "calc_pmp_phase_code.h"
namespace zjlib {
namespace zjbase {

/** @brief: common pattern based profilometry interface
 *
 * This class contain many base Func about pattern usually used
 *
 */
class PhasePattern {
 public:
  /**
   *  cv::Size camouflage_germ_size for build camouflage, only be set when
   * generate camouflage pattern
   */
  static int GeneratePhaseShiftPattern(
      const int cycle, const int shifts, cv::Size size,
      std::vector<cv::Mat>* phase_shift_pattern,
      int pattern_mode = PHASE_PATTERN_MODE::ALIGN_X,
      int pattern_norm = PHASE_PATTERN_NORM::NORM_MINMAX_0_255,
      bool make_phase_intact = false,
      cv::Size camouflage_germ_size = cv::Size(0, 0));

  static int CalcWarppedPhase(const std::vector<cv::Mat>& patterns_in,
                              cv::Mat* phase,
                              int pattern_mode = PHASE_PATTERN_MODE::ALIGN_X);

  static int MultiFreq3Unwarp(const std::vector<cv::Mat>& warpped_phases,
                              cv::Mat* unwarpped_phase,
                              int phase_unwarp_mode = PHASE_UNWARP_MODE::BASE);
  static int QuantRoundFloat2Float(std::vector<cv::Mat>* vecs);
  static float PHASE_PATTERN_RANGE;
};

}  // namespace zjbase
}  // namespace zjlib

#endif
