#include <Arduino.h>

#include "adc_setup.h"
#include "blocks_lib.h"
#include "dma_setup.h"
#include "encoder.h"
#include "gpio_setup.h"
#include "interrupt_setup.h"
#include "spi_setup.h"
#include "system_clock.h"
#include "timerPWM.h"

#include "blocks_lib.h"
#include "foc_setup.h"

#include <SPI.h>

// COM PORT MUST BE OPENED TO RUN PROGRAM !!!!!!!!!!!!!!!!!

Encoder encoder;

uint32_t angle;

void setup() {
    IO_Init();
    NVIC_Init();
    DMA_Init();
    ADC_Init();
    PWM_Init();
    SPI1_Init();
    SPI1_Start();

    digitalWriteFast(PB_2, HIGH);
    digitalWriteFast(PA_4, HIGH);

    // SerialUSB.begin();    // Initialize SerialUSB
    // while (!SerialUSB) {  // Wait for SerialUSB connection
    //     ;
    // }
    // SerialUSB.println("Ready!");

    MOTOR_CONTROL::resistance = 3500;                // Set motor phase resistance in mOhms
    MOTOR_CONTROL::current_target_polar.rad = 1000;  // Set motor phase current in mA
}

void loop() {
    encoder.tick();
    angle = encoder.get_output();
}
