/*
 * Copyright (c) 2022 ZJ.Bai All rights reserved
 * @author  : ZJ.Bai
 * @Date     : 2022-06-09
 * @Description
 */
#ifndef __COMMON_MULTI_FREQ__
#define __COMMON_MULTI_FREQ__
#include "calc_pmp/calc_pmp_phase.h"
namespace zjlib {
namespace zjbase {
class MultiFreq {
 public:
  MultiFreq(const std::vector<int>& freqs, int phase_shifts = 4,
            int pattern_mode = PHASE_PATTERN_MODE::ALIGN_X);
  int GetPatterns();
  int Run();
};
}  // namespace zjbase
}  // namespace zjlib
#endif
