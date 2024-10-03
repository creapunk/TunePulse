#ifndef SPEED_ESTIMATOR_H
#define SPEED_ESTIMATOR_H

#include "..\common\generic_block.h"  // Include the generic block header for any necessary dependencies

// Note: increasing size SPEED_EST_BUFFER of buffer affects on delay but improves speed resolution
template <uint32_t SPEED_EST_BUFFER>
class SpeedEstimator {
private:
    // Inputs
    const int32_t& position_;  // Reference to the input position value, typically from an encoder
    const uint16_t& freq_;     // Reference to the input frequency value, representing the sampling rate

    // Outputs
    int32_t speed_;  // Variable to store the calculated speed

    // Get power of two of given value to improove speed of calculations
    static constexpr uint8_t BUFFER_SIZE = (SPEED_EST_BUFFER > 2) ? SPEED_EST_BUFFER : 2;

public:
    // Constructor initializing references to position and freq
    SpeedEstimator(const int32_t& position, const uint16_t& freq)
        : position_(position), freq_(freq) {}  // Constructor initializes position and frequency references

    // Output getter
    const int32_t& get_speed() const { return speed_; }  // Method to access the calculated speed

private:
    // Local variables and functions
    int32_t pos_buffer[BUFFER_SIZE] = {0};  // Initialize a circular buffer for storing position samples
    uint32_t idx = 0;                       // Index to track the current position in the circular buffer

public:
    // Method to update the estimator with a new sample
    void tick() {
        speed_ = ((position_ - pos_buffer[idx]) * freq_) / BUFFER_SIZE;  // Calculate the speed

        pos_buffer[idx] = position_;  // Store the current position in the circular buffer at the current index

        idx = (idx + 1) % BUFFER_SIZE;  // Increment the index and wrap around when it reaches the buffer size limit
    }
};

#endif  // SPEED_ESTIMATOR_H