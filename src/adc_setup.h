#ifndef ADC_SETUP_H
#define ADC_SETUP_H

#include <Arduino.h>            // Include the Arduino library
#include "stm32g4xx_ll_adc.h"   // Include STM32G4 ADC peripheral library
#include "stm32g4xx_ll_bus.h"   // Include STM32G4 bus peripheral library
#include "stm32g4xx_ll_gpio.h"  // Include STM32G4 GPIO peripheral library
#include "stm32g4xx_ll_rcc.h"   // Include STM32G4 RCC peripheral library

#include "dma_setup.h"


// Function to initialize ADC1 with DMA
void MX_ADC1_Init() {
    LL_ADC_InitTypeDef ADC_InitStruct = {0};              // ADC initialization structure
    LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};      // ADC regular group initialization structure
    LL_ADC_CommonInitTypeDef ADC_CommonInitStruct = {0};  // ADC common initialization structure

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};     // GPIO initialization structure
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};  // Peripheral clock initialization structure

    // Initializes the peripherals clocks
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
    PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {  // Configure peripheral clocks
        Error_Handler();                                        // Handle errors if peripheral clock configuration fails
    }

    // Enable the peripheral clock for ADC12
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12);

    // Configure common settings for ADC
    ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;      // Set ADC resolution to 12 bits
    ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_LEFT;  // Align data to the left (kinda normalization)
    ADC_InitStruct.LowPowerMode = LL_ADC_LP_MODE_NONE;      // Disable low power mode
    LL_ADC_Init(ADC1, &ADC_InitStruct);                     // Initialize ADC

    // Must be changed for real channels amount used
    ADC_REG_InitStruct.SequencerLength = LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS;  // Enable sequencer with 5 ranks

    ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;           // Set trigger source to software
    ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;  // Disable sequencer discontinuity
    ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;            // Set conversion mode to single
    ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_LIMITED;      // Set DMA transfer to limited
    ADC_REG_InitStruct.Overrun = LL_ADC_REG_OVR_DATA_PRESERVED;            // Preserve data on overrun
    LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);                            // Initialize ADC regular group

    LL_ADC_SetGainCompensation(ADC1, 0);                                       // Set gain compensation
    LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_DISABLE);                     // Disable oversampling
    ADC_CommonInitStruct.CommonClock = LL_ADC_CLOCK_SYNC_PCLK_DIV4;            // Set common clock to sync PCLK divided by 4
    ADC_CommonInitStruct.Multimode = LL_ADC_MULTI_INDEPENDENT;                 // Set multimode to independent
    LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC1), &ADC_CommonInitStruct);  // Initialize ADC common settings

    // Disable ADC deep power down (enabled by default after reset state)
    LL_ADC_DisableDeepPowerDown(ADC1);
    // Enable ADC internal voltage regulator
    LL_ADC_EnableInternalRegulator(ADC1);
    // Delay for ADC internal voltage regulator stabilization
    // Compute number of CPU cycles to wait for, from delay in us
    uint32_t wait_loop_index;
    wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
    while (wait_loop_index != 0) {  // Wait for the regulator to stabilize
        wait_loop_index--;
    }

    // Configure ADC channels
    // Configure Regular Channel 1
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_4);
    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_4, LL_ADC_SAMPLINGTIME_2CYCLES_5);
    LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_4, LL_ADC_SINGLE_ENDED);

    // Configure Regular Channel 2
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_15);
    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_15, LL_ADC_SAMPLINGTIME_2CYCLES_5);
    LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_15, LL_ADC_SINGLE_ENDED);

    // Configure Regular Channel 3
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_3, LL_ADC_CHANNEL_3);
    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_3, LL_ADC_SAMPLINGTIME_2CYCLES_5);
    LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_3, LL_ADC_SINGLE_ENDED);

    // Setup Vref and Temp Channel
    LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1),
                                   LL_ADC_PATH_INTERNAL_VREFINT | LL_ADC_PATH_INTERNAL_TEMPSENSOR);

    // Configure Vref Channel
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_4, LL_ADC_CHANNEL_VREFINT);
    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_VREFINT, LL_ADC_SAMPLINGTIME_2CYCLES_5);
    LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_VREFINT, LL_ADC_SINGLE_ENDED);

    // Configure Temperature Sensor Channel
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_5, LL_ADC_CHANNEL_TEMPSENSOR_ADC1);
    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_TEMPSENSOR_ADC1, LL_ADC_SAMPLINGTIME_2CYCLES_5);
    LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_TEMPSENSOR_ADC1, LL_ADC_SINGLE_ENDED);
}

// Function to start ADC1
void MX_ADC1_Start() {
    // Start ADC calibration
    LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);
    // Wait for calibration to complete
    while (LL_ADC_IsCalibrationOnGoing(ADC1));
    // Enable ADC
    LL_ADC_Enable(ADC1);
    // Wait until ADC is ready
    while (LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0);
    if (LL_ADC_IsActiveFlag_EOC(ADC1))  // Clear end of conversion flag if set
        LL_ADC_ClearFlag_EOC(ADC1);
}

// Function to start DMA conversion for ADC1
void ADC1_StartDMAConversion() {
    LL_ADC_REG_StartConversion(ADC1);  // Start the ADC conversion
}

void ADC_Init() {
    MX_ADC1_Init();
    MX_ADC1_Start();
}

// Depends on sensor type
void ADC_get_values(int16_t& current_a,
                    int16_t& current_b,
                    int16_t& voltage_supply,
                    int16_t& voltage_vref,
                    int16_t& voltage_temp) {
    current_a = dma_buffer[0] >> 1;       // uint16->int16
    current_b = dma_buffer[1] >> 1;       // uint16->int16
    voltage_supply = dma_buffer[2] >> 1;  // uint16->int16
    voltage_vref = dma_buffer[3] >> 1;    // uint16->int16
    voltage_temp = dma_buffer[4] >> 1;    // uint16->int16
}

#endif  // ADC_SETUP_H
