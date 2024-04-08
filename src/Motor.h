#include <inttypes.h>
#include "./motor_control/"
class GenericMotor
{
  protected:
    // MotorType motorType;

    // Electrical properties
    uint16_t resistance; // mOhm
    uint32_t inductance; // uH
    uint16_t current;    // mA
    uint16_t kw;         //

    // Physical properties
    uint16_t poleCount; // number of magnetic pole pairs

    // Control properties
    bool direction;

};