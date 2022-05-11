
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//#include "cpu.h"
#include "yolox.h"

// YOLOX use the same focus in yolov5
class YoloV5Focus : public ncnn::Layer {
public:
  YoloV5Focus() { one_blob_only = true; }

  virtual int forward(const ncnn::Mat &bottom_blob, ncnn::Mat &top_blob,
                      const ncnn::Option &opt) const {
    int w = bottom_blob.w;
    int h = bottom_blob.h;
    int channels = bottom_blob.c;

    int outw = w / 2;
    int outh = h / 2;
    int outc = channels * 4;

    top_blob.create(outw, outh, outc, 4u, 1, opt.blob_allocator);
    if (top_blob.empty())
      return -100;

#pragma omp parallel for num_threads(opt.num_threads)
    for (int p = 0; p < outc; p++) {
      const float *ptr =
          bottom_blob.channel(p % channels).row((p / channels) % 2) +
          ((p / channels) / 2);
      float *outptr = top_blob.channel(p);

      for (int i = 0; i < outh; i++) {
        for (int j = 0; j < outw; j++) {
          *outptr = *ptr;

          outptr += 1;
          ptr += 2;
        }

        ptr += w;
      }
    }

    return 0;
  }
};
DEFINE_LAYER_CREATOR(YoloV5Focus)

struct GridAndStride {
  GridAndStride(int g0, int g1, int strd) {
    this->grid0 = g0;
    this->grid1 = g1;
    this->stride = strd;
  }
  int grid0;
  int grid1;
  int stride;
};

static inline float intersection_area(const Object &a, const Object &b) {
  cv::Rect_<float> inter = a.rect & b.rect;
  return inter.area();
}

static void qsort_descent_inplace(std::vector<Object> &objects, int left,
                                  int right) {
  int i = left;
  int j = right;
  float p = objects[(left + right) / 2].prob;

  while (i <= j) {
    while (objects[i].prob > p)
      i++;

    while (objects[j].prob < p)
      j--;

    if (i <= j) {
      // swap
      std::swap(objects[i], objects[j]);

      i++;
      j--;
    }
  }

#pragma omp parallel sections
  {
#pragma omp section
    {
      if (left < j)
        qsort_descent_inplace(objects, left, j);
    }
#pragma omp section
    {
      if (i < right)
        qsort_descent_inplace(objects, i, right);
    }
  }
}

static void qsort_descent_inplace(std::vector<Object> &objects) {
  if (objects.empty())
    return;

  qsort_descent_inplace(objects, 0, objects.size() - 1);
}

static void nms_sorted_bboxes(const std::vector<Object> &objects,
                              std::vector<int> &picked, float nms_threshold) {
  picked.clear();

  const int n = objects.size();

  std::vector<float> areas(n);
  for (int i = 0; i < n; i++) {
    areas[i] = objects[i].rect.area();
  }

  for (int i = 0; i < n; i++) {
    const Object &a = objects[i];

    int keep = 1;
    for (int j = 0; j < (int)picked.size(); j++) {
      const Object &b = objects[picked[j]];

      // intersection over union
      float inter_area = intersection_area(a, b);
      float union_area = areas[i] + areas[picked[j]] - inter_area;
      // float IoU = inter_area / union_area
      if (inter_area / union_area > nms_threshold)
        keep = 0;
    }

    if (keep)
      picked.push_back(i);
  }
}

static int generate_grids_and_stride(const int target_size,
                                     std::vector<int> &strides,
                                     std::vector<GridAndStride> &grid_strides) {
  for (auto stride : strides) {
    int num_grid = target_size / stride;
    for (int g1 = 0; g1 < num_grid; g1++) {
      for (int g0 = 0; g0 < num_grid; g0++) {
        grid_strides.push_back(GridAndStride(g0, g1, stride));
      }
    }
  }

  return 0;
}

