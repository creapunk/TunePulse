#include <Arduino.h>

#include "target.h"
#include "tunepulse.h"

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
    tunepulse_init();

    MOTOR_CONTROL::resistance = 3500;                // Set motor phase resistance in mOhms
    MOTOR_CONTROL::current_target_polar.rad = 1000;  // Set motor phase current in mA

    SerialUSB.begin();

    pinMode(PINOUT::LED_GRN, OUTPUT);
}

void loop() {
    MOTOR_CONTROL::lpf.tick();
    MOTOR_CONTROL::filteredPos = MOTOR_CONTROL::lpf.get_output();
    // MOTOR_CONTROL::positionHandler.tick();
    delay(10);
}
