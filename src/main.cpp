#include <Arduino.h>
#include "blocks_lib.h"
#include "bootloaderTools.h"
#include "target.h"

float a = 100.0;
float b = 10.0;
float c = 3;
float d = 0.001;
static MotionPlanTrapezoidal motion0(a, b, c, d);

float e = 100.0;
float f = 10.0;
float g = 3;
float h = 1.5;
float i = 0.001;
static MotionPlanScurve motion1(e, f, g, h, i);

void setup() {
  SerialUSB.begin();
  
  pinMode(PINOUT::LED_GRN, OUTPUT);

}


void loop() {

  motion0.tick();
  SerialUSB.print(">0testVel:");
  SerialUSB.println(motion0.get_current_vel());

  SerialUSB.print(">0testPosition:");
  SerialUSB.println(motion0.get_current_pos());
  
  motion1.tick();
  SerialUSB.print(">1testAccel:");
  SerialUSB.println(motion1.get_current_accel());

  SerialUSB.print(">1testVel:");
  SerialUSB.println(motion1.get_current_vel());

  SerialUSB.print(">1testPosition:");
  SerialUSB.println(motion1.get_current_pos());

  delay(1);

}
