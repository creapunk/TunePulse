#ifndef BLOCK_ABSOLUTE_POSITION_H
#define BLOCK_ABSOLUTE_POSITION_H

#include "..\common\generic_block.h"

#include "position_filter.h"
#include "speed_estimator.h"

/**
 * @class EncoderPositionHandler
 * @brief Class to manage and calculate absolute position.
 */
class EncoderPositionHandler {
    BLOCK_INPUT(AbsPosition, pos_offset);  // Position offset input.

    BLOCK_OUTPUT(AbsPosition, position_raw);   // Raw position output.
    BLOCK_OUTPUT(AbsPosition, position_inst);  // Instantaneous position output.

protected:
    uint8_t alpha = 200;
    SpeedEstimator<8> speedEst; // Speed estimator with buffer size 8
    PositionFilter filter;
    uint8_t angle_prev_ = 2;  // Previous angle value for zero-crossing detection.
    uint16_t angle_;
    /**
     * @brief Detects zero crossings in angle values and updates the rotation count corresponding to direction.
     */
    inline void angleZCD();

public:
    /**
     * @brief Constructor to initialize the block with inputs.
     * @param raw_angle Reference to raw angle input.
     * @param pos_offset Reference to position offset input.
     * @param freq Reference to frequency input.
     */
    EncoderPositionHandler(const uint16_t& raw_angle, const AbsPosition& pos_offset, const uint16_t& freq)
        : pos_offset_(pos_offset),
          position_raw_(0, 0),
          position_inst_(0, 0),
          filter(raw_angle, alpha), // Position filtration to reduce noise and smooth transients
          speedEst(position_raw_.position, freq) // Instant speed estimator
           {}

    /**
     * @brief Function to update the block's state.
     */
    void tick() {
        filter.tick();
        angle_ = filter.get_output();

        angleZCD();  // Updating the position with detection of zero crossing

        speedEst.tick();

        // Update current position with offset correction
        position_inst_.position = position_raw_.position + pos_offset_.position;
    }

    const int32_t& get_speed_inst() const { return speedEst.get_speed(); }
};

inline void EncoderPositionHandler::angleZCD() {
    /* This function monitors zero crossings in angular positions, updating rotations
     * using the two most significant bits (sectors) for quick comparison.
     * WARNING! Ensure the algorithm runs at least 5 times per full rotation */

    // Update current angle in the raw position structure
    position_raw_.split.angle = angle_;

    // Extract the 2 most significant bits (sectors) of the current angle
    uint8_t angle_curnt_ = angle_ >> 14;

    // Calculate the difference between previous and current sectors
    int8_t diff = angle_prev_ - angle_curnt_;

    // Adjust rotations based on sector difference (±3 indicates a zero crossing)
    position_raw_.split.rotations += (diff == 3) - (diff == -3);

    // Store the current sector as the previous one for the next call
    angle_prev_ = angle_curnt_;
}

#endif  // BLOCK_ABSOLUTE_POSITION_H
