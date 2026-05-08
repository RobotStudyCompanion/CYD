#pragma once
#include <Arduino.h>

struct Config {
    bool     touchDebug    = false;
    bool     moodAutoCycle = true;
    bool     showTouchDots = true;
    uint16_t eyeColour     = 0xFFFF;       // RGB565 (TFT_WHITE)
    uint16_t bgColour      = 0x0000;       // RGB565 (TFT_BLACK)
    char     mood[16]      = "NEUTRAL";    // null-terminated, max 15 chars
    uint8_t brightness = 100;       // 0-100 backlight % (converts to PWM 0=off, 255=full)
    bool hudOn = false;
};

extern Config config;

void initConfig();      // load from NVS, call early in setup() before any other init
void serviceConfig();   // poll Serial for commands, call once per loop()