#ifndef DMA_SETUP_H
#define DMA_SETUP_H

#include <Arduino.h>
#include "stm32g4xx_ll_adc.h"   // Include STM32G4 ADC peripheral library
#include "stm32g4xx_ll_dma.h"   // Include STM32G4 ADC peripheral library
#include "config.h"

#define DMA_ADC_BUFFER_SIZE 5
volatile uint16_t dma_buffer[DMA_ADC_BUFFER_SIZE];

inline void DMA_Clock_Init(void) {
    /* DMA controller clock enable */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMAMUX1);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
}

inline void DMA_ADC_Init(uint16_t* buffer, uint32_t buffer_size) {
    // Initialize DMA for ADC1
    LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_1, LL_DMAMUX_REQ_ADC1);  // Set peripheral request for DMA1
    LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);  // Set data transfer direction
    LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_HIGH);                // Set channel priority level
    LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MODE_CIRCULAR);                                // Set DMA mode to circular
    LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT);                  // Set peripheral increment mode
    LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);                    // Set memory increment mode
    LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_HALFWORD);                    // Set peripheral data size
    LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_HALFWORD);                    // Set memory data size

    LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_1, LL_DMAMUX_REQ_ADC1);  // Set peripheral request for DMA1 again
    LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_1,
                           LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA),  // Configure DMA addresses
                           (uint32_t)buffer,
                           LL_DMA_DIRECTION_PERIPH_TO_MEMORY  // Set direction from peripheral to memory
    );
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, buffer_size);  // Set data length

    // Enable DMA interrupts
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1);  // Transfer complete interrupt
    LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_1);  // Half transfer interrupt

    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);  // Enable DMA channel
}

inline void DMA_Init() {
    DMA_Clock_Init();
    DMA_ADC_Init((uint16_t*)dma_buffer, DMA_ADC_BUFFER_SIZE);
}

#endif  // DMA_SETUP_H