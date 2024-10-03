#ifndef CURRENT_VECTOR_PWM_H
#define CURRENT_VECTOR_PWM_H

#include "blocks_lib.h"
#include "controller_pid_feedforward.h"
#include "math\math_static.h"
#include "math\math_trigonometry.h"

class ControllerCurrentVector {
    BLOCK_INPUT(CurrentControlMode, mode);
    BLOCK_INPUT(VectorAxes2D_I32,
                voltg_targt_mV);  // Measured current sin reference
    BLOCK_INPUT(VectorPolar2D_I32,
                curnt_targt_ang_mA);  // Measured current sin reference
    BLOCK_INPUT(VectorAxes2D_I32,
                curnt_real_mA);             // Measured current sin reference
    BLOCK_INPUT(int32_t, motorResistance);  // Resistance of motor in ohms
    BLOCK_INPUT(int32_t, voltg_sup);        // Supply voltage in volts

    const int32_t voltg_max_;

    BLOCK_OUTPUT(VectorAxes2D_I16,
                 voltg_I16);  // Output calculated voltage sin reference

protected:
    VectorAxes2D_I32 curnt_targt_mA_mA_;
    ControllerPIDFF pidA;
    ControllerPIDFF pidB;
    static constexpr int16_t zero = 0;

public:
    ControllerCurrentVector(const CurrentControlMode& mode,
                            const VectorAxes2D_I32& voltg_targt_mV,
                            const VectorPolar2D_I32& curnt_targt_ang_mA,
                            const VectorAxes2D_I32& curnt_real_mA_mA,
                            const int32_t& motorResistance_mOhm,
                            const int32_t& voltg_sup,
                            const int32_t voltg_max,
                            const ControllerPIDFF_Setting& pid_settings)
        : mode_(mode),
          voltg_targt_mV_(voltg_targt_mV),
          curnt_targt_ang_mA_(curnt_targt_ang_mA),
          curnt_real_mA_(curnt_real_mA_mA),
          motorResistance_(motorResistance_mOhm),
          voltg_sup_(voltg_sup),
          voltg_max_(voltg_max),
          pidA(curnt_real_mA_mA.sin, curnt_targt_mA_mA_.sin, zero, voltg_sup, pid_settings),
          pidB(curnt_real_mA_mA.cos, curnt_targt_mA_mA_.cos, zero, voltg_sup, pid_settings) {}

    void tick() {
        int32_t sin, cos;
        if (mode_ & FOC_VOLTAGE_EST) {
            sin = voltg_targt_mV_.sin;
            cos = voltg_targt_mV_.cos;
        } else {
            VectorAxes2D_I16 sincos = getSinCos(curnt_targt_ang_mA_.ang);
            curnt_targt_mA_mA_ = {.sin = (curnt_targt_ang_mA_.rad * sincos.sin) >> 15,
                                  .cos = (curnt_targt_ang_mA_.rad * sincos.cos) >> 15};

            if (mode_ & CURRENT_FB) {
                pidA.tick();
                pidB.tick();
                sin = pidA.get_output();
                cos = pidB.get_output();
            } else {
                sin = OHMS_LAW::VOLTAGE(curnt_targt_mA_mA_.sin, motorResistance_);
                cos = OHMS_LAW::VOLTAGE(curnt_targt_mA_mA_.cos, motorResistance_);
            }
        }
        voltg_I16_.sin = NORMALIZATION::MV2NORM(sin, voltg_max_);
        voltg_I16_.cos = NORMALIZATION::MV2NORM(cos, voltg_max_);
    }
};

#endif  // CURRENT_VECTOR_PWM_H