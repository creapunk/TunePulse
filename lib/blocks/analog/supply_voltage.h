/**
 * @file supply_voltage.h
 * @brief Provides the SupplyVoltage class for managing and filtering supply voltage measurements.
 *
 * This file defines the SupplyVoltage class, which handles raw supply voltage readings,
 * applies a low-pass filter to smooth the data, and scales the filtered output to obtain
 * the voltage in millivolts.
 */

#ifndef SUPPLY_VOLTAGE_H
#define SUPPLY_VOLTAGE_H

#include "common/generic_block.h"   // Include generic block definitions
#include "filtration\filter_lpf.h"  // Include low-pass filter definitions
#include "math\math_static.h"

/**
 * @class SupplyVoltage
 * @brief Manages supply voltage measurements with low-pass filtering.
 *
 * This class processes raw supply voltage readings, applies a low-pass filter to smooth the data,
 * and scales the filtered output to obtain the voltage in millivolts.
 *
 * @tparam MAX_VOLTAGE The maximum expected supply voltage in millivolts.
 */
class SupplyVoltage {
private:
    Filter_LPF filter;               // Instance of low-pass filter for smoothing voltage measurements
    const VoltageMV MAX_VOLTAGE_MV;  // Maximum voltage in millivolts for scaling
    VoltageNZ voltage_norm_;         // Current normalized voltage value
    VoltageMV voltage_mv_;           // Current voltage measurement in millivolts
public:
    /**
     * @brief Constructs a SupplyVoltage object with the specified raw voltage and filter constant.
     *
     * @param VSupRaw The raw supply voltage reading from ADC [supposed 16bit for positive range]
     * @param k_filter The filter constant for the low-pass filter.
     * @param max_sup_voltage The maximum supply voltage in millivolts.
     */
    SupplyVoltage(const VoltageADC& VSupRaw, const uint8_t& k_filter, const VoltageMV max_sup_voltage)
        : MAX_VOLTAGE_MV(max_sup_voltage),
          filter(VSupRaw, k_filter) {}  // Initialize maximum voltage and filter with raw voltage and filter constant

    /**
     * @brief Updates the voltage measurement by processing the filter and scaling the output.
     * This function should be called periodically to refresh the voltage measurement.
     */
    void tick() {
        filter.tick();                                                        // Advance the filter state
        voltage_norm_ = (int16_t)((uint16_t)(filter.get_output()) >> 1);      // Retrieve and normalize the filter output
        voltage_mv_ = NORMALIZATION::NORM2MV(voltage_norm_, MAX_VOLTAGE_MV);  // Convert normalized voltage to millivolts
    }

    /**
     * @brief Retrieves the normalized voltage value.
     * @return A constant reference to the normalized voltage.
     */
    const VoltageNZ& get_voltage_norm() const { return voltage_norm_; }  // Return normalized voltage

    /**
     * @brief Retrieves the voltage in millivolts.
     * @return A constant reference to the voltage in millivolts.
     */
    const VoltageMV& get_voltage_mv() const { return voltage_mv_; }  // Return voltage in millivolts

    /**
     * @brief Retrieves the maximum voltage in millivolts.
     * @return The maximum voltage in millivolts.
     */
    const VoltageMV get_max_voltage_mv() { return MAX_VOLTAGE_MV; }  // Return maximum voltage
};

#endif  // SUPPLY_VOLTAGE_H
