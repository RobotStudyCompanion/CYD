#pragma once
#include <Arduino.h>

enum ThemeMode : uint8_t { THEME_DARK = 0, THEME_LIGHT = 1 };

struct Config {
    bool     touchDebug         = false;
    bool     moodAutoCycle      = true;
    bool     showTouchDots      = false;
    uint16_t eyeColour          = 0xFFFF;       // RGB565 (TFT_WHITE)
    uint16_t bgColour           = 0x0000;       // RGB565 (TFT_BLACK)
    char     mood[16]           = "NEUTRAL";    // null-terminated, max 15 chars
    uint8_t  brightness         = 100;       // 0-100 backlight % (converts to PWM 0=off, 255=full)
    bool     hudOn              = false;
    bool     autoBright         = false;
    uint8_t  brightLight        = 50;   // target % in bright rooms
    uint8_t  brightDark         = 1;    // target % in dark rooms
    bool     idleAnim           = false;
    uint16_t menuTimeoutSec     = 10;   // auto-return to face; 0 = disabled
    ThemeMode theme             = THEME_DARK;
    String (*format)();   // optional: short live-value string for menus, e.g. "75%"
};

extern Config config;

void initConfig();      // load from NVS, call early in setup() before any other init
void serviceConfig();   // poll Serial for commands, call once per loop()

struct Command {
    const char* name;
    void (*set)(const String& val);
    void (*get)();
    const char* help;
    String (*format)();
};

const Command* findCommand(const String& name);
void serviceNvs();
void markDirty();    // for setters