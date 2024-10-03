#ifndef FOC_SYSTEM_SETUP_H
#define FOC_SYSTEM_SETUP_H

#include "blocks_lib.h"

// Structure to hold system setup configurations
struct SystemSetup {
    CurrentControlMode currentMode;  // Current control mode
    PatternPWM connectionMode;       // PWM connection pattern
    ModePWM pwmMode;                 // PWM operating mode
    MotorType motorMode;             // Type of motor
};


namespace MOTOR_CONTROL {
// Variables for motor and PWM configuration
int32_t resistance = 3500;      // Motor resistance in ohms
int16_t pwm_resolution = 2625;  // PWM resolution
constexpr int32_t MAX_SUPPLY_VOLTAGE = 69000;

// Initialize system setup with specific modes
SystemSetup setup = {
    .currentMode = VOLTAGE_EST,  // Set current control mode to voltage estimation
    .connectionMode = ABCD,      // Set PWM connection mode to ABCD
    .pwmMode = ALLIGNED_GND,     // Set PWM mode to aligned with ground
    .motorMode = STEPPER         // Set motor type to stepper
};

// Initialize ADC channels
AdcChannels adc_channels = {0};  // ADC channels configuration

// Normalize ADC with calibrated reference voltage
NormalizeADC<SENSOR_CONFIG_ADC::CH1_CH2> adc_normilizer(
    VREF_CALC_CALIBRATED(3300, *VREFINT_CAL_ADDR, 3000, 12),  // Calibrated reference voltage
    adc_channels                                              // ADC channels to normalize
);

// Filter constant for supply voltage
uint8_t KfilterVSup = 240;  // Filter constant for supply voltage

// Initialize supply voltage with normalized voltage and filter settings
SupplyVoltage supply(adc_normilizer.get_vsup(),  // Get supply voltage from ADC normalizer
                     KfilterVSup,                // Filter constant
                     MAX_SUPPLY_VOLTAGE          // Maximum supply value for voltage divider by HW design
);

// PID controller settings (temporary)
ControllerPIDFF_Setting pid_settgs = {0, 0, 0, 0};  // PID settings initialized to zero

// Target voltage in millivolts (temporary)
VectorAxes2D_I32 voltage_target_mv = {.sin = 0, .cos = 0};  // Voltage target vector components

// Current target in polar coordinates
VectorPolar2D_I32 current_target_polar = {.ang = 0, .rad = 0};  // Current target angle and magnitude

// Real current target in axes coordinates (temporary)
VectorAxes2D_I32 current_target_real = {.sin = 0, .cos = 0};  // Real current target vector components

// Initialize current vector controller with system setup and targets
ControllerCurrentVector currntVectorController(setup.currentMode,            // Current control mode
                                               voltage_target_mv,            // Voltage target vector
                                               current_target_polar,         // Current target in polar coordinates
                                               current_target_real,          // Real current target vector
                                               resistance,                   // Motor resistance
                                               supply.get_voltage_norm(),    // Normalized supply voltage
                                               supply.get_max_voltage_mv(),  // Maximum supply voltage in millivolts
                                               pid_settgs                    // PID controller settings
);


Motor motor(currntVectorController.get_voltg_I16(), supply.get_voltage_norm(), adc_normilizer.get_current1234());


// Initialize PWM module with setup configurations
ModuleDriverPWM pwm(setup.pwmMode,              // PWM mode from setup
                    pwm_resolution,             // PWM resolution
                    supply.get_voltage_norm(),  // Normalized supply voltage
                    motor.getPwmChannels()    // PWM channels from PWM selector
);

// Absolute position offset initialization
AbsPosition pos_offset(0, 0);  // Position offset set to zero

// Raw position and frequency for encoder
uint16_t position_raw = UINT16_MAX / 2;  // Raw position initialized to half of maximum unsigned 16-bit value
uint16_t frequency = 16000;              // Encoder frequency set to 16,000 Hz

// Initialize encoder position handler with raw position, offset, and frequency
EncoderPositionHandler positionHandler(position_raw,  // Raw encoder position
                                       pos_offset,    // Position offset
                                       frequency      // Encoder frequency
);

int32_t speed_incr = 8000000;

}  // namespace MOTOR_CONTROL

#endif  // FOC_SYSTEM_SETUP_H