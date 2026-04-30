// AnimationPlayer.cpp
// Hardware bring-up for the CYD display.
// SD/MJPEG playback removed — superseded by FaceRenderer.

#include <Arduino_GFX_Library.h>
#include "FaceRenderer.h"

#define BL_PIN 21
#define DISPLAY_SPI_SPEED 80000000L

Arduino_DataBus *bus = new Arduino_HWSPI(2, 15, 14, 13, 12);
Arduino_GFX     *gfx = new Arduino_ILI9341(bus);

void initAnimationPlayer()
{
    pinMode(BL_PIN, OUTPUT);
    digitalWrite(BL_PIN, HIGH);

    if (!gfx->begin(DISPLAY_SPI_SPEED)) {
        Serial.println("Display init failed");
        return;
    }
    gfx->setRotation(1);   // landscape, matches touch
    gfx->fillScreen(0x0000);

    initFaceRenderer(gfx);
    showSplash();
}