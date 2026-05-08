#pragma once
#include <Arduino.h>

// Wiring: 3V3 -> fixed R -> GPIO34 -> LDR -> GND.
// analogRead reads LOWER for BRIGHTER (bright ~0, dark ~LDR_DARK_REF).
// getLdrLuxish() and getLdrBrightnessPct() flip that for "bigger = brighter" semantics.

uint16_t getLdrRaw();              // raw averaged ADC 0-4095 (low = bright)
uint16_t getLdrLuxish();           // 4095 - raw; high = bright
uint8_t  getLdrBrightnessPct();    // 0-100, clipped against dark reference
void     printLdr();               // serial diagnostic
void     serviceLdr();   // call once per loop()