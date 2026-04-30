#ifndef TOUCHHANDLER_H
#define TOUCHHANDLER_H

#include <Arduino.h>

void initTouch();
bool getTouchPoint(uint16_t &x, uint16_t &y, uint16_t &z);

#endif