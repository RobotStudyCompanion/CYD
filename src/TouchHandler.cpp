#include <Arduino.h>
#include "TouchHandler.h"
#include "CYD28_TouchscreenR.h"
#include "Config.h"

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

CYD28_TouchR touchscreen(SCREEN_WIDTH, SCREEN_HEIGHT);

void initTouch()
{
    touchscreen.begin();
    touchscreen.setRotation(1);
}

bool getTouchPoint(uint16_t &x, uint16_t &y, uint16_t &z)
{
    if (!touchscreen.touched())
    {
        if (config.touchDebug) Serial.println("not touched");
        return false;
    }

    CYD28_TS_Point p = touchscreen.getPointScaled();

    if (config.touchDebug) {
        Serial.print("RAW X = ");      Serial.print(p.x);
        Serial.print(" | RAW Y = ");   Serial.print(p.y);
        Serial.print(" | RAW Z = ");   Serial.println(p.z);
    }

    x = p.x; y = p.y; z = p.z;
    return true;
}