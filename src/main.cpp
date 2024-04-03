#include <Arduino.h>
#include <SPI.h>
// #include <Wire.h>

#include <SimpleFOC.h>
#include <SimpleFOCDrivers.h>

#define TARGET_CLN17_V2

#include "target.h"




// #ifdef TARGET_CLN17_V1_5
// #warning "Target: CLN17 v1.5 not yet implemented"
// #endif

#ifdef TARGET_CLN17_V2
#warning "Target: CLN17 v2 not yet implemented"
#endif


// This includes the target selected by the PIO build environment
// The include error can be dismissed until we find a fix for the dev env.

SPIClass SPI_1(PA7, PA6, PA5);
SPISettings ENC_SPI_SETTINGS(8000000, MSBFIRST, SPI_MODE1);

volatile uint16_t respond = 0;
void initMySensorCallback() {
  SPI_1.begin();
  SPI_1.beginTransaction(ENC_SPI_SETTINGS);
  pinMode(PC4, OUTPUT);
  digitalWrite(PC4, HIGH);
}


float readMySensorCallback() {
  digitalWriteFast(PC_4, LOW); // start spi
  SPI_1.transfer16(0x8020); // Send command word
  respond = SPI_1.transfer16(0x0000); // Recieve position
  digitalWriteFast(PC_4, HIGH); // end spi
  respond = respond & 0x7FFF; // remove useless bit
  float angle = respond * (3.14159265358979323846 * 2) / 32768; // convert to radians

  // read my sensor
  // return the angle value in radians in between 0 and 2PI
  return angle;
}
// create the sensor
GenericSensor sensor =
    GenericSensor(readMySensorCallback, initMySensorCallback);

// Stepper motor instance
StepperMotor motor = StepperMotor(50);
// Stepper driver instance
StepperDriver4PWM driver = StepperDriver4PWM(PA0, PA1, PB11, PB10);

// commander interface
Commander command = Commander(SerialUSB);
void onMotor(char* cmd) {
  command.motor(&motor, cmd);
}

void setup() {
  // initialize sensor hardware
  delay(1000);
  pinMode(PB2, OUTPUT); // rst
  digitalWrite(PB2, LOW);
  delay(100);
  digitalWrite(PB2, HIGH);
  pinMode(PA4, OUTPUT); // en
  digitalWrite(PA4, HIGH);
  pinMode(PB1, INPUT_PULLUP); //err
  sensor.init();

  // link to the motor
  motor.linkSensor(&sensor);

  // power supply voltage [V]
  driver.pwm_frequency = 41503;
  driver.voltage_power_supply = 12;
  driver.voltage_limit  = 2;
  driver.init();
  // link the motor to the sensor
  motor.linkDriver(&driver);


  // set control loop type to be used
  motor.controller = MotionControlType::torque;

  // controller configuration based on the control type
  motor.PID_velocity.P = 0.2;
  motor.PID_velocity.I = 20;
  motor.PID_velocity.D = 0;
  // default voltage_power_supply
  motor.voltage_limit = 12;
  // velocity low pass filtering time constant
  motor.LPF_velocity.Tf = 0.01;

  // angle loop controller
  motor.P_angle.P = 20;
  // angle loop velocity limit
  motor.velocity_limit = 50;

  // use monitoring with serial for motor init
  // monitoring port
  SerialUSB.begin();
  // comment out if not needed
  motor.useMonitoring(SerialUSB);

  // initialise motor
  motor.init();

  motor.initFOC();

  // set the initial target value
  motor.target = 3.14159265358979323846;

  // define the motor id
  command.add('M', onMotor, "motor");

  // Run user commands to configure and the motor (find the full command list in
  // docs.simplefoc.com)
  SerialUSB.println(
      F("Motor commands sketch | Initial motion control > torque/voltage : "
        "target 2V."));

  _delay(1000);
}

void loop() {
  
  // iterative setting FOC phase voltage
  motor.loopFOC();

  // iterative function setting the outter loop target
  // velocity, position or voltage
  // if tatget not set in parameter uses motor.target variable
  motor.move();

  // user communication
  command.run();
}