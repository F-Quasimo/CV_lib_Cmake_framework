/*
 * Copyright (c) 2022 ZJ.Bai All rights reserved
 * @author  : ZJ.Bai
 * @Date     : 2022-06-09
 * @Description
 */
#ifndef __COMMON_BASE_OP__
#define __COMMON_BASE_OP__
#include <opencv2/core.hpp>
#include <string>
#include <vector>

namespace zjlib {
namespace zjbase {
class BaseOP {
 public:
  static void MatVecAppend(const std::vector<cv::Mat>& vec,
                           std::vector<cv::Mat>* target_vecs) {
    if (vec.size() == 0) {
      return;
    }
    for (int i = 0; i < vec.size(); i++) {
      target_vecs->push_back(vec[i]);
    }
    return;
  }

  static void MatVecSave(const std::vector<cv::Mat>& vec, std::string save_path,
                         std::string suffix, int type = CV_8U) {
    if (vec.size() == 0) {
      return;
    }
    int n_zero = 6;
    for (int i = 0; i < vec.size(); i++) {
      cv::Mat tmp_mat;
      vec[i].convertTo(tmp_mat, CV_8U);
      std::string idx = std::to_string(i);
      std::string new_str =
          save_path + std::string(n_zero - idx.length(), '0') + idx + suffix;
      cv::imwrite(new_str, tmp_mat);
    }
    return;
  }

  static void A32Merge(const std::vector<cv::Mat>& vec,
                       std::vector<cv::Mat>* target_vecs) {
    if (vec.size() != 3) {
      return;
    }
    target_vecs->clear();
    std::vector<cv::Mat> a0;
    cv::Mat merge_a0;
    a0.push_back(vec[0]);
    a0.push_back(vec[1]);
    a0.push_back(vec[2]);
    cv::merge(a0, merge_a0);
    target_vecs->push_back(merge_a0);

    std::vector<cv::Mat> a1;
    cv::Mat merge_a1;
    a1.push_back(vec[0]);
    a1.push_back(vec[2]);
    a1.push_back(vec[1]);
    cv::merge(a1, merge_a1);
    target_vecs->push_back(merge_a1);

    std::vector<cv::Mat> a2;
    cv::Mat merge_a2;
    a2.push_back(vec[1]);
    a2.push_back(vec[0]);
    a2.push_back(vec[2]);
    cv::merge(a2, merge_a2);
    target_vecs->push_back(merge_a2);

    std::vector<cv::Mat> a3;
    cv::Mat merge_a3;
    a3.push_back(vec[1]);
    a3.push_back(vec[2]);
    a3.push_back(vec[0]);
    cv::merge(a3, merge_a3);
    target_vecs->push_back(merge_a3);

    std::vector<cv::Mat> a4;
    cv::Mat merge_a4;
    a4.push_back(vec[2]);
    a4.push_back(vec[0]);
    a4.push_back(vec[1]);
    cv::merge(a4, merge_a4);
    target_vecs->push_back(merge_a4);

    std::vector<cv::Mat> a5;
    cv::Mat merge_a5;
    a5.push_back(vec[2]);
    a5.push_back(vec[1]);
    a5.push_back(vec[0]);
    cv::merge(a5, merge_a5);
    target_vecs->push_back(merge_a5);

    return;
  }
};

}  // namespace zjbase
}  // namespace zjlib
#endif
