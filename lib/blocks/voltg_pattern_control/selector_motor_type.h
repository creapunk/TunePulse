/**
 * @file selector_motor_type.h
 * @brief Header file for the SelectorMotorType class.
 *
 * This class handles different types of motor controls.
 *
 * @details
 * The SelectorMotorType class provides functionality to manage various motor
 * types, including DC, stepper, and BLDC motors. It allows for versatile
 * control of motors by handling the necessary voltage transformations and
 * mode-specific operations.
 *
 * @author Anton Khrustalev
 * @date 2024-07-25
 * @version 1.0
 * @license Apache-2.0
 *
 * @section copyright Copyright
 * Copyright 2024 by CREAPUNK, http://creapunk.com
 * This project is developed with contributions from the CREAPUNK community.
 * For a full list of contributors, see the CONTRIBUTORS file.
 *
 * @section license License
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section dependencies Dependencies
 * - data_types.h
 * - control_modes.h
 *
 * @section example Example Usage
 * @code
 * #include "selector_motor_type.h"
 *
 * // Define your motor type mode and input voltages
 * MotorType mode = MotorType::BLDC;
 * int16_t supplyVoltageNorm = 1000; // normalized input value
 * VectorAxes2D_I16 normalizedVoltage = { // initialize with values // };
 * int16_t brakeVoltage = 0;
 *
 * // Create instance of SelectorMotorType
 * SelectorMotorType motorSelector(mode, normalizedVoltage, supplyVoltageNorm,
 * brakeVoltage);
 *
 * // Update the motor control based on mode
 * motorSelector.tick();
 *
 * // Retrieve the current PWM channels
 * const VoltgChannelslNormlzd& pwmChannels = motorSelector.getPwmChannels();
 * @endcode
 */

#ifndef SELECTOR_MOTOR_TYPE_H
#define SELECTOR_MOTOR_TYPE_H

#include "../control_modes.h"
#include "../data_types.h"

/**
 * @class SelectorMotorType
 * @brief Class to handle different types of motor controls.
 */
class SelectorMotorType {
  const VectorAxes2D_I16& voltg_;  ///< Input for alpha component of voltage
  const int16_t& voltg_sup_;       ///< Input for supply voltage
  const int16_t& voltg_brak_;      ///< Input for brake voltage
  const MotorType& mode_;          ///< Input for motor type mode

 private:
  VoltgChannelslNormlzd ChABCD;  ///< Array to store voltages for four channels

  /**
   * @enum phase_index
   * @brief Enumeration for channel indices.
   */
  enum phase_index : uint8_t {
    ChannelA = 0,  ///< Channel A
    ChannelB = 1,  ///< Channel B
    ChannelC = 2,  ///< Channel C
    ChannelD = 3   ///< Channel D
  };

  /**
   * @brief Function to calculate coil voltages.
   * @param voltg_ref_ Reference voltage.
   * @param voltg_out_1 Output voltage 1.
   * @param voltg_out_2 Output voltage 2.
   */
  void math_coil(const int16_t& voltg_ref_,
                 int16_t& voltg_out_1,
                 int16_t& voltg_out_2);

  /**
   * @brief Function to handle one-phase motor control.
   */
  void tick1PHASE();

  /**
   * @brief Function to handle two-phase motor control.
   */
  void tick2PHASE();

  /**
   * @brief Function to control 3Phase 3Wire motor with SVPWM algorithm.
   */
  void tick3PHASE();

 public:
  /**
   * @brief Constructor for SelectorMotorType.
   * @param mode Reference to the motor type mode.
   * @param sup Reference to the supply voltage.
   * @param voltg_norm Reference to the normalized voltage.
   * @param brake Reference to the brake voltage.
   */
  SelectorMotorType(const MotorType& mode,
                    const VectorAxes2D_I16& voltg_norm,
                    const int16_t& sup_norm,
                    const int16_t& brake)
      : mode_(mode),
        voltg_sup_(sup_norm),
        voltg_(voltg_norm),
        voltg_brak_(brake) {}

