#ifndef CONTROL_MODES_H
#define CONTROL_MODES_H

#include <inttypes.h>

enum PatternPWM : uint8_t {
  ABCD = 0b11100100,  // Pattern 0: {0, 1, 2, 3}
  ACDB = 0b01111000,  // Pattern 1: {0, 2, 3, 1}
  ADBC = 0b10011100,  // Pattern 2: {0, 3, 1, 2}
  DCAB = 0b01001011   // Pattern 3: {3, 2, 0, 1}
};

enum CurrentControlMode : uint8_t {
  VOLTAGE_EST = 0b00,
  CURRENT_FB = 0b01,
  FOC_VOLTAGE_EST = 0b10,
  FOC_CURRENT_FB = 0b11
};

/**
 * @enum MotorType
 * @brief Enumeration to define types of motors.
 */
enum MotorType : uint8_t {
  NOPE = 0,  // No motor
  DC,        // DC motor
  STEPPER,   // Stepper motor
  BLDC       // BLDC motor
};


enum ModePWM : uint8_t { ALLIGNED_GND = 0, ALLIGNED_VCC = 1 };

#endif  // CONTROL_MODES_H