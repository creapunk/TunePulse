/**
 * @file voltage_estimation.h
 * @brief Header file to manage voltage estimation across the motor
 *
 */

#ifndef BLOCK_CONTROLLER_CURRENT_H
#define BLOCK_CONTROLLER_CURRENT_H

#include "controller_pid_feedforward.h"
#include "generic_block.h"

class ControllerCurrent {
  BLOCK_INPUT(CurrentControlMode, mode);
  BLOCK_INPUT(VectorAxes2D_I32,
              curnt_targt_mA);  // Measured current sin reference
  BLOCK_INPUT(VectorAxes2D_I32, curnt_real_mA);
  BLOCK_INPUT(int32_t, motorResistance_mOhm);  // Resistance of motor in ohms
  BLOCK_INPUT(int32_t, voltgSup_mV);           // Supply voltage in volts

  BLOCK_OUTPUT(VectorAxes2D_I32,
               voltg_mV);  // Output calculated voltage sin reference

 protected:
  static constexpr int16_t zero = 0;

  ControllerPIDFF pidA;
  ControllerPIDFF pidB;

 public:
  ControllerCurrent(const CurrentControlMode& mode,
                    const VectorAxes2D_I32& curnt_targt_mA,
                    const VectorAxes2D_I32& curnt_real_mA,
                    const int32_t& motorResistance_mOhm,
                    const int32_t& voltageSupply_mV,
                    const ControllerPIDFF_Setting& pid_settings)
      : mode_(mode),
        curnt_targt_mA_(curnt_targt_mA),
        curnt_real_mA_(curnt_real_mA),
        motorResistance_mOhm_(motorResistance_mOhm),
        voltgSup_mV_(voltageSupply_mV),
        pidA(curnt_real_mA_.sin,
             curnt_targt_mA_.sin,
             zero,
             voltgSup_mV_,
             pid_settings),
        pidB(curnt_real_mA_.cos,
             curnt_targt_mA_.cos,
             zero,
             voltgSup_mV_,
             pid_settings) {}

  /**
   * @brief Updates voltage coltroller calculations
   */
  void tick() {
    if (mode_ & CURRENT_FB) {
      pidA.tick();
      pidB.tick();
      voltg_mV_.sin = pidA.get_output();
      voltg_mV_.cos = pidB.get_output();
      return;
    }
    voltg_mV_.sin = (curnt_targt_mA_.sin * motorResistance_mOhm_) / 1000;
    voltg_mV_.cos = (curnt_targt_mA_.cos * motorResistance_mOhm_) / 1000;
  }
};

#endif  // BLOCK_CONTROLLER_CURRENT_H