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

#ifdef TARGET_CLN17_V2_0
#warning "Target: CLN17 v2 not yet implemented"
#endif

void setup() {
  pinMode(PINOUT::LED_GRN, OUTPUT);
}

void loop() {
  digitalToggle(PINOUT::LED_GRN);
  delay(100);
}
