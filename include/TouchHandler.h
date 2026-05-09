#ifndef TOUCHHANDLER_H
#define TOUCHHANDLER_H

#include <Arduino.h>

void initTouch();
bool getTouchPoint(uint16_t &x, uint16_t &y, uint16_t &z);

enum TouchEventKind { TOUCH_NONE = 0, TOUCH_TAP, TOUCH_LONG_PRESS };
struct TouchEvent {
    TouchEventKind kind;
    uint16_t x, y;
};

bool pollTouchEvent(TouchEvent& out);
bool peekLastTouch(uint16_t& x, uint16_t& y, uint16_t& z);
#endif