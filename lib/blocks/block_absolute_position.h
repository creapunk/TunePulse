#ifndef BLOCK_ABSOLUTE_POSITION_H
#define BLOCK_ABSOLUTE_POSITION_H

#include "filters/filter_lpf.h"
#include "generic_block.h"

/**
 * @class BlockAbsolutePosition
 * @brief Class to manage and calculate absolute position.
 */
class BlockAbsolutePosition {
    BLOCK_INPUT(uint16_t, angle_raw);      // Raw angle input.
    BLOCK_INPUT(AbsPosition, pos_offset);  // Position offset input.
    BLOCK_INPUT(uint16_t, freq);           // Frequency input.

    BLOCK_OUTPUT(AbsPosition, position_raw);   // Raw position output.
    BLOCK_OUTPUT(AbsPosition, position_inst);  // Instantaneous position output.
    BLOCK_OUTPUT(int32_t, speed_inst);         // Instantaneous speed output.

    uint8_t alpha = 200;
    Filter_LPF_Overflow lpf;

protected:
    uint8_t angle_prev_ = 2;  // Previous angle value for zero-crossing detection.
    int32_t position_prev = 0;
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
    BlockAbsolutePosition(const uint16_t& raw_angle, const AbsPosition& pos_offset, const uint16_t& freq)
        : angle_raw_(raw_angle),
          pos_offset_(pos_offset),
          freq_(freq),
          position_raw_(0, 0),
          position_inst_(0, 0),
          lpf(raw_angle, alpha) {}

    /**
     * @brief Function to update the block's state.
     */
    void tick() {
        lpf.tick();
        angle_ = lpf.get_output();

        position_prev = position_raw_.position;  // Memorizing the current position

        angleZCD();  // Updating the position with detection of zero crossing

        // Calculate the position difference and multiply by dt
        speed_inst_ = (position_raw_.position - position_prev) * freq_;

        // Update current position with offset correction
        position_inst_.position = position_raw_.position + pos_offset_.position;
    }
};

inline void BlockAbsolutePosition::angleZCD() {
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
