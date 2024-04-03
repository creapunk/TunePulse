#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include <SimpleFOC.h>
#include <SimpleFOCDrivers.h>

// This includes the target selected by the PIO build environment
// The include error can be dismissed until we find a fix for the dev env.
#include "target.h"

#ifdef TARGET_CLN17_V1_5
#warning "Target: CLN17 v1.5 not yet implemented"
#endif

#ifdef TARGET_CLN17_V2
#warning "Target: CLN17 v2 not yet implemented"
#endif

void initMySensorCallback() {
  // do the init
}
float readMySensorCallback() {
  // read my sensor
  // return the angle value in radians in between 0 and 2PI
  return 0;
}
// create the sensor
GenericSensor sensor =
    GenericSensor(readMySensorCallback, initMySensorCallback);

// Stepper motor instance
StepperMotor motor = StepperMotor(50);
// Stepper driver instance
StepperDriver4PWM driver = StepperDriver4PWM(5, 6, 9, 10, 8, 7);

// commander interface
Commander command = Commander(Serial);
void onMotor(char* cmd) {
  command.motor(&motor, cmd);
}

void setup() {
  // initialize sensor hardware
  sensor.init();

  // link to the motor
  motor.linkSensor(&sensor);

  // power supply voltage [V]
  driver.voltage_power_supply = 12;
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
  Serial.begin(115200);
  // comment out if not needed
  motor.useMonitoring(Serial);

  // initialise motor
  motor.init();

  motor.initFOC();

  // set the initial target value
  motor.target = 2;

  // define the motor id
  command.add('M', onMotor, "motor");

  // Run user commands to configure and the motor (find the full command list in
  // docs.simplefoc.com)
  Serial.println(
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