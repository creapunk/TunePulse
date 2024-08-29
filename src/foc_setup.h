#ifndef FOC_SYSTEM_SETUP_H
#define FOC_SYSTEM_SETUP_H

#include "blocks_lib.h"

struct SystemSetup {
    CurrentControlMode currentMode;
    PatternPWM connectionMode;
    ModePWM pwmMode;
    MotorType motorMode;
};

namespace MOTOR_CONTROL {
// Variables for motor and PWM configuration

SystemSetup setup = {.currentMode = VOLTAGE_EST, .connectionMode = ABCD, .pwmMode = ALLIGNED_GND, .motorMode = STEPPER};

VectorAxes2D_I32 voltage_target_mv = {.sin = 0, .cos = 0};      // temporary
VectorPolar2D_I32 current_target_polar = {.ang = 0, .rad = 0};  // current vector
VectorAxes2D_I32 current_target_real = {.sin = 0, .cos = 0};    // temporary

int32_t resistance = 3500;
int16_t pwm_resolution = 2625;

VoltageContainer voltg_container = {.voltg_norm = 580, .voltg_mv = 12000, .max_sup_voltage = 69000};

ControllerPIDFF_Setting pid_settgs = {0, 0, 0, 0};  // temp

CurrentVectorPWM currntVectorController(setup.currentMode,
                                        voltage_target_mv,
                                        current_target_polar,
                                        current_target_real,
                                        resistance,
                                        voltg_container,
                                        pid_settgs);

SelectorMotorType motor_sel(setup.motorMode, currntVectorController.get_voltg_I16(), voltg_container.voltg_norm, INT16_MIN);

SelectorInterconnectPwm pwm_mux(setup.connectionMode, motor_sel.getPwmChannels());

ModuleDriverPWM pwm(setup.pwmMode, pwm_resolution, voltg_container.voltg_norm, pwm_mux.getPwmChannels());

AbsPosition pos_offset(0, 0);

uint16_t position_raw = UINT16_MAX / 2;
uint16_t frequency = 16000;
BlockAbsolutePosition positionHandler(position_raw, pos_offset, frequency);

uint8_t alpha = 220;

Filter_LPF_Overflow lpf(positionHandler.get_position_inst().split.angle, alpha);
uint16_t filteredPos = 0;


}  // namespace MOTOR_CONTROL

#endif  // FOC_SYSTEM_SETUP_H