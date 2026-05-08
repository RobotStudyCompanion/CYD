#include "TouchOverlay.h"
#include <TFT_eSPI.h>
#include "Config.h"

extern TFT_eSPI tft;

static const int      MAX_DOTS        = 8;
static const uint32_t DOT_TTL_MS      = 2000;
static const int16_t  EYE_BAND_TOP    = 50;   // matches sprite vertical anchor
static const int16_t  EYE_BAND_BOTTOM = 190;  // anchor + sprite height (140)

struct TouchDot {
    int16_t  x, y;
    uint8_t  baseRadius;
    uint32_t bornAt;
};

static TouchDot dots[MAX_DOTS] = {};
static int      nextSlot = 0;

static inline bool outsideEyeBand(int16_t y) {
    return y < EYE_BAND_TOP || y > EYE_BAND_BOTTOM;
}

void recordTouch(uint16_t x, uint16_t y, uint16_t z) {
    if (!config.showTouchDots) return;
    uint8_t r = constrain(2 + (z / 500), 2, 10);
    dots[nextSlot] = { (int16_t)x, (int16_t)y, r, millis() };
    nextSlot = (nextSlot + 1) % MAX_DOTS;
}

void serviceTouchOverlay() {
    if (!config.showTouchDots) return;
    uint32_t now = millis();
    for (auto &d : dots) {
        if (d.bornAt == 0) continue;

        uint32_t age = now - d.bornAt;
        bool persistent = outsideEyeBand(d.y);

        if (age >= DOT_TTL_MS) {
            // Final wipe only in persistent zones; eye band auto-clears via sprite
            if (persistent) {
                tft.fillCircle(d.x, d.y, d.baseRadius, TFT_BLACK);
            }
            d.bornAt = 0;
            continue;
        }

        float lifeLeft = 1.0f - (float)age / (float)DOT_TTL_MS;
        int radius = (int)(d.baseRadius * lifeLeft);
        if (radius < 1) radius = 1;

        // Erase the previous (larger) footprint only where pixels persist
        if (persistent) {
            tft.fillCircle(d.x, d.y, d.baseRadius, TFT_BLACK);
        }
        tft.fillCircle(d.x, d.y, radius, TFT_GREEN);
    }
}