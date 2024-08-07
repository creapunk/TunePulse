/**
 * @file selector_interconnect_pwm_4ch.h
 * @brief Header file for the SelectorInterconnectPwm class.
 *
 * This class handles the re-mapping of PWM channels for universal motor
 * connection without being bound to phase sequence.
 *
 * @details
 * The SelectorInterconnectPwm class provides functionality to map normalized
 * voltage channels to specific output patterns based on a given mode. This
 * allows for versatile motor control across different configurations and
 * applications.
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
 * #include "selector_interconnect_pwm_4ch.h"
 *
 * // Define your PWM mode and input channels
 * PatternPWM mode = PatternPWM::ABCD;
 * VoltgChannelslNormlzd inputChannels = { // initialize with values // };
 *
 * // Create instance of SelectorInterconnectPwm
 * SelectorInterconnectPwm pwmSelector(mode, inputChannels);
 *
 * // Update the output pattern
 * pwmSelector.tick();
 *
 * // Retrieve the current PWM channels
 * const VoltgChannelslNormlzd& pwmChannels = pwmSelector.getPwmChannels();
 * @endcode
 */

#ifndef SELECTOR_INTERCONNECT_PWM_H
#define SELECTOR_INTERCONNECT_PWM_H

#include "../control_modes.h"
#include "../data_types.h"

class SelectorInterconnectPwm {
   private:
    Input<VoltgChannelslNormlzd> chABCD_;                           ///< Reference to the array containing ABCD channels
    Input<PatternPWM> mode_;                                        ///< Reference to the current mode
    Output<VoltgChannelslNormlzd> normalized_voltage{{INT16_MIN}};  ///< Array to store the current output pattern

   public:
    /**
     * @brief Constructor for SelectorInterconnectPwm.
     * @param mode Reference to the current mode.
     * @param chABCD Reference to the input array containing PWM channels.
     */
    constexpr SelectorInterconnectPwm(const Input<PatternPWM> mode, const Input<VoltgChannelslNormlzd> chABCD)
        : mode_(mode), chABCD_(chABCD) {}

    /**
     * @brief Updates the output pattern based on the current mode.
     */
    void tick() {
        // for (uint8_t i = 0; i < 4; i++) {
        //     normalized_voltage[i] = chABCD_[(mode_ >> (i * 2)) & 0b11];
        // }

        normalized_voltage = {chABCD_[(mode_ >> (0 * 2)) & 0b11], chABCD_[(mode_ >> (1 * 2)) & 0b11],
                              chABCD_[(mode_ >> (2 * 2)) & 0b11], chABCD_[(mode_ >> (3 * 2)) & 0b11]};
    }

    /**
     * @brief Returns the current PWM channels.
     * @return Reference to the array of current PWM channels.
     */
    const Input<VoltgChannelslNormlzd> getPwmChannels() const { return normalized_voltage; }
};

#endif  // SELECTOR_INTERCONNECT_PWM_H
