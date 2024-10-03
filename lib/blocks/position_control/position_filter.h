#ifndef POSITION_FILTER_H
#define POSITION_FILTER_H

#include "..\common\generic_block.h"

class PositionFilter {
private:
    // Inputs
    const uint8_t& alpha_;  // 0 <..> 255 = 0.0f <..> 1.0f
    const uint16_t& input_; // input variable

    // Outputs
    uint16_t output_;

public:
    // Constructor
    PositionFilter(const uint16_t& input, const uint8_t& alpha)
        : input_(input), alpha_(alpha), output_(input), temp_((uint32_t)input << RESOLUTION) {}

    // Output getter
    const uint16_t& get_output() const { return output_; }

private:
    // Local variables and functions
    uint32_t temp_;

    static constexpr uint32_t RESOLUTION = 7;  // Additional bits allows for better filtering (min 0 max 8)

    static constexpr int32_t THRESHOLD = 0x3FFFFF;
    static constexpr int32_t MASK = 0x7FFFFF;

public:
    void tick() {
        // Convert the input to a 32-bit integer and shift left by the resolution value to scale it.
        uint32_t current = (uint32_t)input_ << RESOLUTION;

        // ########## ZERO CROSS OFFSET CALCULATION #######################
        // Calculate difference to detect zero-cross condition
        uint32_t diff = temp_ - current + THRESHOLD;
        // Asset offset depending on input values to bias calculations
        diff = (diff <= MASK) ? 0 : ((diff >> 31) ? -THRESHOLD : THRESHOLD);

        // LPF filter with integer math: y = alpha * y + (1 - alpha) * x
        // Update the filtered value (temp_) using the LPF equation, with scaling by alpha_ and 256 - alpha_.
        temp_ = ((alpha_ * (temp_ - diff)) + ((256 - alpha_) * (current + diff))) >> 8;

        // Adjust the filtered value within the range defined by the MASK.
        temp_ = (temp_ + diff) & MASK;

        // Scale the filtered value back down and store it as the output.
        output_ = (uint16_t)(temp_ >> RESOLUTION);
    }
};

#endif  // ENCODER_POSITION_FILTER_H