#ifndef GENERIC_DRIVER_H
#define GENERIC_DRIVER_H

#include <inttypes.h>
#include "../ErrorHandler.h"

// @todo

#define current_limit 0     // Placeholder for specifying the default current limit

class GenericMotorDriver
{
  public:
    virtual void init() = 0;

    uint32_t getSupportedMotors();

    ////////// Chopper Control //////////
    // Output PWM frequency in Hz
    virtual void setPWMFrequency(uint32_t frequency) = 0;

    ////////// Current Vector Control //////////
    // Angle of Current Vector [0...65535] <--> [0...2Pi]
    virtual void setPhase(uint16_t phase) = 0;
    // Module of Current Vector [0...65535]
    virtual void setCurrent(uint16_t scale) = 0;

    ////////// Output State Control //////////
    // General enable/disable flag for the motor driver
    virtual void setEnable(bool enable) = 0;
    // Enable/disable each phases for multi-phase motor control
    virtual void setEnable(bool enA, bool enB, bool enC, bool enD) = 0;
    // Enable/disable phase A
    virtual void setEnableA(bool enA = true) = 0;
    // Enable/disable phase B
    virtual void setEnableB(bool enB = true) = 0;
    // Enable/disable phase C
    virtual void setEnableC(bool enC = true) = 0;
    // Enable/disable phase D
    virtual void setEnableD(bool enD = true) = 0;

    ////////// Direct PWM Control //////////
    // Directly controls the PWM duty for phases A & B & C & D
    virtual void setPWM(uint16_t pwmA, uint16_t pwmB, uint16_t pwmC, uint16_t pwmD) = 0;
    // Directly controls the PWM duty for phase A
    virtual void setPWMA(uint16_t pwmA) = 0;
    // Directly controls the PWM duty for phase B
    virtual void setPWMB(uint16_t pwmB) = 0;
    // Directly controls the PWM duty for phase C
    virtual void setPWMC(uint16_t pwmC) = 0;
    // Directly controls the PWM duty for phase D
    virtual void setPWMD(uint16_t pwmD) = 0;

    ////////// Current Control //////////
    // Monitors the current through each phase (A, B, C, D) for feedback control or diagnostics
    virtual void getCurrents(int16_t &currentA, int16_t &currentB, int16_t &currentC, int16_t &currentD) = 0;

    ////////// System Protection Control //////////
    // Output motor voltage threshold in mV [to prevent overcurrent]
    virtual void setMotorVoltageThreshold(uint32_t voltageThreshold) = 0;
    // Output motor current threshold in mA [to prevent overcurrent]
    virtual void setMotorCurrentThreshold(uint32_t currentThreshold) = 0;
    // Output motor temperature threshold in *C [to prevent overheating]
    virtual void setMotorTempThreshold(uint8_t tempThreshold) = 0;

    ////////// System Error Control //////////
    // Temperature reading from the motor or driver
    virtual uint8_t getTemperature() = 0;
    // Error flags and fault detection
    virtual ERROR::DriverError getErrorFlag() = 0;

  protected:
    uint8_t temperature; // Stores the current temperature reading from the motor or driver

    // Monitors the current through each phase (A, B, C, D) for feedback control or diagnostics
    int16_t currentA, currentB, currentC, currentD;

    ERROR::DriverError errorFlag; // Stores general and specific error flags, essential for fault detection and handling

    uint32_t supportedMotors; // Stores all possible configurations of motor type and motor connection
};

uint32_t GenericMotorDriver::getSupportedMotors() {
  return supportedMotors;
}

#endif