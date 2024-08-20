#include <Arduino.h>

#include "tunepulse.h"

void setup() {
    tunepulse_init();

    MOTOR_CONTROL::resistance = 3500;                // Set motor phase resistance in mOhms
    MOTOR_CONTROL::current_target_polar.rad = 1000;  // Set motor phase current in mA
}

void loop() {
    // Do anything here
}
