#ifndef NVIC_H
#define NVIC_H

#include <Arduino.h>
#include "config.h"

#include "foc_setup.h"

#include "adc_setup.h"
#include "blocks_lib.h"
#include "dma_setup.h"
#include "gpio_setup.h"
#include "interrupt_setup.h"
#include "system_clock.h"
#include "timerPWM.h"

void NVIC_Init() {
    /* DMA interrupt init */
    /* DMA1_Channel1_IRQn interrupt configuration */
    NVIC_SetPriority(DMA1_Channel1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), DMA_CH1_INT_PRIORITY, 0));
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    /* SPI1 interrupt init */
    NVIC_SetPriority(SPI1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), SPI1_INT_PRIORITY, 0));
    NVIC_EnableIRQ(SPI1_IRQn);

    /* PWM TIM2 interrupt Init */
    HAL_NVIC_SetPriority(TIM2_IRQn, TIM2_INT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
    if (htim->Instance == TIM2) {
        static bool underflow = true;
        if (underflow = !underflow) {
            TIM2_Set_PWM_Values(MOTOR_CONTROL::pwm.getPwmChannels());
        } else {
            ADC1_StartDMAConversion();
        }
    }
}

int16_t sup_vltg = 10000;  // not used yet
int16_t current_sensor_A, current_sensor_B, voltage_vref, temperature;
extern "C" void DMA1_Channel1_IRQHandler(void) {
    // Check whether DMA transfer complete caused the DMA interruption
    if (LL_DMA_IsActiveFlag_TC1(DMA1) == 1) {
        LL_DMA_ClearFlag_TC1(DMA1);  // Clear flag DMA transfer complete

        // // Complete DMA transfer handler
        // HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_10);
        ADC_get_values(current_sensor_A, current_sensor_B, MOTOR_CONTROL::voltg_container.voltg_norm, voltage_vref, temperature);
        MOTOR_CONTROL::current_target_polar.ang += 1 << 23;
        // MOTOR_CONTROL::angleRaw += 1 << 24;

        MOTOR_CONTROL::voltg_container.voltg_mv = (MOTOR_CONTROL::voltg_container.voltg_norm * 69000) >> 15;  // Danger
        MOTOR_CONTROL::tick();
    }

    // Check whether DMA half transfer caused the DMA interruption
    if (LL_DMA_IsActiveFlag_HT1(DMA1) == 1) {
        LL_DMA_ClearFlag_HT1(DMA1);  // Clear flag DMA half transfer

        // // Half DMA transfer handler
        // HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_10);
    }
}

#endif  // NVIC_H