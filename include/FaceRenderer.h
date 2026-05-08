#pragma once
#include <TFT_eSPI.h>

void initFaceRenderer(TFT_eSPI *tft);
void showSplash();
void serviceFaceRenderer();   // call every loop()

// Runtime tuning hooks — destroys and rebuilds the eyes internally.
// Future serial-config layer will call these from UART command handlers.
void setEyeColour(uint16_t rgb565);
void setBackgroundColour(uint16_t rgb565);
void drawTouchMarker(int x, int y);
void setMood(const char *moodName);