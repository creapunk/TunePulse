#include "..\GenericMotorType.h"

class M_3PHASE : public GenericMotorType
{
  protected:
    GenericMotorDriver *driver;

  public:
    // Same for all variants
    void setDriver(GenericMotorDriver *driver); // attaching driver control interface
    GenericMotorDriver *getDriver();

    // Motor type dependent
    void setActive(bool isActive = true) = 0;     // disable/enable driver
    void setPhase(uint16_t phase) = 0;            // set phase position (electric angle)
    void setCurrent(uint16_t current) = 0;        // ser output current
    void setFrequencySIN(uint16_t frequency) = 0; // set electrical sine frequency
    void setFrequencyPWM(uint32_t frequency);     // set pwm frequency
    bool setMotorType(MotorType motorType);
};