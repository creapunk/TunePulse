#ifndef NVIC_H
#define NVIC_H

#include <Arduino.h>
#include "config.h"

#include "adc_setup.h"
#include "blocks_lib.h"
#include "dma_setup.h"
#include "gpio_setup.h"

#include "spi_setup.h"
#include "clock_setup.h"
#include "timer_pwm_setup.h"

#include "foc_setup.h"

#include "system_setup.h"

extern void PWM_Callback();

extern void ENCODER_Callback();

extern void ANALOG_Callback();

extern void MAIN_Callback();

extern void POSITION_Callback();


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
        (underflow = !underflow) ? PWM_Callback() : ANALOG_Callback();
    }
}

extern "C" void DMA1_Channel1_IRQHandler(void) {
    // Check whether DMA transfer complete caused the DMA interruption
    if (LL_DMA_IsActiveFlag_TC1(DMA1) == 1) {
        LL_DMA_ClearFlag_TC1(DMA1);  // Clear flag DMA transfer complete
        MAIN_Callback();
    }

    // Check whether DMA half transfer caused the DMA interruption
    if (LL_DMA_IsActiveFlag_HT1(DMA1) == 1)
        LL_DMA_ClearFlag_HT1(DMA1);  // Clear flag DMA half transfer
}

#endif  // NVIC_H