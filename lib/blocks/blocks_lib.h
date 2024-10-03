#ifndef BLOCK_LIB_H
#define BLOCK_LIB_H

#include "Arduino.h"

// Handles encoder position
#include "position_control/position_handler.h"

// Current vector control
#include "controller_current_vector.h"

// PWM control
#include "voltg_pattern_control/selector_interconnect_pwm.h"
#include "voltg_pattern_control/selector_motor_type.h"
#include "module_pwm_driver.h"
#include "motor.h"

// Motion planning
#include "motion_plan_trapezoidal.h"
#include "motion_plan_scurve.h"

// Math
#include "math\math_rotate_vector_sin_cos.h"
#include "math\math_static.h"
#include "math\math_trigonometry.h"

// ADC and supply handling
#include "analog/normalize_adc.h"
#include "analog/supply_voltage.h"

// Helpers
#include "filtration\filter_lpf.h"
#include "controller_pid_feedforward.h"

#endif  // BLOCK_LIB_H