static void generate_yolox_proposals(std::vector<GridAndStride> grid_strides,
                                     const ncnn::Mat &feat_blob,
                                     float prob_threshold,
                                     std::vector<Object> &objects) {
  const int num_grid = feat_blob.h;
  // fprintf(stderr, "output height: %d, width: %d, channels: %d, dims:%d\n",
  // feat_blob.h, feat_blob.w, feat_blob.c, feat_blob.dims);

  const int num_class = feat_blob.w - 5;

  const int num_anchors = grid_strides.size();

  const float *feat_ptr = feat_blob.channel(0);
  for (int anchor_idx = 0; anchor_idx < num_anchors; anchor_idx++) {
    const int grid0 = grid_strides[anchor_idx].grid0;
    const int grid1 = grid_strides[anchor_idx].grid1;
    const int stride = grid_strides[anchor_idx].stride;

    // yolox/models/yolo_head.py decode logic
    //  outputs[..., :2] = (outputs[..., :2] + grids) * strides
    //  outputs[..., 2:4] = torch.exp(outputs[..., 2:4]) * strides
    float x_center = (feat_ptr[0] + grid0) * stride;
    float y_center = (feat_ptr[1] + grid1) * stride;
    float w = exp(feat_ptr[2]) * stride;
    float h = exp(feat_ptr[3]) * stride;
    float x0 = x_center - w * 0.5f;
    float y0 = y_center - h * 0.5f;

    float box_objectness = feat_ptr[4];
    for (int class_idx = 0; class_idx < num_class; class_idx++) {
      float box_cls_score = feat_ptr[5 + class_idx];
      float box_prob = box_objectness * box_cls_score;
      if (box_prob > prob_threshold) {
        Object obj;
        obj.rect.x = x0;
        obj.rect.y = y0;
        obj.rect.width = w;
        obj.rect.height = h;
        obj.label = class_idx;
        obj.prob = box_prob;

        objects.push_back(obj);
      }

    } // class loop
    feat_ptr += feat_blob.w;

  } // point anchor loop
}

YoloX::YoloX() {}

YoloX::YoloX(float prob_thresh, bool use_vulkan, int target_width,
             int target_height, float nms_thresh) {
  this->target_height = target_height;
  this->target_width = target_width;
  this->target_size = target_size;
  this->prob_thresh = prob_thresh;
  this->nms_thresh = nms_thresh;
  yolox_net.opt.use_vulkan_compute = use_vulkan;
  yolox_net.register_custom_layer("YoloV5Focus", YoloV5Focus_layer_creator);
  yolox_net.load_param(yolox_nano_param_bin);
  yolox_net.load_model(yolox_nano_bin);
  this->LOAD_TYPE = YOLOX_LOAD_MODE::YOLOX_LOAD_FROM_HEAD;
  return;
}

YoloX::YoloX(std::string param_path, std::string bin_path, float prob_thresh,
             bool use_vulkan, int target_width, int target_height,
             float nms_thresh) {
  this->target_height = target_height;
  this->target_width = target_width;
  this->target_size = target_size;
  this->prob_thresh = prob_thresh;
  this->nms_thresh = nms_thresh;
  yolox_net.opt.use_vulkan_compute = use_vulkan;
  yolox_net.register_custom_layer("YoloV5Focus", YoloV5Focus_layer_creator);
  yolox_net.load_param(param_path.c_str());
  yolox_net.load_model(bin_path.c_str());
  this->LOAD_TYPE = YOLOX_LOAD_MODE::YOLOX_LOAD_FROM_FILE;
  return;
}
int YoloX::Load(const char *modeltype, int _target_size,
                const float *_mean_vals, const float *_norm_vals,
                bool use_gpu) {

  return -1;
}
// int YoloX::Load(const char* modeltype, int _target_size, const float*
// _mean_vals, const float* _norm_vals, bool use_gpu){
//     yolox_net.clear();
//     blob_pool_allocator.clear();
//     workspace_pool_allocator.clear();

//     ncnn::set_cpu_powersave(2);
//     ncnn::set_omp_num_threads(ncnn::get_big_cpu_count());

//     yolox_net.opt = ncnn::Option();

// #if NCNN_VULKAN
//     yolox.opt.use_vulkan_compute = use_gpu;
// #endif
//     yolox_net.register_custom_layer("YoloV5Focus",
//     YoloV5Focus_layer_creator); yolox_net.opt.num_threads =
//     ncnn::get_big_cpu_count(); yolox_net.opt.blob_allocator =
//     &blob_pool_allocator; yolox_net.opt.workspace_allocator =
//     &workspace_pool_allocator;

//     char parampath[256];
//     char modelpath[256];
//     sprintf(parampath, "%s.param", modeltype);
//     sprintf(modelpath, "%s.bin", modeltype);

//     yolox_net.load_param(parampath);
//     yolox_net.load_model(modelpath);

//     target_size = _target_size;
//     mean_vals[0] = _mean_vals[0];
//     mean_vals[1] = _mean_vals[1];
//     mean_vals[2] = _mean_vals[2];
//     norm_vals[0] = _norm_vals[0];
//     norm_vals[1] = _norm_vals[1];
//     norm_vals[2] = _norm_vals[2];

