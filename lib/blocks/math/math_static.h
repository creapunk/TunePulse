#ifndef MATH_STATIC_H
#define MATH_STATIC_H

#include <Arduino.h>
#include <inttypes.h>
#include "..\blocks_lib.h"

namespace NORMALIZATION {
/**
 * @brief Converts normalized voltage to millivolts.
 * @param voltage_norm The normalized voltage value [int16_t]
 * @param max_voltage The maximum voltage in millivolts [int32_t]
 * @return The voltage in millivolts [int32_t]
 */
constexpr int32_t NORM2MV(int16_t voltage_norm, const int32_t max_voltage) {
    constexpr uint8_t BIT_SHIFT = 9;  // Define bit shift for resolution enhancement
    // Calculate scaling factor to prevent overflow
    int32_t scale = max_voltage >> (15 - BIT_SHIFT);
    // Scale and shift the normalized voltage to millivolts
    return (int32_t)(((int32_t)voltage_norm * (int32_t)scale) >> BIT_SHIFT);
}

/**
 * @brief Converts millivolts to normalized voltage.
 * @param voltage_mv The voltage in millivolts [int32_t]
 * @param max_voltage The maximum voltage in millivolts (only positive range) [int32_t]
 * @return The normalized voltage value [int16_t]
 */
constexpr int16_t MV2NORM(int32_t voltage_mv, const int32_t max_voltage) {
    constexpr uint8_t BIT_SHIFT = 9;  // Define bit shift for resolution enhancement
    // Calculate scaling factor to prevent overflow
    int32_t scale = max_voltage >> (15 - BIT_SHIFT);
    // Shift and scale the millivolt value to normalized voltage
    return (int16_t)(((int32_t)voltage_mv << BIT_SHIFT) / (int32_t)scale);
}
}  // namespace NORMALIZATION

namespace OHMS_LAW {

// Calculate current in milliamps (mA) from voltage (mV) and resistance (mΩ)
constexpr int32_t CURRENT(int32_t voltage_mv, int32_t resistance_mohm) {
    // I = V / R, ensuring we prevent division by zero
    return (resistance_mohm == 0) ? 0 : (voltage_mv * 1000) / resistance_mohm;
}

// Calculate voltage in millivolts (mV) from current (mA) and resistance (mΩ)
constexpr int32_t VOLTAGE(int32_t current_ma, int32_t resistance_mohm) {
    // V = I * R
    return (current_ma * resistance_mohm) / 1000;
}

// Calculate resistance in milliohms (mΩ) from voltage (mV) and current (mA)
constexpr int32_t RESISTANCE(int32_t voltage_mv, int32_t current_ma) {
    // R = V / I, ensuring we prevent division by zero
    return (current_ma == 0) ? 0 : (voltage_mv * 1000) / current_ma;
}

// Calculate power in milliwatts (mW) from voltage (mV) and current (mA)
constexpr int32_t POWER(int32_t voltage_mv, int32_t current_ma) {
    // P = (V * I) / 1000
    return (voltage_mv * current_ma) / 1000;
}

}  // namespace OHMS_LAW

constexpr uint16_t CLAMP_NEGATIVE2ZERO(int16_t x) {
    return x & -(x >= 0);
}

#endif  // MATH_STATIC_H