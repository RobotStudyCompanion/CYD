#include "DebugOverlay.h"
#include "TouchHandler.h"
#include "Config.h"
#include <TFT_eSPI.h>
#include "version.h"

extern TFT_eSPI tft;

static const int      MAX_DOTS        = 8;
static const uint32_t DOT_TTL_MS      = 2000;
static const int16_t  EYE_BAND_TOP    = 50;
static const int16_t  EYE_BAND_BOTTOM = 190;

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

static void renderDots() {
    if (!config.showTouchDots) return;
    uint32_t now = millis();
    for (auto &d : dots) {
        if (d.bornAt == 0) continue;

        uint32_t age = now - d.bornAt;
        bool persistent = outsideEyeBand(d.y);

        if (age >= DOT_TTL_MS) {
            if (persistent) tft.fillCircle(d.x, d.y, d.baseRadius, TFT_BLACK);
            d.bornAt = 0;
            continue;
        }

        float lifeLeft = 1.0f - (float)age / (float)DOT_TTL_MS;
        int radius = (int)(d.baseRadius * lifeLeft);
        if (radius < 1) radius = 1;

        if (persistent) tft.fillCircle(d.x, d.y, d.baseRadius, TFT_BLACK);
        tft.fillCircle(d.x, d.y, radius, TFT_GREEN);
    }
}

void serviceDebugOverlay() {
    static bool wasDown = false;
    uint16_t x, y, z;
    bool isDown = getTouchPoint(x, y, z);

    if (isDown && !wasDown) {
        Serial.printf("Touch at (%d, %d) z=%d\n", x, y, z);
        recordTouch(x, y, z);
    }
    wasDown = isDown;

    renderDots();
}

void printMemoryReport() {
    Serial.println("=== Memory ===");
    bool hasPsram = psramFound();
    Serial.printf("PSRAM found: %s\n", hasPsram ? "yes" : "no");
    if (hasPsram) {
        Serial.printf("PSRAM size: %u KB\n", ESP.getPsramSize() / 1024);
        Serial.printf("PSRAM free: %u KB\n", ESP.getFreePsram() / 1024);
    }
    Serial.printf("Heap free: %u KB\n", ESP.getFreeHeap() / 1024);
    Serial.printf("Largest heap block: %u KB\n", ESP.getMaxAllocHeap() / 1024);
    Serial.println("==============");
}

void printVersion() {
    Serial.printf("Firmware: %s\n", FW_VERSION);
}