#ifndef SPI_SETUP_H
#define SPI_SETUP_H

#include <Arduino.h>  // Include the Arduino library
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_gpio.h"
#include "stm32g4xx_ll_spi.h"

uint16_t SPI1_tx_buffer[] = {0x8020, 0};
volatile uint16_t SPI1_rx_buffer[] = {0, 0};

volatile uint8_t SPI1_tx_buffer_index = 0;
volatile uint8_t SPI1_rx_buffer_index = 0;

volatile bool SPI1_busy = false;

void SPI1_Init() {
    LL_SPI_InitTypeDef SPI_InitStruct = {0};

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

    /* SPI1 parameter configuration*/
    SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
    SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
    SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_16BIT;
    SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
    SPI_InitStruct.ClockPhase = LL_SPI_PHASE_2EDGE;
    SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
    SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV64;
    SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
    SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    SPI_InitStruct.CRCPoly = 7;
    LL_SPI_Init(SPI1, &SPI_InitStruct);
    LL_SPI_SetStandard(SPI1, LL_SPI_PROTOCOL_MOTOROLA);
    LL_SPI_DisableNSSPulseMgt(SPI1);

    // Enable SPI Interrupts
    // LL_SPI_EnableIT_RXNE(SPI1);
    // LL_SPI_EnableIT_TXE(SPI1);
    // LL_SPI_EnableIT_ERR(SPI1);
}

void SPI1_ChipSelect(bool select) {
    select ? LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_4) : LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_4);
}

void SPI1_Start() {
    SPI1_ChipSelect(false);
    LL_SPI_Enable(SPI1);
}

void SPI1_StartTransfer() {
    // If SPI is still busy with the previous request, just exit
    if (SPI1_busy) {
        return;
    }

    SPI1_rx_buffer_index = 0;
    SPI1_tx_buffer_index = 0;
    SPI1_ChipSelect(true);
    LL_SPI_EnableIT_RXNE(SPI1);
    LL_SPI_EnableIT_TXE(SPI1);
    SPI1_busy = true;
}

extern "C" void SPI1_IRQHandler(void) {
    /* Check RXNE flag value in ISR register */
    if (LL_SPI_IsActiveFlag_RXNE(SPI1)) {
        SPI1_rx_buffer[SPI1_rx_buffer_index++] = LL_SPI_ReceiveData16(SPI1);

        if (SPI1_rx_buffer_index == 2) {
            LL_SPI_DisableIT_RXNE(SPI1);
            // Try to uncomment the following line if you see any signs of data corruption
            // while (LL_SPI_IsActiveFlag_BSY(SPI1));
            SPI1_ChipSelect(false);
            SPI1_busy = false;
        } else {
            LL_SPI_EnableIT_TXE(SPI1);
        }
    }
    /* Check RXNE flag value in ISR register */
    else if (LL_SPI_IsActiveFlag_TXE(SPI1)) {
        LL_SPI_TransmitData16(SPI1, SPI1_tx_buffer[SPI1_tx_buffer_index++]);

        LL_SPI_DisableIT_TXE(SPI1);
    }
    /* Check STOP flag value in ISR register */
    /*
    else if (LL_SPI_IsActiveFlag_OVR(SPI1)) {
        // Call Error function
    }
    */
}

#endif  // SPI_SETUP_H
