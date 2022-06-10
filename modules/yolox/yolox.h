#ifndef __YOLOX_H__
#define __YOLOX_H__
#include <opencv2/core/core.hpp>
#include <vector>

#include "net.h"
#include "yolox_nano.id.h"
#include "yolox_nano.mem.h"

// Jetson Nano overclocked to 2014 MHz
// YoloX Nano  | size=416 | model=yoloxN.* | 17.61 FPS | 25.8 mAP
// YoloX Tiny  | size=416 | model=yoloxT.* | 8.24 FPS | 32.8 mAP
// YoloX Small | size=640 | model=yoloxS.* | 2.57 FPS | 40.5 mAP
struct Object {
  cv::Rect_<float> rect;
  int label;
  float prob;
};
enum YOLOX_LOAD_MODE { YOLOX_LOAD_FROM_HEAD = 0, YOLOX_LOAD_FROM_FILE = 2 };

class YoloX {
 public:
  YoloX();
  YoloX(float prob_thresh, bool use_vulkan = false, int target_width = 416,
        int target_height = 416, float nms_thresh = 0.45f);
  // YoloX(std::string param_path,
  //       std::string bin_path,
  //       float prob_thresh=0.25,
  //       bool use_vulkan = false,
  //       int target_width = 416, int target_height = 416,
  //       float nms_thresh = 0.45f);
  YoloX(std::string param_path, std::string bin_path, float prob_thresh = 0.25,
        bool use_vulkan = false, int target_width = 416,
        int target_height = 416, float nms_thresh = 0.45f);
  int Detect(const cv::Mat &bgr, std::vector<Object> &objects);
  int Draw(cv::Mat &bgr, const std::vector<Object> &object);

 private:
  int Load(const char *modeltype, int _target_size, const float *_mean_vals,
           const float *_norm_vals, bool use_gpu);
  int load_from_head_detect(const cv::Mat &bgr, std::vector<Object> &objects);
  int load_from_file_detect(const cv::Mat &bgr, std::vector<Object> &objects);
  ncnn::Net yolox_net;
  int target_size = 416;
  int target_width = 416;
  int target_height = 416;
  float prob_thresh = 0.25f;
  float nms_thresh = 0.45f;
  const float norm_vals[3] = {1 / 255.f, 1 / 255.f, 1 / 255.f};
  float mean_vals[3];
  int LOAD_TYPE = YOLOX_LOAD_MODE::YOLOX_LOAD_FROM_HEAD;
  // ncnn::UnlockedPoolAllocator blob_pool_allocator;
  // ncnn::PoolAllocator workspace_pool_allocator;
};

#endif
