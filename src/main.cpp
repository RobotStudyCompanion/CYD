#include <Arduino.h>
#include "FaceRenderer.h"
#include "LED_Solution.h"
#include "TouchHandler.h"
#include "AnimationPlayer.h"
#include "DebugOverlay.h"
#include "Config.h"

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
    initConfig(); //must come before initAnimationPlayer
    initAnimationPlayer();
    initTouch();
    setupLed();
    
    Serial.println("RSC ready");
}

void loop() {
    serviceConfig();
    serviceFaceRenderer();
    serviceDebugOverlay();
}