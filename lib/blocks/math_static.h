#ifndef MATH_STATIC_H
#define MATH_STATIC_H

#include <Arduino.h>
#include <inttypes.h>
#include "blocks_lib.h"



static int16_t voltageNormilize(int32_t voltg_mv, int32_t voltg_sense_max_mv) {
  int64_t voltage_norm_raw =
      (static_cast<int64_t>(voltg_mv) << 15) / voltg_sense_max_mv;
  return int16_t(voltage_norm_raw);
}

#endif  // MATH_STATIC_H