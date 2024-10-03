#ifndef NORMALIZE_ADC_H
#define NORMALIZE_ADC_H

#include "Arduino.h"

#include "common/generic_block.h"  // Include the generic block definitions for I/O blocks

// Structure to hold ADC channel readings
struct AdcChannels {
    uint16_t VRef;  // ADC reading of the reference voltage
    uint16_t VSup;  // ADC reading of the supply voltage
    uint16_t Temp;  // ADC reading of the temperature sensor
    uint16_t ICh1;  // ADC reading of current channel 1
    uint16_t ICh2;  // ADC reading of current channel 2
    uint16_t ICh3;  // ADC reading of current channel 3
    uint16_t ICh4;  // ADC reading of current channel 4
};

using SensorConfig = uint16_t;
namespace SENSOR_CONFIG_ADC {
static constexpr SensorConfig NONE = 0;
static constexpr SensorConfig CH1 = 1 << 0;  // 0b0001
static constexpr SensorConfig CH2 = 1 << 1;  // 0b0010
static constexpr SensorConfig CH3 = 1 << 2;  // 0b0100
static constexpr SensorConfig CH4 = 1 << 3;  // 0b1000

// Комбинации из двух каналов
static constexpr SensorConfig CH1_CH2 = CH1 | CH2;  // 0b0011
static constexpr SensorConfig CH1_CH3 = CH1 | CH3;  // 0b0101
static constexpr SensorConfig CH1_CH4 = CH1 | CH4;  // 0b1001
static constexpr SensorConfig CH2_CH3 = CH2 | CH3;  // 0b0110
static constexpr SensorConfig CH2_CH4 = CH2 | CH4;  // 0b1010
static constexpr SensorConfig CH3_CH4 = CH3 | CH4;  // 0b1100

// Комбинации из трех каналов
static constexpr SensorConfig CH1_CH2_CH3 = CH1 | CH2 | CH3;  // 0b0111
static constexpr SensorConfig CH1_CH2_CH4 = CH1 | CH2 | CH4;  // 0b1011
static constexpr SensorConfig CH1_CH3_CH4 = CH1 | CH3 | CH4;  // 0b1101
static constexpr SensorConfig CH2_CH3_CH4 = CH2 | CH3 | CH4;  // 0b1110

// Все каналы
static constexpr SensorConfig ALL = CH1 | CH2 | CH3 | CH4;  // 0b1111
}  // namespace SENSOR_CONFIG_ADC

// NormalizrerADC class handles voltage normalization and correction
template <SensorConfig Config>
class NormalizeADC {
private:
    BLOCK_INPUT(AdcChannels, adc);  // Input block for reference voltage (vref)

    // BLOCK_OUTPUT(CurntChannelslNormlzd, curnt1234);
    BLOCK_OUTPUT(VoltageADC, vtemp);
    BLOCK_OUTPUT(VoltageADC, vsup);
    BLOCK_OUTPUT(ChCurrentNZ, current1234);

    const uint32_t VREF_CAL;

public:
    // Constructor to initialize the driver with normalized value and maximum voltage
    NormalizeADC(const uint32_t vref_cal, const AdcChannels& adc_readings)
        : VREF_CAL((uint32_t)vref_cal << K_BITSHIFT), adc_(adc_readings) {}
    //, curnt1234_{0}

private:
    static constexpr uint32_t K_BITSHIFT = 15;  // Bitshift constant for the correction factor
    uint32_t k_factor = UINT16_MAX >> 1;        // Initial correction factor 1.0f (starts as half of UINT16_MAX)

    // Function to update the correction factor based on vref
    inline void updateK() {
        k_factor = VREF_CAL / adc_.VRef;  // Adjust correction factor based on the reference voltage
    }

