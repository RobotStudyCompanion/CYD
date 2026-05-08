// AnimationPlayer.cpp
// Display init via TFT_eSPI. All pins/backlight/speed live in platformio.ini
// under USER_SETUP_LOADED build flags.
// Name retained for now; rename to DisplayInit.cpp in a later cleanup PR.

#include <TFT_eSPI.h>
#include "FaceRenderer.h"

TFT_eSPI tft = TFT_eSPI();

void initAnimationPlayer()
{
    tft.init();
    tft.setRotation(1);     // landscape, matches touch rotation
    tft.fillScreen(TFT_BLACK);

    initFaceRenderer(&tft);
    showSplash();
}