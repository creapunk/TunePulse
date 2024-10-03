#ifndef MOTOR_H
#define MOTOR_H

#include "common/generic_block.h"
#include "voltg_pattern_control/selector_interconnect_pwm.h"
#include "voltg_pattern_control/selector_motor_type.h"

#include "filtration/filter_lpf.h"

namespace MOTOR_STATUS {
static constexpr int8_t INIT = 0;
static constexpr int8_t RUN = 1;
static constexpr int8_t IDLE = 2;
static constexpr int8_t BRAKE = 3;
static constexpr int8_t ERROR = -1;
}  // namespace MOTOR_STATUS

class Motor {
    uint32_t resistance;
    uint32_t inductance;
    uint32_t max_current;
    int8_t direction;
    uint8_t motor_type;
    uint32_t pole_pairs;

    MotorType motorType = STEPPER;
    PatternPWM connectionType = ABCD;

    int16_t brake = INT16_MIN;

    // VoltgChannelslNZ devAccess;  // Voltage to manually control driver to set it up

    SelectorMotorType motor_sel;

    SelectorInterconnectPwm pwm_mux;

    const ChCurrentNZ& current;

    int8_t status = MOTOR_STATUS::RUN;

    uint16_t DCcurrent;

    Filter_LPF lpf1;
    Filter_LPF lpf2;

    uint8_t alpha = 250;

private:
public:
    Motor(const VectorAxes2D_I16& ab_system, const VoltageNZ& supply, const ChCurrentNZ& current)
        : current(current),
          motor_sel(motorType, ab_system, supply, brake),
          pwm_mux(connectionType, motor_sel.getPwmChannels()),
          lpf1(current[0], alpha),
          lpf2(current[1], alpha) {}

    void tick() {
        switch (status) {
            case MOTOR_STATUS::INIT:
                lpf1.tick();
                lpf2.tick();
                tuneConnection();
                break;
            case MOTOR_STATUS::RUN:
                motor_sel.tick();
                pwm_mux.tick();
                break;
            case MOTOR_STATUS::ERROR:

                break;
            default:
                // Обработка неопределенного состояния
                break;
        }
    }

private:
    uint32_t counter = 32000;
    int16_t devAccess[4];
    void tuneConnection() {
        
        devAccess[0] = 0;
        devAccess[1] = 0;
        devAccess[2] = 0;
        devAccess[3] = 0;
        if (counter > 24000) {
            devAccess[0] = 10000;
        } else if (counter > 16000) {
            devAccess[1] = 10000;
        } else if (counter > 8000) {
            devAccess[2] = 10000;
        } else {
            devAccess[3] = 10000;
        }
        motor_sel.dev(devAccess);
        counter--;
        if (!counter)
            // counter = 32000;
            status = MOTOR_STATUS::RUN;
    }

public:
    const VoltgChannelslNZ& getPwmChannels() const { return pwm_mux.getPwmChannels(); }
};

#endif  // MOTOR_H