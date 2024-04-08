#include "GenericMotorDriver.h"
#include "MotorTypeConnectionVariants.h"
#include <inttypes.h>

class GenericMotorType
{
  protected:
    GenericMotorDriver *driver;
  public:
    // Same for all variants
    void setDriver(GenericMotorDriver *driver); // attaching driver control interface
    GenericMotorDriver *getDriver();

    // Motor type dependent
    virtual void setActive(bool isActive = true) = 0;     // disable/enable driver
    virtual void setPhase(uint16_t phase) = 0;            // set phase position (electric angle)
    virtual void setCurrent(uint16_t current) = 0;        // ser output current
    virtual void setFrequencySIN(uint16_t frequency) = 0; // set electrical sine frequency
    virtual void setFrequencyPWM(uint32_t frequency);     // set pwm frequency
    virtual bool setMotorType(MotorType motorType);
};