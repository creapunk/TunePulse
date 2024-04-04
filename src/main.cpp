#include <Arduino.h>
#include <SPI.h>

#include <SimpleFOC.h>
#include <SimpleFOCDrivers.h>
#include "encoders/tle5012b/MagneticSensorTLE5012B.h"

#include "target.h"

// Encoder setup
MagneticSensorTLE5012B sensor(PA7, PA5, PC4);
// Stepper motor instance
StepperMotor motor = StepperMotor(50);
// Stepper driver instance
StepperDriver4PWM driver = StepperDriver4PWM(PINOUT::DRV_A1, PINOUT::DRV_A2, PINOUT::DRV_B1, PINOUT::DRV_B2, PINOUT::DRV_EN);
// commander interface
Commander command = Commander(SerialUSB);

void onMotor(char* cmd) {
  command.motor(&motor, cmd);
}


void setup() {
  SerialUSB.begin();

  // initialize sensor hardware
  delay(100);
  pinMode(PINOUT::DRV_RST, OUTPUT);  // rst
  digitalWrite(PINOUT::DRV_RST, LOW);
  delay(100);
  digitalWrite(PINOUT::DRV_RST, HIGH);
  pinMode(PINOUT::DRV_ERR, INPUT_PULLUP);  // err

  sensor.init();
  // link to the motor
  motor.linkSensor(&sensor);
  // power supply voltage [V]
  driver.pwm_frequency = 41500;
  driver.voltage_power_supply = 24;
  driver.voltage_limit = 2;
  driver.init();
  // link the motor to the sensor
  motor.linkDriver(&driver);

  // set control loop type to be used
  motor.controller = MotionControlType::torque;

  // controller configuration based on the control type
  motor.PID_velocity.P = 0.2;
  motor.PID_velocity.I = 20;
  motor.PID_velocity.D = 0;
  motor.PID_velocity.output_ramp = 1000;
  // velocity low pass filtering time constant

  motor.LPF_velocity.Tf = 0.01;

  // angle loop controller
  motor.P_angle.P = 20;
  // angle loop velocity limit
  motor.velocity_limit = 100;


  // initialise motor
  motor.init();

  motor.initFOC();

  // set the initial target value
  motor.target = 3.14159265358979323846;

  motor.useMonitoring(SerialUSB);

  // define the motor id
  command.add('M', onMotor, "motor");
  _delay(1000);

}

void loop() {
  // iterative foc function 
  motor.loopFOC();

  // iterative function setting and calculating the velocity loop
  // this function can be run at much lower frequency than loopFOC function
  motor.move();

  // user communication
  command.run();

  digitalWriteFast(PINOUT::LED_RED, digitalReadFast(PINOUT::DRV_ERR));
  // delayMicroseconds(50);
}