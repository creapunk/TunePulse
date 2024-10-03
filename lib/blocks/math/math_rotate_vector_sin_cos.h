/**
 * @file math_rotate_vector_sin_cos.h
 * @brief Header file for the Coordinate Rotation
 *
 * This file contains the definition of the RotateCoordinates class, which
 * implements a method to rotate a set of coordinates based on given sine and cosine values.
 *
 * The rotation operation helps in transforming the coordinates to a new frame
 * of reference, which is useful in various applications such as robotics,
 * computer graphics, and aerospace engineering.
 */

#ifndef BLOCK_ROTATE_VECTOR_SIN_COS_H
#define BLOCK_ROTATE_VECTOR_SIN_COS_H

#include "..\common\generic_block.h"

/**
 * @brief Rotate Coordinates
 * This class implements the rotation of coordinates based on input sine and cosine values.
 * @param tick Update the rotation calculations.
 * @param output_sin Resultant sine value after rotation.
 * @param output_cos Resultant cosine value after rotation.
 */
class MathRotateVectorSinCos : public Block {
  BLOCK_INPUT(float, input_sin);     // Input sine value of the initial angle
  BLOCK_INPUT(float, input_cos);     // Input cosine value of the initial angle
  BLOCK_INPUT(float, rotation_sin);  // Sine value of the rotation angle
  BLOCK_INPUT(float, rotation_cos);  // Cosine value of the rotation angle
  BLOCK_OUTPUT(float, output_sin);   // Resultant sine value after rotation
  BLOCK_OUTPUT(float, output_cos);   // Resultant cosine value after rotation

 public:
  /**
   * @brief RotateCoordinates constructor.
   * @param input_sin The input sine value of the initial angle.
   * @param input_cos The input cosine value of the initial angle.
   * @param rotation_sin The sine value of the rotation angle.
   * @param rotation_cos The cosine value of the rotation angle.
   */
  MathRotateVectorSinCos(const float& input_sin,
                    const float& input_cos,
                    const float& rotation_sin,
                    const float& rotation_cos)
      : input_sin_(input_sin),
        input_cos_(input_cos),
        rotation_sin_(rotation_sin),
        rotation_cos_(rotation_cos) {}

  /**
   * @brief Updates the rotation calculations.
   * This method computes the new sine and cosine values after applying the rotation.
   */
  void tick() override;
};

void MathRotateVectorSinCos::tick() {
  // Compute the resultant sine value after rotation
  output_sin_ = input_sin_ * rotation_cos_ + input_cos_ * rotation_sin_;
  
  // Compute the resultant cosine value after rotation
  output_cos_ = input_cos_ * rotation_cos_ - input_sin_ * rotation_sin_;
}

/*
MACRO FOR INTELLISENSE THAT WILL HELP NOT TO MISPLACE VARIABLES
*/

/**
 * @brief Rotate Coordinates
 * This class implements the rotation of coordinates based on input sine and cosine values.
 * Initializes a RotateCoordinates class instance with specified parameters.
 * @param instance_name The unique name for the class instance.
 * @param input_sin The input sine value of the initial angle.
 * @param input_cos The input cosine value of the initial angle.
 * @param rotation_sin The sine value of the rotation angle.
 * @param rotation_cos The cosine value of the rotation angle.
 */
#define INIT_ROTATE_VECTOR_SIN_COS(instance_name, input_sin, input_cos, rotation_sin, rotation_cos) \
  MathRotateVectorSinCos instance_name((input_sin), (input_cos), (rotation_sin), (rotation_cos))

#endif  // BLOCK_ROTATE_VECTOR_SIN_COS_H