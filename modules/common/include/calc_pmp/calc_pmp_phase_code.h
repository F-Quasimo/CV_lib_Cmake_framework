/*
 * Copyright (c) 2022 ZJ.Bai All rights reserved
 * @author  : ZJ.Bai
 * @Date     : 2022-06-09
 * @Description
 */
#ifndef __COMMON_CALC_PMP_CODE__
#define __COMMON_CALC_PMP_CODE__
namespace zjlib {
namespace zjbase {
enum {
  PMP_SUCCESS = 0,
  PMP_ERROR = -1,
  PHASE_PATTERN_NORM_TBD = -2,
  BASE_TBD = -3
};

enum PHASE_PATTERN_MODE {
  ALIGN_X = 0,
  ALIGN_Y = 1,
  RANDOM = 2,
  CAMOUFLAGE = 3
};

enum PHASE_PATTERN_NORM {
  ORG_OUT = 0,
  NORM_INF = 1,
  NORM_L1 = 2,
  NORM_L2 = 4,
  NORM_L2SQR = 5,
  NORM_HAMMING = 6,
  NORM_HAMMING2 = 7,
  NORM_TYPE_MASK = 7,
  NORM_RELATIVE = 8,
  NORM_MINMAX_0_255 = 32
};

enum PHASE_UNWARP_MODE {
  // BASE for simple 3 frames shift multi-freq pmp
  BASE = 0,
  FREQ_DIV = 1,
};
}  // namespace zjbase
}  // namespace zjlib

#endif
