#ifndef FILTER_LPF_H
#define FILTER_LPF_H

#include "common/generic_block.h"

class Filter_LPF {
private:
    BLOCK_INPUT(uint8_t, alpha);  // 0...255 = 0.0f...1.0f
    BLOCK_INPUT(uint16_t, input);
    BLOCK_OUTPUT(uint16_t, output);

private:
    uint32_t temp_;

    static constexpr uint8_t RESOLUTION = 8;  // Additional bits allows for better filtering (min 0 max 8)

public:
    Filter_LPF(const uint16_t& input, const uint8_t& alpha)
        : input_(input), alpha_(alpha), output_(input), temp_((uint32_t)(input) << RESOLUTION) {}

    void tick() {
        uint32_t current = (uint32_t)input_ << RESOLUTION;
        // LPF filter with integer math: y = alpha * y + (1 - alpha) * x
        temp_ = ((alpha_ * temp_) + ((256 - alpha_) * current)) >> 8;
        output_ = (uint16_t)(temp_ >> RESOLUTION);
    }
};

#endif  // FILTER_LPF_H