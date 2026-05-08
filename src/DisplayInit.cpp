// DisplayInit.cpp
// Display init via TFT_eSPI. All pins/backlight/speed live in platformio.ini
// under USER_SETUP_LOADED build flags.

#include <TFT_eSPI.h>
#include "FaceRenderer.h"
#include "Config.h"
#include "DisplayInit.h"

static const int BL_PIN = 21;
TFT_eSPI tft = TFT_eSPI();

void initDisplay()
{
    tft.init();
    tft.setRotation(1);     // landscape, matches touch rotation
    tft.fillScreen(TFT_BLACK);
    
    ledcSetup(0, 5000, 8);              // channel 0, 5 kHz, 8-bit
    ledcAttachPin(BL_PIN, 0);
    setBacklight(config.brightness);

    initFaceRenderer(&tft);
    showSplash();
}

void setBacklight(uint8_t duty) {
    config.brightness = duty;
    ledcWrite(0, duty);                 // channel 0 (was: ledcWrite(BL_PIN, duty))
}