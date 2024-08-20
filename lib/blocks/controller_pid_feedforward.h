/**
 * @file block_pid_feedforward.h
 * @brief Header file for the PID Controller with Feed-Forward Correction
 * @todo Update to work with floats and int64_t
 *
 * This file contains the definition of the PIDController_FF class, which
 * implements a PID (Proportional-Integral-Derivative) controller with
 * an additional feed-forward term. The class is designed to be used in
 * embedded systems for precise control of various processes.
 *
 * The PID controller helps in minimizing the error by adjusting the control
 * input, and the feed-forward term aids in predicting system behavior to
 * improve response time and stability.
 */

#ifndef BLOCK_CONTROLLER_PID_FEEDFORWARD_H
#define BLOCK_CONTROLLER_PID_FEEDFORWARD_H

#include "generic_block.h"

struct ControllerPIDFF_Setting {
  int16_t Kp;
  int16_t Ki;
  int16_t Kd;
  int16_t Kff;
};

/**
 * @brief PID Controller with Feed-Forward Correction
 * This class implements a PID controller with an additional feed-forward term.
 * @param tick Update the PID controller calculations.
 * @param error An result of the controller's error calculation.
 * @param output An result of the controller's output calculation.
 */
class ControllerPIDFF : public Block {
  BLOCK_INPUT(int32_t, actual);       // The measured process variable
  BLOCK_INPUT(int32_t, reference);    // The reference value (setpoint)
  BLOCK_INPUT(int32_t, feedforward);  // The feed-forward term
  BLOCK_INPUT(int32_t, limit);           // The output limit

  const ControllerPIDFF_Setting& coef_;  // PIDFF coeffs multiplied by 1000

  
  BLOCK_OUTPUT(int32_t, output);         // The variable where the output stored
  BLOCK_OUTPUT(int32_t, error);          // The variable where the error  stored

 private:
  // Internal state variables
  int32_t integral_ = 0;  // Integral term accumulator
  int32_t previous_error_ =
      0;  // Previous error value for derivative calculation

 public:
  /**
   * @brief PIDFFController constructor.
   * @param actual The measured process variable.
   * @param reference The reference value (setpoint).
   * @param feedforward The feed-forward term.
   * @param Kp Proportional coefficient (multiplied by 1000).
   * @param Ki Integral coefficient (multiplied by 1000).
   * @param Kd Derivative coefficient (multiplied by 1000).
   * @param Kff Feed-forward coefficient (multiplied by 1000).
   * @param limit The output limit.
   */
  ControllerPIDFF(const int32_t& actual,
                  const int32_t& reference,
                  const int32_t& feedforward,
                  const int32_t& limit,
                  const ControllerPIDFF_Setting& coefficients)
      : actual_(actual),
        reference_(reference),
        feedforward_(feedforward),
        coef_(coefficients),
        limit_(limit) {}

  /**
   * @brief Updates the PID controller calculations.
   * This method computes the new control output based on the PID algorithm
   * and the additional feed-forward term.
   */
  void tick() override;
};

void ControllerPIDFF::tick() {
  // Calculate the error
  error_ = reference_ - actual_;

  // Calculate proportional term
  int32_t P = (coef_.Kp * error_) / 1000;

  // Calculate integral term with anti-windup
  integral_ += error_;
  integral_ = constrain(integral_, INT32_MIN / 1000, INT32_MAX / 1000);

  int32_t I = (coef_.Ki * integral_) / 1000;

  // Calculate derivative term
  int32_t D = (coef_.Kd * (error_ - previous_error_)) / 1000;

  // Calculate feed-forward term
  int32_t FF = (coef_.Kff * feedforward_) / 1000;

  // Calculate the output and apply the output limit
  int32_t raw_output = P + I + D + FF;
  output_ = constrain(raw_output, -limit_, limit_);

  // Update previous error
  previous_error_ = error_;
}

#endif  // BLOCK_CONTROLLER_PID_FEEDFORWARD_H