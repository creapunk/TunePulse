#ifndef GPIO_SETUP_H
#define GPIO_SETUP_H

#include "stm32g4xx_ll_bus.h"   // Include STM32G4 bus peripheral library
#include "stm32g4xx_ll_gpio.h"  // Include STM32G4 GPIO peripheral library
#include "stm32g4xx_ll_rcc.h"   // Include STM32G4 RCC peripheral library

void IO_Init() {
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};  // GPIO initialization structure

    // Enable the peripheral clock for GPIOA and GPIOB
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

    //////////////////////////////// ADC PINS /////////////////////////////////

    // Configure ADC1 GPIOs Mode
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;  // Set pins to analog mode
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;      // Disable pull-up / pull-down

    // Assign selcted mode to GPIOs
    GPIO_InitStruct.Pin = LL_GPIO_PIN_2 | LL_GPIO_PIN_3;  // PA2, PA3,
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);                // Initialize GPIOA

    GPIO_InitStruct.Pin = LL_GPIO_PIN_0;    // PB0
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);  // Initialize GPIOB

    /////////////////////////////////// TIM2 //////////////////////////////////

    // Configure TIM2 GPIOs Mode
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_1;  // TIM2

    // Assign selcted mode to GPIOs
    GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1;  // PA0, PA1
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);                // Initialize GPIOA

    GPIO_InitStruct.Pin = LL_GPIO_PIN_10 | LL_GPIO_PIN_11;  // PB10, PB11
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);                  // Initialize GPIOB

    /////////////////////////////////// SPI1 //////////////////////////////////

    // PA5   ------> SPI1_SCK
    // PA6   ------> SPI1_MISO
    // PA7   ------> SPI1_MOSI
    // PC4   ------> SPI1_CS

    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_5;

    GPIO_InitStruct.Pin = LL_GPIO_PIN_5;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = 0;

    GPIO_InitStruct.Pin = LL_GPIO_PIN_4;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /////////////////////////// GENERAL PURPOSE GPIO //////////////////////////

    // Assign GPIO default state
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_2);

    // Configure GPIOs Mode
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    GPIO_InitStruct.Alternate = 0;

    // Assign selcted mode to GPIOs
    GPIO_InitStruct.Pin = LL_GPIO_PIN_4;  // PA4
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_2;  // PB2
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // LED PINS
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_13);
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_14);
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_15);
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Pin = LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15;  // PB13, PB14, PB15
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

#endif  // GPIO_SETUP_H