    // constexpr function to clamp ADC value to 16 bits
    constexpr uint16_t CLAMP(uint32_t adc_val) {
        // Extract the higher 16 bits
        uint32_t overflow = adc_val >> 16;
        // Create a mask: if overflow != 0, mask = 0xFFFF, otherwise 0x0000
        uint32_t mask = -(overflow != 0);
        // Apply the mask: if overflow != 0, return 0xFFFF, else return the lower 16 bits of adc_val
        return (uint16_t)(adc_val | (mask & 0xFFFF));
    }


    VoltageADC adjustADC(VoltageADC adc_val) {
        uint32_t corrected_adc = ((uint32_t)adc_val * k_factor) >> K_BITSHIFT;  // Apply k-factor correction

        // return CLAMP(corrected_adc);  // Return the corrected ADC value as a 16-bit unsigned integer

        if (corrected_adc >> 16)
            corrected_adc = 0xFFFF;        // Clamp the value if it exceeds 16 bits
        return (VoltageADC)corrected_adc;  // Return the corrected ADC value as a 16-bit unsigned integer     
    };

public:
    void tick() {
        updateK();
        vsup_ = adjustADC(adc_.VSup);
        vtemp_ = adjustADC(adc_.Temp);

        if constexpr (Config & SENSOR_CONFIG_ADC::CH1)
            current1234_[0] = adjustADC(adc_.ICh1);
        if constexpr (Config & SENSOR_CONFIG_ADC::CH2)
            current1234_[1] = adjustADC(adc_.ICh2);
        if constexpr (Config & SENSOR_CONFIG_ADC::CH3)
            current1234_[2] = adjustADC(adc_.ICh3);
        if constexpr (Config & SENSOR_CONFIG_ADC::CH4)
            current1234_[3] = adjustADC(adc_.ICh4);
    }
};

/**
 * @brief Calculate the calibrated VREF based on design voltage and factory calibration values.
 *
 * This macro computes the calibrated VREF by adjusting the factory-calibrated value
 * to the design VDDA voltage. It shifts the VREF_CAL_VAL to match the resolution
 * and scales it according to the VDDA calibration voltage.
 *
 * @param VDDA_DESIGN_VOLTAGE_MV Target/design VDDA voltage in millivolts.
 * @param VREF_CAL_VAL Factory-calibrated VREF value.
 * @param VDDA_CAL_VOLTAGE_MV VDDA voltage used during factory calibration in millivolts.
 * @param VREF_CAL_RESOLUTION_BITS Resolution of the calibration (e.g., 12 bits for ADC).
 *
 * @return uint32_t The calibrated VREF value adjusted for the design VDDA voltage.
 */
#define VREF_CALC_CALIBRATED(VDDA_DESIGN_VOLTAGE_MV, VREF_CAL_VAL, VDDA_CAL_VOLTAGE_MV, VREF_CAL_RESOLUTION_BITS)     \
    ((uint32_t)((((uint32_t)(VREF_CAL_VAL) << (16 - (VREF_CAL_RESOLUTION_BITS))) * (uint32_t)(VDDA_CAL_VOLTAGE_MV)) / \
                (uint32_t)(VDDA_DESIGN_VOLTAGE_MV)))

/**
 * @brief Approximate the normalized VREF based on the target VDDA voltage.
 *
 * This macro approximates the normalized VREF using the measured VREF voltage and
 * scales it to UINT16_MAX based on the target design VDDA voltage.
 *
 * @param VDDA_DESIGN_VOLTAGE_MV Target/design VDDA voltage in millivolts.
 * @param VREF_VOLTAGE_MV Measured VREF voltage in millivolts.
 *
 * @return uint32_t The normalized VREF value scaled to UINT16_MAX.
 */
#define VREF_CALC_APPROXIMATED(VDDA_DESIGN_VOLTAGE_MV, VREF_VOLTAGE_MV) \
    ((uint32_t)(((uint32_t)(VREF_VOLTAGE_MV) * (uint32_t)UINT16_MAX) / (uint32_t)(VDDA_DESIGN_VOLTAGE_MV)))

#endif  // NORMALIZE_ADC_H