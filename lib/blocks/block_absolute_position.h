#ifndef BLOCK_ABSOLUTE_POSITION_H
#define BLOCK_ABSOLUTE_POSITION_H

#include "generic_block.h"

/**
 * @class BlockAbsolutePosition
 * @brief Class to manage and calculate absolute position.
 */
class BlockAbsolutePosition {
  BLOCK_INPUT(uint16_t, angle_raw);      // Raw angle input.
  BLOCK_INPUT(AbsPosition, pos_offset);  // Position offset input.
  BLOCK_INPUT(uint16_t, freq);            // Frequency input.

  BLOCK_OUTPUT(AbsPosition, position_raw);   // Raw position output.
  BLOCK_OUTPUT(AbsPosition, position_inst);  // Instantaneous position output.
  BLOCK_OUTPUT(int32_t, speed_inst);         // Instantaneous speed output.

 protected:
  uint8_t angle_prev_ = 2;  // Previous angle value for zero-crossing detection.
  int32_t position_prev = 0;
  /**
   * @brief Function to detect zero crossing in angle values.
   */
  void angleZCD();

 public:
  /**
   * @brief Constructor to initialize the block with inputs.
   * @param raw_angle Reference to raw angle input.
   * @param pos_offset Reference to position offset input.
   * @param freq Reference to frequency input.
   */
  BlockAbsolutePosition(const uint16_t& raw_angle,
                        const AbsPosition& pos_offset,
                        const uint16_t& freq)
      : angle_raw_(raw_angle),
        pos_offset_(pos_offset),
        freq_(freq),
        position_raw_(0, 0),
        position_inst_(0, 0) {}

  /**
   * @brief Function to update the block's state.
   */
  void tick() {

    position_prev = position_raw_.position;  // Memorizing the current position

    angleZCD();  // Updating the position with detection of zero crossing

    // Calculate the position difference and multiply by dt
    speed_inst_ = (position_raw_.position - position_prev) * freq_;

    // Update current position with offset correction
    position_inst_.position = position_raw_.position + pos_offset_.position;
  }
};

/**
 * @brief Function to detect zero crossing in angle values and update rotations.
 */
void BlockAbsolutePosition::angleZCD() {
  position_raw_.split.angle = angle_raw_;  // update angle

  // getting 2 highest bits of position value to speed up comparison
  uint8_t angle_curnt_ = angle_raw_ >> 14;

  // Check simplified difference between 2 consecutive values
  int8_t diff = angle_prev_ - angle_curnt_;

  // Handle overflow in angle value to update full rotations
  if (diff == 3)
    position_raw_.split.rotations++;
  else if (diff == -3)
    position_raw_.split.rotations--;

  angle_prev_ = angle_curnt_;  // update previous value
}

#endif  // BLOCK_ABSOLUTE_POSITION_H
