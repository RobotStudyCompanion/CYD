#ifndef DISPLAYINIT_H
#define DISPLAYINIT_H

#include <Arduino.h>

void initDisplay();
void setBacklight(uint8_t duty);   // 0-255, called by 'bright:' command
void reattachBacklight();   // call after GUIslice re-inits TFT_eSPI
#endif