#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

/*
A class for linking code elements into one whole
- Encoder connection
- Motor control
- Setting motor parameters
*/
#include <inttypes.h>

#include "ErrorHandler.h"
#include "Motor.h"
#include "motor_control/GenericMotorType.h"

class MotorControl
{
  private:
    Motor* motor; // motor instance with all properties
    GenericMotorType* driver; // unified driver interface
    ERROR::MotorError* error; // error 
  public:
    void setTorque();
    void setAngle();

    void getPosition();
};

#endif