  /**
   * @brief Function to update motor control based on mode.
   */
  void tick() {
    switch (mode_) {
      case DC:
        tick1PHASE();
        break;
      case STEPPER:
        tick2PHASE();
        break;
      case BLDC:
        tick3PHASE();
        break;
      default:
        ChABCD[ChannelA] = INT16_MIN;
        ChABCD[ChannelB] = INT16_MIN;
        ChABCD[ChannelC] = INT16_MIN;
        ChABCD[ChannelD] = INT16_MIN;
        break;
    }
  }

  /**
   * @brief Function to get PWM channel values.
   * @return Reference to array of PWM channel values.
   */
  const VoltgChannelslNormlzd& getPwmChannels() const { return ChABCD; }
};

void SelectorMotorType::math_coil(const int16_t& voltg_ref_,
                                  int16_t& voltg_out_1,
                                  int16_t& voltg_out_2) {
  // Set the default voltage for both outputs
  voltg_out_1 = voltg_ref_;
  voltg_out_2 = voltg_ref_;

  // If the reference voltage is equal to the disable state, do not change
  // anything
  if (voltg_ref_ == INT16_MIN)
    return;
  // If the reference voltage is negative, set to reverse drive mode
  else if (voltg_ref_ < 0) {
    voltg_out_1 = 0;
    voltg_out_2 = abs(voltg_out_2);
  }
  // If the reference voltage is zero or positive, set to direct drive mode
  else
    voltg_out_2 = 0;

  // Total operations: Min 4, Max 6
}

void SelectorMotorType::tick1PHASE() {
  SelectorMotorType::math_coil(voltg_.sin, ChABCD[ChannelA], ChABCD[ChannelB]);
  ChABCD[ChannelC] = voltg_brak_;
  ChABCD[ChannelD] = voltg_brak_;
}

void SelectorMotorType::tick2PHASE() {
  SelectorMotorType::math_coil(voltg_.sin, ChABCD[ChannelA], ChABCD[ChannelB]);
  SelectorMotorType::math_coil(voltg_.cos, ChABCD[ChannelC], ChABCD[ChannelD]);
}

void SelectorMotorType::tick3PHASE() {
  // Precalculated sqrt(3)/2 scaled to uint16_t (sqrt(3)/2 * 2^16)
  static constexpr int32_t SQRT3DIV2 = 56755;

  // Convert beta voltage component to a scaled value using SQRT3DIV2
  int32_t voltg_beta_sqrt3_div2_ = (SQRT3DIV2 * voltg_.cos) >> 16;

  //////// Clarke Transformation ///////////////
  // Set phase A voltage to the alpha component
  ChABCD[ChannelA] = voltg_.sin;

  // Calculate phase B voltage: -1/2 * V_alpha + sqrt(3)/2 * V_beta
  ChABCD[ChannelB] = -voltg_.sin / 2 + voltg_beta_sqrt3_div2_;

  // Calculate phase C voltage: -1/2 * V_alpha - sqrt(3)/2 * V_beta
  ChABCD[ChannelC] = -voltg_.sin / 2 - voltg_beta_sqrt3_div2_;

  // Space Vector Pulse Width Modulation (SVPWM) Algorithm
  int16_t voltg_min_ =
      min(ChABCD[ChannelA], min(ChABCD[ChannelB], ChABCD[ChannelC]));
  int16_t voltg_max_ =
      max(ChABCD[ChannelA], max(ChABCD[ChannelB], ChABCD[ChannelC]));
  int16_t voltg_ref_ = ((voltg_sup_ - voltg_max_) - voltg_min_) >> 1;

  // Shift all phase voltages by voltage_ref_
  ChABCD[ChannelA] += voltg_ref_;
  ChABCD[ChannelB] += voltg_ref_;
  ChABCD[ChannelC] += voltg_ref_;

  // Set unused phase to brake (optional)
  ChABCD[ChannelD] = voltg_brak_;

  // Total Operations: 17
}

#endif  // SELECTOR_MOTOR_TYPE_H
