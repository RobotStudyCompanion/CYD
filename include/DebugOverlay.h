#pragma once
#include <Arduino.h>

// Direct injection — kept public for future programmatic markers (mock taps, etc.)
void recordTouch(uint16_t x, uint16_t y, uint16_t z);

// Reads touch, detects rising edge, logs, records dot, renders all active dots.
// Call once per loop().
void serviceDebugOverlay();