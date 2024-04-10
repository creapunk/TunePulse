#ifndef MOTOR_DC_VARIANT_H
#define MOTOR_DC_VARIANT_H

#include "../GenericMotorDriver.h"
#include "../GenericMotorType.h"
#include "../MotorTypeConnectionVariants.h"

class M_DC : public GenericMotorType
{
  public:
    // Motor type dependent
    void setActive(bool isActive = true) override;     // disable/enable driver
    void setPhase(int16_t phase) override;             // sets direction
    void setCurrent(uint16_t current) override;        // ser output current
    void setFrequencySIN(uint16_t frequency) override; // set electrical sine frequency
    void setFrequencyPWM(uint32_t frequency) override; // set pwm frequency

  private:
    void setupControl(uint32_t motorConnection) override;
    void enable();
    void setPWM();

    // ptr enableA 
    // ptr enableB
    // ptr pwmA
    // ptr pwmB
    // ptr currentA
    // ptr currentB
};

void M_DC::setupControl(uint32_t motorConnection)
{
    if (motorConnection && MOTOR_TYPE::M_DC) // Check for DC motor type
    {
        switch (motorConnection)
        {
        case MOTOR_TYPE::M_DC_ABxx:
            // setup here
            break;
        case MOTOR_TYPE::M_DC_xxAB:
            // setup here
            break;
        case MOTOR_TYPE::M_DC_AxxB:
            // setup here
            break;
        case MOTOR_TYPE::M_DC_xABx:
            // setup here
            break;
        case MOTOR_TYPE::M_DC_AxBx:
            // setup here
            break;
        case MOTOR_TYPE::M_DC_xAxB:
            // setup here
            break;
        case MOTOR_TYPE::M_DC_AABB:
            // setup here
            break;
        case MOTOR_TYPE::M_DC_ABAB:
            // setup here
            break;
        default:
        }
    }
}

void M_DC::setPhase(int16_t phase)
{
    // DO NOTHING FOR DC MOTOR CAUSE PHASE 90* BY MOTOR DESIGN
}

#endif