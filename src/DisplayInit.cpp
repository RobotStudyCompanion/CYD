// DisplayInit.cpp
// Display init via TFT_eSPI. All pins/backlight/speed live in platformio.ini
// under USER_SETUP_LOADED build flags.

#include <TFT_eSPI.h>
#include "FaceRenderer.h"
#include "Config.h"
#include "DisplayInit.h"

static const int BL_PIN = 21;
static const int BL_LEDC_CHANNEL = 0;

TFT_eSPI tft = TFT_eSPI();

void initDisplay()
{
    tft.init();
    tft.setRotation(1);     // landscape, matches touch rotation
    tft.fillScreen(TFT_BLACK);
    
    ledcSetup(0, 5000, 8);              // channel 0, 5 kHz, 8-bit
    ledcAttachPin(BL_PIN, BL_LEDC_CHANNEL);
    setBacklight(config.brightness);

    initFaceRenderer(&tft);
    showSplash();
}

void setBacklight(uint8_t pct) {
    if (pct > 100) pct = 100;
    uint8_t duty = (pct * 255) / 100;
    ledcWrite(BL_LEDC_CHANNEL, duty);
    config.brightness = pct;
}