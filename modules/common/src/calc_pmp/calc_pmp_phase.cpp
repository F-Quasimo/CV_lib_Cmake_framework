/*
 * Copyright (c) 2022 ZJ.Bai All rights reserved
 * @author  : ZJ.Bai
 * @Date     : 2022-06-09
 * @Description
 */
#include "calc_pmp/calc_pmp_phase.h"
namespace zjlib {
namespace zjbase {
float PhasePattern::PHASE_PATTERN_RANGE = 127.0;

int PhasePattern::GeneratePhaseShiftPattern(
    const int cycle, const int shifts, cv::Size size,
    std::vector<cv::Mat>* phase_shift_pattern, int pattern_mode,
    int pattern_norm, bool make_phase_intact, cv::Size camouflage_germ_size) {
  if (phase_shift_pattern->size() > 0 || size.width * size.height == 0) {
    return PMP_ERROR;
  }
  if (pattern_mode == PHASE_PATTERN_MODE::ALIGN_X) {
    float phase_step_per_pixel =
        static_cast<float>(2 * CV_PI * cycle / size.width);
    float frame_phase_shift = static_cast<float>(2 * CV_PI / shifts);
    for (int i = 0; i < shifts; i++) {
      cv::Mat tmp_pattern = cv::Mat::zeros(size, CV_32FC1);
      for (int col_idx = 0; col_idx < size.width; col_idx++) {
        float phase =
            PHASE_PATTERN_RANGE *
            (sin(phase_step_per_pixel * (col_idx) + i * frame_phase_shift));
        tmp_pattern.colRange(col_idx, col_idx + 1) = phase;
      }
      phase_shift_pattern->push_back(tmp_pattern);
    }
    if (make_phase_intact) {
      // make cycle greate
      cv::Mat phase;
      CalcWarppedPhase(*phase_shift_pattern, &phase, pattern_mode);
      int idx_c = phase.cols - 1;
      for (; idx_c > 0; idx_c--) {
        if (phase.ptr<float>(0)[idx_c] > phase.ptr<float>(0)[0]) {
          break;
        }
      }
      if (idx_c != phase.cols - 1) {
        for (int idx_phase = 0; idx_phase < phase_shift_pattern->size();
             idx_phase++) {
          cv::Mat tmp_pattern = cv::Mat::zeros(
              (*phase_shift_pattern)[idx_phase].size(), CV_32FC1);
          (*phase_shift_pattern)[idx_phase]
              .colRange(idx_c + 1, (*phase_shift_pattern)[idx_phase].cols)
              .copyTo(tmp_pattern.colRange(
                  0, (*phase_shift_pattern)[idx_phase].cols - idx_c - 1));
          (*phase_shift_pattern)[idx_phase]
              .colRange(0, idx_c + 1)
              .copyTo(tmp_pattern.colRange(
                  (*phase_shift_pattern)[idx_phase].cols - idx_c - 1,
                  (*phase_shift_pattern)[idx_phase].cols));
          (*phase_shift_pattern)[idx_phase] = tmp_pattern;
        }
      }
    }

  } else if (pattern_mode == PHASE_PATTERN_MODE::ALIGN_Y) {
    float phase_step_per_pixel =
        static_cast<float>(2 * CV_PI * cycle / size.height);
    float frame_phase_shift = static_cast<float>(2 * CV_PI / shifts);
    for (int i = 0; i < shifts; i++) {
      cv::Mat tmp_pattern = cv::Mat::zeros(size, CV_32FC1);
      for (int row_idx = 0; row_idx < size.height; row_idx++) {
        float phase =
            PHASE_PATTERN_RANGE *
            (sin(phase_step_per_pixel * (row_idx + 1) + i * frame_phase_shift));
        tmp_pattern.rowRange(row_idx, row_idx + 1) = phase;
      }
      phase_shift_pattern->push_back(tmp_pattern);
    }
    if (make_phase_intact) {
      // not ready because it is not emergency for now
      // make cycle greate
      cv::Mat phase;
      CalcWarppedPhase(*phase_shift_pattern, &phase, pattern_mode);
    }
  } else if (pattern_mode == PHASE_PATTERN_MODE::RANDOM) {
    float phase_step_per_pixel =
        static_cast<float>(2 * CV_PI * cycle / size.height);
    float frame_phase_shift = static_cast<float>(2 * CV_PI / shifts);
    cv::Mat base_idx = cv::Mat::zeros(size, CV_32FC1);
    for (int row_idx = 0; row_idx < size.height; row_idx++) {
      float phase = PHASE_PATTERN_RANGE * sin(phase_step_per_pixel * row_idx);
      base_idx.rowRange(row_idx, row_idx + 1) = row_idx;
    }
    cv::randShuffle(base_idx, 2);
    // phase_shift_pattern->push_back(base_pattern);
    for (int i = 0; i < shifts; i++) {
      cv::Mat tmp_pattern = cv::Mat::zeros(size, CV_32FC1);
      for (int row_idx = 0; row_idx < size.height; row_idx++) {
        float* ptr_base = base_idx.ptr<float>(row_idx);
        float* ptr_tmp_pattern = tmp_pattern.ptr<float>(row_idx);
        for (int col_idx = 0; col_idx < size.width; col_idx++) {
          ptr_tmp_pattern[col_idx] =
              PHASE_PATTERN_RANGE *
              sin(phase_step_per_pixel * ptr_base[col_idx] +
                  i * frame_phase_shift);
        }
      }
      phase_shift_pattern->push_back(tmp_pattern);
    }
  } else if (pattern_mode == PHASE_PATTERN_MODE::CAMOUFLAGE) {
    // TBD
    std::vector<cv::Mat> germs;
    GeneratePhaseShiftPattern(cycle, 1, camouflage_germ_size, &germs,
                              PHASE_PATTERN_MODE::RANDOM);
    cv::Mat germ, phase, phase_2pi;
    int radius = static_cast<int>(size.width / camouflage_germ_size.width);
    radius = radius % 2 == 0 ? radius + 1 : radius;
    cv::resize(germs[0], germ, size, 0, 0,
               cv::InterpolationFlags::INTER_NEAREST);
    cv::Mat kernel = cv::getStructuringElement(cv::MorphShapes::MORPH_ELLIPSE,
                                               cv::Size(radius, radius));
    kernel.convertTo(kernel, CV_32FC1);
    cv::filter2D(germ, germ, CV_32F, kernel);
    cv::normalize(germ, germ, -1, 1, cv::NormTypes::NORM_MINMAX);
    phase = cv::Mat::zeros(germ.size(), CV_32FC1);
    for (int row_idx = 0; row_idx < size.height; row_idx++) {
      float* ptr_base = germ.ptr<float>(row_idx);
      float* ptr_phase = phase.ptr<float>(row_idx);
      for (int col_idx = 0; col_idx < size.width; col_idx++) {
        ptr_phase[col_idx] = asin(ptr_base[col_idx]);
      }
    }
    phase_2pi = phase + CV_PI;
    float frame_phase_shift = static_cast<float>(2 * CV_PI / shifts);
    for (int i = 0; i < shifts; i++) {
      cv::Mat tmp_pattern = cv::Mat::zeros(size, CV_32FC1);
      for (int row_idx = 0; row_idx < size.height; row_idx++) {
        float* ptr_base = phase_2pi.ptr<float>(row_idx);
        float* ptr_tmp_pattern = tmp_pattern.ptr<float>(row_idx);
        for (int col_idx = 0; col_idx < size.width; col_idx++) {
          ptr_tmp_pattern[col_idx] =
              PHASE_PATTERN_RANGE *
              sin(ptr_base[col_idx] + i * frame_phase_shift);
        }
      }
      phase_shift_pattern->push_back(tmp_pattern);
    }
    int a = 0;
  }

  if (pattern_norm == PHASE_PATTERN_NORM::ORG_OUT) {
    return PMP_SUCCESS;
  } else if (pattern_norm == PHASE_PATTERN_NORM::NORM_MINMAX_0_255) {
    for (int i = 0; i < phase_shift_pattern->size(); i++) {
      cv::normalize((*phase_shift_pattern)[i], (*phase_shift_pattern)[i], 0,
                    255, cv::NormTypes::NORM_MINMAX);
    }
  } else {
    return PHASE_PATTERN_NORM_TBD;
  }

  return PMP_SUCCESS;
}

int PhasePattern::CalcWarppedPhase(const std::vector<cv::Mat>& patterns_in,
                                   cv::Mat* phase, int pattern_mode) {
  if (patterns_in.size() == 0 ||
      (pattern_mode != PHASE_PATTERN_MODE::ALIGN_X &&
       pattern_mode != PHASE_PATTERN_MODE::ALIGN_Y)) {
    return PMP_ERROR;
  }
  if (CV_MAT_DEPTH(patterns_in[0].type()) != CV_32F) {
    return PMP_ERROR;
  }
  if (pattern_mode == PHASE_PATTERN_MODE::ALIGN_Y) {
    std::vector<cv::Mat> patterns_in_y;
    for (int i = 0; i < patterns_in.size(); i++) {
      patterns_in_y.push_back(patterns_in[i]);
      cv::transpose(patterns_in_y[i], patterns_in_y[i]);
    }
    int ret =
        CalcWarppedPhase(patterns_in_y, phase, PHASE_PATTERN_MODE::ALIGN_X);
    if (ret == PMP_SUCCESS) {
      cv::transpose(*phase, *phase);
    }
    return ret;
  }
  if (patterns_in.size() == 4) {
    cv::Mat pattern0 = patterns_in[0];
    cv::Mat pattern1 = patterns_in[1];
    cv::Mat pattern2 = patterns_in[2];
    cv::Mat pattern3 = patterns_in[3];
    *phase = cv::Mat::zeros(pattern0.size(), CV_32FC1);
    for (int row_idx = 0; row_idx < pattern0.rows; row_idx++) {
      float* ptr_pattern0 = (float*)pattern0.ptr<float>(row_idx);
      float* ptr_pattern1 = (float*)pattern1.ptr<float>(row_idx);
      float* ptr_pattern2 = (float*)pattern2.ptr<float>(row_idx);
      float* ptr_pattern3 = (float*)pattern3.ptr<float>(row_idx);
      float* ptr_phase = phase->ptr<float>(row_idx);
      for (int col_idx = 0; col_idx < pattern0.cols; col_idx++) {
        ptr_phase[col_idx] =
            atan2(static_cast<float>(ptr_pattern3[col_idx]) -
                      static_cast<float>(ptr_pattern1[col_idx]),
                  static_cast<float>(ptr_pattern0[col_idx]) -
                      static_cast<float>(ptr_pattern2[col_idx])) +
            CV_PI;
      }
    }
  } else if (patterns_in.size() == 3) {
    cv::Mat pattern0 = patterns_in[0];
    cv::Mat pattern1 = patterns_in[1];
    cv::Mat pattern2 = patterns_in[2];
    *phase = cv::Mat::zeros(pattern0.size(), CV_32FC1);
    for (int row_idx = 0; row_idx < pattern0.rows; row_idx++) {
      float* ptr_pattern0 = (float*)pattern0.ptr<float>(row_idx);
      float* ptr_pattern1 = (float*)pattern1.ptr<float>(row_idx);
      float* ptr_pattern2 = (float*)pattern2.ptr<float>(row_idx);
      float* ptr_phase = phase->ptr<float>(row_idx);
      for (int col_idx = 0; col_idx < pattern0.cols; col_idx++) {
        ptr_phase[col_idx] =
            atan2(2 * static_cast<float>(ptr_pattern1[col_idx]) -
                      static_cast<float>(ptr_pattern0[col_idx]) -
                      static_cast<float>(ptr_pattern2[col_idx]),
                  static_cast<float>(ptr_pattern2[col_idx]) -
                      static_cast<float>(ptr_pattern0[col_idx])) +
            CV_PI;
      }
    }
  }
  return PMP_SUCCESS;
}

int PhasePattern::MultiFreq3Unwarp(const std::vector<cv::Mat>& warpped_phases,
                                   cv::Mat* unwarpped_phase,
                                   int phase_unwarp_mode) {
  if (warpped_phases.size() == 0) {
    return PMP_ERROR;
  }
  if (CV_MAT_DEPTH(warpped_phases[0].type()) != CV_32F) {
    return PMP_ERROR;
  }
  if (phase_unwarp_mode == PHASE_UNWARP_MODE::BASE &&
      warpped_phases.size() == 3) {
    cv::Mat phase_sub_01 = cv::Mat::zeros(warpped_phases[0].size(), CV_32FC1);
    cv::Mat phase_sub_12 = cv::Mat::zeros(warpped_phases[0].size(), CV_32FC1);
    cv::Mat phase_sub = cv::Mat::zeros(warpped_phases[0].size(), CV_32FC1);
    for (int row_idx = 0; row_idx < phase_sub.rows; row_idx++) {
      float* ptr_warp_0 = (float*)warpped_phases[0].ptr<float>(row_idx);
      float* ptr_warp_1 = (float*)warpped_phases[1].ptr<float>(row_idx);
      float* ptr_warp_2 = (float*)warpped_phases[2].ptr<float>(row_idx);
      float* ptr_sub_01 = phase_sub_01.ptr<float>(row_idx);
      float* ptr_sub_12 = phase_sub_12.ptr<float>(row_idx);
      for (int col_idx = 0; col_idx < phase_sub.cols; col_idx++) {
        if (ptr_warp_0[col_idx] > ptr_warp_1[col_idx]) {
          ptr_sub_01[col_idx] = ptr_warp_0[col_idx] - ptr_warp_1[col_idx];
        } else {
          ptr_sub_01[col_idx] = static_cast<float>(
              ptr_warp_0[col_idx] - ptr_warp_1[col_idx] + 2 * CV_PI);
        }
        if (ptr_warp_1[col_idx] > ptr_warp_2[col_idx]) {
          ptr_sub_12[col_idx] = ptr_warp_1[col_idx] - ptr_warp_2[col_idx];
        } else {
          ptr_sub_12[col_idx] = static_cast<float>(
              ptr_warp_1[col_idx] - ptr_warp_2[col_idx] + 2 * CV_PI);
        }
      }
    }
    /*
    cv::Mat phase_sub_01_tmp = cv::Mat::zeros(phase_sub_01.size(), CV_32FC1);
    phase_sub_01.colRange(1, phase_sub_01.cols)
        .copyTo(phase_sub_01_tmp.colRange(0, phase_sub_01_tmp.cols - 1));
    phase_sub_01.colRange(0, 1).copyTo(phase_sub_01_tmp.colRange(
        phase_sub_01_tmp.cols - 1, phase_sub_01_tmp.cols));
     */
    for (int row_idx = 0; row_idx < phase_sub.rows; row_idx++) {
      float* ptr_sub_01 = phase_sub_01.ptr<float>(row_idx);
      float* ptr_sub_12 = phase_sub_12.ptr<float>(row_idx);
      float* ptr_sub = phase_sub.ptr<float>(row_idx);
      for (int col_idx = 0; col_idx < phase_sub.cols; col_idx++) {
        if (ptr_sub_01[col_idx] > ptr_sub_12[col_idx]) {
          ptr_sub[col_idx] = ptr_sub_01[col_idx] - ptr_sub_12[col_idx];
        } else {
          ptr_sub[col_idx] =
              ptr_sub_01[col_idx] - ptr_sub_12[col_idx] + 2 * CV_PI;
        }
      }
    }
    *unwarpped_phase = phase_sub;
  } else if (phase_unwarp_mode == PHASE_UNWARP_MODE::FREQ_DIV &&
             warpped_phases.size() == 3) {
    return BASE_TBD;
    // 不好用，等完成
    cv::Mat phase_cycle = cv::Mat::zeros(warpped_phases[0].size(), CV_32FC1);
    cv::Mat phase_sub = cv::Mat::zeros(warpped_phases[0].size(), CV_32FC1);
    for (int row_idx = 0; row_idx < phase_sub.rows; row_idx++) {
      float* ptr_warp_0 = (float*)warpped_phases[0].ptr<float>(row_idx);
      float* ptr_warp_1 = (float*)warpped_phases[1].ptr<float>(row_idx);
      float* ptr_warp_2 = (float*)warpped_phases[2].ptr<float>(row_idx);
      float* ptr_phase_cycle = phase_cycle.ptr<float>(row_idx);
      for (int col_idx = 0; col_idx < phase_sub.cols; col_idx++) {
        ptr_phase_cycle[col_idx] =
            ((ptr_warp_1[col_idx] * 70 / 64 - ptr_warp_0[col_idx]) /
             (2 * CV_PI));
      }
    }
    int a = 0;
  }
  return PMP_SUCCESS;
}

int PhasePattern::QuantRoundFloat2Float(std::vector<cv::Mat>* vecs) {
  if (vecs->size() == 0) {
    return PMP_ERROR;
  }
  if (CV_MAT_DEPTH((*vecs)[0].type()) != CV_32F) {
    return PMP_ERROR;
  }
  for (int i = 0; i < vecs->size(); i++) {
    (*vecs)[i].convertTo((*vecs)[i], CV_8UC1);
    (*vecs)[i].convertTo((*vecs)[i], CV_32FC1);
  }
}
}  // namespace zjbase
}  // namespace zjlib
