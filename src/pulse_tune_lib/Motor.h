#include "./motor_control/MotorTypeConnectionVariants.h"
#include <inttypes.h>

class GenericMotor
{
  protected:
    MotorType motorType;

    // Electrical properties
    uint16_t resistance;            // mOhm
    uint32_t inductance;            // uH
    uint16_t current;               // mA
    uint16_t kw;                    //
    uint16_t torque;                // mN/m
    uint16_t torqueVsFreq[10] = {}; // mN/m

    // Physical properties
    uint16_t poleCount; // number of magnetic pole pairs

    // Control properties
    bool direction;
};