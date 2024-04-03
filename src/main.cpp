#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

// This includes the target selected by the PIO build environment
// The include error can be dismissed until we find a fix for the dev env.
#include "target.h"

#ifdef TARGET_CLN17_V1_7
#warning "Target: CLN17 v1.7 not yet implemented"
#endif

#ifdef TARGET_CLN17_V2
#warning "Target: CLN17 v2 not yet implemented"
#endif

void setup() {}

void loop() {}