//     return 0;
// }
int YoloX::load_from_head_detect(const cv::Mat &bgr,
                                 std::vector<Object> &objects) {
  int img_w = bgr.cols;
  int img_h = bgr.rows;

  // letterbox pad to multiple of 32
  int w = img_w;
  int h = img_h;
  float scale = 1.f;
  if (w > h) {
    scale = (float)target_size / w;
    w = target_size;
    h = h * scale;
  } else {
    scale = (float)target_size / h;
    h = target_size;
    w = w * scale;
  }

  ncnn::Mat in = ncnn::Mat::from_pixels_resize(bgr.data, ncnn::Mat::PIXEL_BGR,
                                               img_w, img_h, w, h);

  // pad to target_size rectangle
  int wpad = target_size - w;
  int hpad = target_size - h;
  ncnn::Mat in_pad;
  // different from yolov5, yolox only pad on bottom and right side,
  // which means users don't need to extra padding info to decode boxes
  // coordinate.
  ncnn::copy_make_border(in, in_pad, 0, hpad, 0, wpad, ncnn::BORDER_CONSTANT,
                         114.f);

  ncnn::Extractor ex = yolox_net.create_extractor();

  // ex.input("images", in_pad);
  ex.input(yolox_nano_param_id::BLOB_images, in_pad);
  std::vector<Object> proposals;

  {
    ncnn::Mat out;
    // ex.extract("output", out);
    ex.extract(yolox_nano_param_id::BLOB_output, out);
    static const int stride_arr[] = {8, 16,
                                     32}; // might have stride=64 in YOLOX
    std::vector<int> strides(
        stride_arr, stride_arr + sizeof(stride_arr) / sizeof(stride_arr[0]));
    std::vector<GridAndStride> grid_strides;
    generate_grids_and_stride(target_size, strides, grid_strides);
    generate_yolox_proposals(grid_strides, out, prob_thresh, proposals);
  }
  // sort all proposals by score from highest to lowest
  qsort_descent_inplace(proposals);

  // apply nms with nms_thresh
  std::vector<int> picked;
  nms_sorted_bboxes(proposals, picked, nms_thresh);

  int count = picked.size();

  objects.resize(count);
  for (int i = 0; i < count; i++) {
    objects[i] = proposals[picked[i]];

    // adjust offset to original unpadded
    float x0 = (objects[i].rect.x) / scale;
    float y0 = (objects[i].rect.y) / scale;
    float x1 = (objects[i].rect.x + objects[i].rect.width) / scale;
    float y1 = (objects[i].rect.y + objects[i].rect.height) / scale;

    // clip
    x0 = std::max(std::min(x0, (float)(img_w - 1)), 0.f);
    y0 = std::max(std::min(y0, (float)(img_h - 1)), 0.f);
    x1 = std::max(std::min(x1, (float)(img_w - 1)), 0.f);
    y1 = std::max(std::min(y1, (float)(img_h - 1)), 0.f);

    objects[i].rect.x = x0;
    objects[i].rect.y = y0;
    objects[i].rect.width = x1 - x0;
    objects[i].rect.height = y1 - y0;
  }
  return 0;
}
int YoloX::load_from_file_detect(const cv::Mat &bgr,
                                 std::vector<Object> &objects) {
  int img_w = bgr.cols;
  int img_h = bgr.rows;

  // letterbox pad to multiple of 32
  int w = img_w;
  int h = img_h;
  float scale = 1.f;
  if (w > h) {
    scale = (float)target_size / w;
    w = target_size;
    h = h * scale;
  } else {
    scale = (float)target_size / h;
    h = target_size;
    w = w * scale;
  }

  ncnn::Mat in = ncnn::Mat::from_pixels_resize(bgr.data, ncnn::Mat::PIXEL_BGR,
                                               img_w, img_h, w, h);

  // pad to target_size rectangle
  int wpad = target_size - w;
  int hpad = target_size - h;
  ncnn::Mat in_pad;
  // different from yolov5, yolox only pad on bottom and right side,
  // which means users don't need to extra padding info to decode boxes
  // coordinate.
  ncnn::copy_make_border(in, in_pad, 0, hpad, 0, wpad, ncnn::BORDER_CONSTANT,
                         114.f);

  ncnn::Extractor ex = yolox_net.create_extractor();

  ex.input("images", in_pad);
  // ex.input(yolox_nano_param_id::BLOB_images, in_pad);
  std::vector<Object> proposals;

  {
    ncnn::Mat out;
    ex.extract("output", out);
    // ex.extract(yolox_nano_param_id::BLOB_output, out);
    static const int stride_arr[] = {8, 16,
                                     32}; // might have stride=64 in YOLOX
    std::vector<int> strides(
        stride_arr, stride_arr + sizeof(stride_arr) / sizeof(stride_arr[0]));
    std::vector<GridAndStride> grid_strides;
    generate_grids_and_stride(target_size, strides, grid_strides);
    generate_yolox_proposals(grid_strides, out, prob_thresh, proposals);
  }
  // sort all proposals by score from highest to lowest
  qsort_descent_inplace(proposals);

  // apply nms with nms_thresh
  std::vector<int> picked;
  nms_sorted_bboxes(proposals, picked, nms_thresh);

  int count = picked.size();

  objects.resize(count);
  for (int i = 0; i < count; i++) {
    objects[i] = proposals[picked[i]];

    // adjust offset to original unpadded
    float x0 = (objects[i].rect.x) / scale;
    float y0 = (objects[i].rect.y) / scale;
    float x1 = (objects[i].rect.x + objects[i].rect.width) / scale;
    float y1 = (objects[i].rect.y + objects[i].rect.height) / scale;

    // clip
    x0 = std::max(std::min(x0, (float)(img_w - 1)), 0.f);
    y0 = std::max(std::min(y0, (float)(img_h - 1)), 0.f);
    x1 = std::max(std::min(x1, (float)(img_w - 1)), 0.f);
    y1 = std::max(std::min(y1, (float)(img_h - 1)), 0.f);

    objects[i].rect.x = x0;
    objects[i].rect.y = y0;
    objects[i].rect.width = x1 - x0;
    objects[i].rect.height = y1 - y0;
  }
  return 0;
}
int YoloX::Detect(const cv::Mat &bgr, std::vector<Object> &objects) {
  if (this->LOAD_TYPE == YOLOX_LOAD_MODE::YOLOX_LOAD_FROM_HEAD) {
    return load_from_head_detect(bgr, objects);
  } else if (this->LOAD_TYPE == YOLOX_LOAD_MODE::YOLOX_LOAD_FROM_FILE) {
    return load_from_file_detect(bgr, objects);
  }
}
int YoloX::Draw(cv::Mat &bgr, const std::vector<Object> &objects) {
  static const char *class_names[] = {
      "person",        "bicycle",      "car",
      "motorcycle",    "airplane",     "bus",
      "train",         "truck",        "boat",
      "traffic light", "fire hydrant", "stop sign",
      "parking meter", "bench",        "bird",
      "cat",           "dog",          "horse",
      "sheep",         "cow",          "elephant",
      "bear",          "zebra",        "giraffe",
      "backpack",      "umbrella",     "handbag",
      "tie",           "suitcase",     "frisbee",
      "skis",          "snowboard",    "sports ball",
      "kite",          "baseball bat", "baseball glove",
      "skateboard",    "surfboard",    "tennis racket",
      "bottle",        "wine glass",   "cup",
      "fork",          "knife",        "spoon",
      "bowl",          "banana",       "apple",
      "sandwich",      "orange",       "broccoli",
      "carrot",        "hot dog",      "pizza",
      "donut",         "cake",         "chair",
      "couch",         "potted plant", "bed",
      "dining table",  "toilet",       "tv",
      "laptop",        "mouse",        "remote",
      "keyboard",      "cell phone",   "microwave",
      "oven",          "toaster",      "sink",
      "refrigerator",  "book",         "clock",
      "vase",          "scissors",     "teddy bear",
      "hair drier",    "toothbrush"};
  for (size_t i = 0; i < objects.size(); i++) {
    const Object &obj = objects[i];

    //        fprintf(stderr, "%d = %.5f at %.2f %.2f %.2f x %.2f\n", obj.label,
    //        obj.prob,
    //                obj.rect.x, obj.rect.y, obj.rect.width, obj.rect.height);

    cv::rectangle(bgr, obj.rect, cv::Scalar(255, 0, 0));

    char text[256];
    sprintf(text, "%s %.1f%%", class_names[obj.label], obj.prob * 100);

    int baseLine = 0;
    cv::Size label_size =
        cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);

    int x = obj.rect.x;
    int y = obj.rect.y - label_size.height - baseLine;
    if (y < 0)
      y = 0;
    if (x + label_size.width > bgr.cols)
      x = bgr.cols - label_size.width;

    cv::rectangle(
        bgr,
        cv::Rect(cv::Point(x, y),
                 cv::Size(label_size.width, label_size.height + baseLine)),
        cv::Scalar(255, 255, 255), -1);

    cv::putText(bgr, text, cv::Point(x, y + label_size.height),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
  }
  return 0;
}
