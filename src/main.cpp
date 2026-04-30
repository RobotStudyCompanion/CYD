#include <Arduino.h>
#include "FaceRenderer.h"
#include "LED_Solution.h"
#include "TouchHandler.h"

void initAnimationPlayer();  // forward decl

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(50);
    // Memory diagnostics
    Serial.println("=== Memory ===");
    Serial.print("PSRAM found: ");
    Serial.println(psramFound() ? "yes" : "no");
    if (psramFound()) {
        Serial.print("PSRAM size: ");
        Serial.print(ESP.getPsramSize() / 1024);
        Serial.println(" KB");
        Serial.print("PSRAM free: ");
        Serial.print(ESP.getFreePsram() / 1024);
        Serial.println(" KB");
    }
    Serial.print("Heap free: ");
    Serial.print(ESP.getFreeHeap() / 1024);
    Serial.println(" KB");
    Serial.print("Largest heap block: ");
    Serial.print(ESP.getMaxAllocHeap() / 1024);
    Serial.println(" KB");
    Serial.println("==============");
    initAnimationPlayer();
    initTouch();
    setupLed();

    Serial.println("RSC ready");
}

bool touchWasDown = false;

void loop() {
    serviceFaceRenderer();

    uint16_t x, y, z;
    bool touchNow = getTouchPoint(x, y, z);
    if (touchNow && !touchWasDown) {
        Serial.print("Touch X = ");
        Serial.print(x);
        Serial.print(" Y = ");
        Serial.println(y);
    }
    touchWasDown = touchNow;
}