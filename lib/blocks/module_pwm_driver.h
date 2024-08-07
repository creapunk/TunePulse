#ifndef MODULE_PWM_DRIVER_H
#define MODULE_PWM_DRIVER_H

#include "generic_block.h"

class ModuleDriverPWM {
    Input<ModePWM> mode_;
    Input<int16_t> sup_voltg_;
    Input<int16_t> pwm_res_;

    const VoltgChannelslNormlzd& ch1234_;
    VoltgChannelslNormlzd duty = {0};
    bool enb[4] = {0};

   private:
    /* data */
   public:
    ModuleDriverPWM(Input<ModePWM> mode,
                    Input<int16_t> pwm_resolution,
                    Input<int16_t> sup_voltage,
                    Input<VoltgChannelslNormlzd> ch1234)
        : mode_(mode), ch1234_(ch1234), sup_voltg_(sup_voltage), pwm_res_(pwm_resolution) {}

    void tick() {
        for (uint8_t i = 0; i < 4; i++) {
            enb[i] = {bool(ch1234_[i] != INT16_MIN)};
            if (ch1234_[i] <= 0)
                duty[i] = 0;
            else
                duty[i] = (ch1234_[i] * pwm_res_) / sup_voltg_;
        }
    }

    const Input<VoltgChannelslNormlzd> getPwmChannels() const { return duty; }
};

#endif  // MODULE_PWM_DRIVER_H