#ifndef SYSTEM_SETUP_H
#define SYSTEM_SETUP_H

#include "foc_setup.h"

#include "setup\adc_setup.h"
#include "blocks_lib.h"
#include "setup\clock_setup.h"
#include "setup\dma_setup.h"
#include "setup\gpio_setup.h"
#include "setup\spi_setup.h"
#include "setup\timer_pwm_setup.h"

#define PERIODIC_COUNTER(var_name, value) \
    constexpr uint8_t var_name##_DIV = value; \
    volatile uint8_t var_name##_COUNT = var_name##_DIV;

#define PERIODIC_CHECK(var_name) ((var_name##_COUNT == var_name##_DIV) ? (var_name##_COUNT = 1, true) : (var_name##_COUNT++, false))

PERIODIC_COUNTER(ENCODER, 2);

int16_t current_sensor_A = 0, current_sensor_B = 0, voltage_vref = 0, temperature = 0;

inline void PWM_Callback() {
    TIM2_Set_PWM_Values(MOTOR_CONTROL::pwm.getPwmChannels());
    if (PERIODIC_CHECK(ENCODER))
        SPI1_StartTransfer();
}

inline void ENCODER_Callback() {
    MOTOR_CONTROL::position_raw = POSITION_ENCODER;
    MOTOR_CONTROL::positionHandler.tick();
}

inline void ANALOG_Callback() {
    ADC1_StartDMAConversion();
}

inline void MAIN_Callback() {
    ADC_get_values(MOTOR_CONTROL::adc_channels.ICh1, MOTOR_CONTROL::adc_channels.ICh2, MOTOR_CONTROL::adc_channels.VSup, MOTOR_CONTROL::adc_channels.VRef, MOTOR_CONTROL::adc_channels.Temp);

    MOTOR_CONTROL::adc_normilizer.tick();
    MOTOR_CONTROL::supply.tick();
    MOTOR_CONTROL::currntVectorController.tick();
    MOTOR_CONTROL::motor.tick();
    MOTOR_CONTROL::pwm.tick();

    MOTOR_CONTROL::current_target_polar.ang +=  MOTOR_CONTROL::speed_incr;
}

inline void POSITION_Callback() {
    
}

#endif