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

void setup() {
#ifdef FEATURE_STATUSLED
  pinMode(PINOUT::LED_BLU, OUTPUT);
#endif
}

void loop() {
#ifdef FEATURE_STATUSLED
  digitalWrite(PINOUT::LED_BLU, HIGH);
  delay(500);
  digitalWrite(PINOUT::LED_BLU, LOW);
  delay(500);
#endif
}
