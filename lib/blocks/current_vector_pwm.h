#ifndef CURRENT_VECTOR_PWM_H
#define CURRENT_VECTOR_PWM_H

#include "blocks_lib.h"
#include "controller_current.h"
#include "driver_voltage_sense.h"
#include "math_static.h"
#include "math_trigonometry.h"

class CurrentVectorPWM {
  BLOCK_INPUT(CurrentControlMode, mode);
  BLOCK_INPUT(VectorAxes2D_I32,
              voltg_targt_mV);  // Measured current sin reference
  BLOCK_INPUT(VectorPolar2D_I32,
              curnt_targt_ang_mA);  // Measured current sin reference
  BLOCK_INPUT(VectorAxes2D_I32,
              curnt_real_mA);                // Measured current sin reference
  BLOCK_INPUT(int32_t, motorResistance);     // Resistance of motor in ohms
  BLOCK_INPUT(VoltageContainer, voltg_sys);  // Supply voltage in volts

  BLOCK_OUTPUT(VectorAxes2D_I16,
               voltg_I16);  // Output calculated voltage sin reference

 protected:
  VectorAxes2D_I32 curnt_targt_mA_mA_;
  ControllerCurrent currentController;

 public:
  CurrentVectorPWM(const CurrentControlMode& mode,
                   const VectorAxes2D_I32& voltg_targt_mV,
                   const VectorPolar2D_I32& curnt_targt_ang_mA,
                   const VectorAxes2D_I32& curnt_real_mA_mA,
                   const int32_t& motorResistance_mOhm,
                   const VoltageContainer& voltg_sys,
                   const ControllerPIDFF_Setting& pid_settings)
      : mode_(mode),
        voltg_targt_mV_(voltg_targt_mV),
        curnt_targt_ang_mA_(curnt_targt_ang_mA),
        curnt_real_mA_(curnt_real_mA_mA),
        motorResistance_(motorResistance_mOhm),
        voltg_sys_(voltg_sys),
        currentController(mode,
                          curnt_targt_mA_mA_,
                          curnt_real_mA_mA,
                          motorResistance_mOhm,
                          voltg_sys_.voltg_mv,
                          pid_settings) {}

  void tick() {
    if (mode_ & FOC_VOLTAGE_EST) {
      voltg_I16_.sin =
          voltageNormilize(voltg_targt_mV_.sin, voltg_sys_.max_sup_voltage);
      voltg_I16_.cos =
          voltageNormilize(voltg_targt_mV_.cos, voltg_sys_.max_sup_voltage);
      return;
    }
    VectorAxes2D_I16 sincos = getSinCos(curnt_targt_ang_mA_.ang);
    curnt_targt_mA_mA_ = {.sin = (curnt_targt_ang_mA_.rad * sincos.sin) >> 15,
                          .cos = (curnt_targt_ang_mA_.rad * sincos.cos) >> 15};

    currentController.tick();

    voltg_I16_.sin = voltageNormilize(currentController.get_voltg_mV().sin,
                                      voltg_sys_.max_sup_voltage);
    voltg_I16_.cos = voltageNormilize(currentController.get_voltg_mV().cos,
                                      voltg_sys_.max_sup_voltage);
  }
};

#endif  // CURRENT_VECTOR_PWM_H