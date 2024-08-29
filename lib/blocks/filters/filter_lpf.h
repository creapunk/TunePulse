#ifndef FILTER_LPF_H
#define FILTER_LPF_H

#include "..\generic_block.h"

class Filter_LPF {
private:
    BLOCK_INPUT(uint8_t, alpha);  // 0...255 = 0.0f...1.0f
    BLOCK_INPUT(uint16_t, input);
    BLOCK_OUTPUT(uint16_t, output);
    uint32_t temp_;

    static constexpr uint32_t resolution = 8;  // Additional bits allows for better filtering (min 0 max 8)

public:
    Filter_LPF(const uint16_t& input, const uint8_t& alpha)
        : input_(input), alpha_(alpha), output_(input), temp_(input << resolution) {}

    void tick() {
        uint32_t current = (uint32_t)input_ << resolution;
        // LPF filter with integer math: y = alpha * y + (1 - alpha) * x
        temp_ = ((alpha_ * temp_) + ((256 - alpha_) * current)) >> 8;
        output_ = (uint16_t)(temp_ >> resolution);
    }
};

class Filter_LPF_Overflow {
private:
    BLOCK_INPUT(uint8_t, alpha);  // 0...255 = 0.0f...1.0f
    BLOCK_INPUT(uint16_t, input);
    BLOCK_OUTPUT(uint16_t, output);

    uint32_t temp_;

    static constexpr uint32_t resolution = 7;  // Additional bits allows for better filtering (min 0 max 8)

    static constexpr int32_t threshold = 0x3FFFFF;
    static constexpr int32_t mask = 0x7FFFFF;

public:
    Filter_LPF_Overflow(const uint16_t& input, const uint8_t& alpha)
        : input_(input), alpha_(alpha), output_(input), temp_(input << resolution) {}

    void tick() {
        // Convert the input to a 32-bit integer and shift left by the resolution value to scale it.
        uint32_t current = (uint32_t)input_ << resolution;

        // ########## ZERO CROSS OFFSET CALCULATION #######################
        // Calculate difference to detect zero-cross condition
        uint32_t diff = temp_ - current + threshold;
        // Asset offset depending on input values to bias calculations
        diff = (diff <= mask) ? 0 : ((diff >> 31) ? -threshold : threshold);

        // LPF filter with integer math: y = alpha * y + (1 - alpha) * x
        // Update the filtered value (temp_) using the LPF equation, with scaling by alpha_ and 256 - alpha_.
        temp_ = ((alpha_ * (temp_ - diff)) + ((256 - alpha_) * (current + diff))) >> 8;

        // Adjust the filtered value within the range defined by the mask.
        temp_ = (temp_ + diff) & mask;

        // Scale the filtered value back down and store it as the output.
        output_ = (uint16_t)(temp_ >> resolution);
    }
};

#endif  // FILTER_LPF_H