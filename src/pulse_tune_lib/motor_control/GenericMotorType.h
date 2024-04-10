#ifndef GENERIC_MOTOR_TYPE_H
#define GENERIC_MOTOR_TYPE_H

#include "GenericMotorDriver.h"
#include "MotorTypeConnectionVariants.h"
#include <inttypes.h>

class GenericMotorType
{
  public:
    // Same for all variants
    void setDriver(GenericMotorDriver *driver); // attaching driver control interface
    GenericMotorDriver *getDriver();
    ERROR::MotorError *getMotorError();
    bool setMotorType(MOTOR_TYPE motorType);

    // Motor type dependent
    virtual void setActive(bool isActive = true) = 0;     // disable/enable driver
    virtual void setPhase(int16_t phase) = 0;             // set phase position (electric angle)
    virtual void setCurrent(uint16_t current) = 0;        // ser output current
    virtual void setFrequencySIN(uint16_t frequency) = 0; // set electrical sine frequency

    virtual void setFrequencyPWM(uint32_t frequency); // set pwm frequency

  protected:
    GenericMotorDriver *driver;
    uint32_t motorType = 0;
    ERROR::MotorError motorError;

    virtual void setupControl(uint32_t motorConnection) = 0; // sets correct pinout
};

void GenericMotorType::setDriver(GenericMotorDriver *driver)
{
    this->driver = driver;
}

// @todo make safe
GenericMotorDriver *GenericMotorType::getDriver()
{
    return this->driver;
}

ERROR::MotorError *GenericMotorType::getMotorError()
{
    return &this->motorError;
}

bool GenericMotorType::setMotorType(MOTOR_TYPE motor)
{
    this->motorType = 0;
    if (driver->getSupportedMotors() && motor)
        this->motorType = motor;
    setupControl(this->motorType);
    bool sucsess = (this->motorType != 0);
    this->motorError.flags.initializationFailure = !sucsess;
    this->motorError.flags.notSupported = !sucsess;
    return sucsess;
}

#endif