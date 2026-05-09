#include <Arduino.h>
#include "TouchHandler.h"
#include "CYD28_TouchscreenR.h"
#include "Config.h"

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

CYD28_TouchR touchscreen(SCREEN_WIDTH, SCREEN_HEIGHT);

static const uint32_t LONG_PRESS_MS   = 1000;
static const uint16_t TAP_MOVE_THRESH = 15;

static enum { TS_IDLE, TS_DOWN, TS_LONG_FIRED } _ts = TS_IDLE;
static uint32_t _pressStartMs = 0;
static uint16_t _startX = 0, _startY = 0, _lastX = 0, _lastY = 0;

static uint16_t _lastX_cached = 0, _lastY_cached = 0, _lastZ_cached = 0;
static bool     _lastTouched_cached = false;

void initTouch()
{
    touchscreen.begin();
    touchscreen.setRotation(1);
}

bool getTouchPoint(uint16_t &x, uint16_t &y, uint16_t &z) {
    if (!touchscreen.touched()) {
        _lastTouched_cached = false;
        if (config.touchDebug) Serial.println("not touched");
        return false;
    }
    CYD28_TS_Point p = touchscreen.getPointScaled();
    if (config.touchDebug) {
        Serial.print("RAW X = "); Serial.print(p.x);
        Serial.print(" | RAW Y = "); Serial.print(p.y);
        Serial.print(" | RAW Z = "); Serial.println(p.z);
    }
    x = p.x; y = p.y; z = p.z;
    _lastX_cached = x; _lastY_cached = y; _lastZ_cached = z;
    _lastTouched_cached = true;
    return true;
}

bool pollTouchEvent(TouchEvent& out) {
    out.kind = TOUCH_NONE;
    uint16_t x, y, z;
    bool touched = getTouchPoint(x, y, z);
    uint32_t now = millis();

    switch (_ts) {
        case TS_IDLE:
            if (touched) {
                _ts = TS_DOWN;
                _pressStartMs = now;
                _startX = _lastX = x;
                _startY = _lastY = y;
            }
            break;

        case TS_DOWN:
            if (touched) {
                _lastX = x; _lastY = y;
                if (now - _pressStartMs >= LONG_PRESS_MS &&
                    abs((int)x - (int)_startX) <= TAP_MOVE_THRESH &&
                    abs((int)y - (int)_startY) <= TAP_MOVE_THRESH) {
                    out = { TOUCH_LONG_PRESS, _startX, _startY };
                    _ts = TS_LONG_FIRED;
                    return true;
                }
            } else {
                if (abs((int)_lastX - (int)_startX) <= TAP_MOVE_THRESH &&
                    abs((int)_lastY - (int)_startY) <= TAP_MOVE_THRESH) {
                    out = { TOUCH_TAP, _startX, _startY };
                    _ts = TS_IDLE;
                    return true;
                }
                _ts = TS_IDLE;
            }
            break;

        case TS_LONG_FIRED:
            if (!touched) _ts = TS_IDLE;
            break;
    }
    return false;
}

bool peekLastTouch(uint16_t& x, uint16_t& y, uint16_t& z) {
    x = _lastX_cached; y = _lastY_cached; z = _lastZ_cached;
    return _lastTouched_cached;
}