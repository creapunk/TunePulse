#ifndef TUNEPULSE_H
#define TUNEPULSE_H

#include "setup\interrupt_setup.h"

inline void tunepulse_init() {
    IO_Init();
    NVIC_Init();
    DMA_Init();
    ADC_Init();
    PWM_Init();
    SPI1_Init();
    SPI1_Start();

    digitalWriteFast(PB_2, HIGH);
    digitalWriteFast(PA_4, HIGH);
}

#endif  // TUNEPULSE_H