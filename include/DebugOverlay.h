#pragma once
#include <Arduino.h>

// Direct injection — kept public for future programmatic markers (mock taps, etc.)
void recordTouch(uint16_t x, uint16_t y, uint16_t z);

// Reads touch, detects rising edge, logs, records dot, renders all active dots.
// Call once per loop().
void serviceDebugOverlay();
void printMemoryReport();   // call any time; useful at boot and on demand
void printVersion();   // prints FW_VERSION line
void printUptime();
void printLdr();
uint16_t getLdrRaw();           // 0–4095, lower = brighter
uint16_t getLdrLuxish();        // 0–4095, higher = brighter (raw inverted)
uint8_t  getLdrBrightnessPct(); // 0–100, calibrated against LDR_DARK_REF
// bool isRoomTooDark();  // hysteretic: true if ambient too low for comfortable reading