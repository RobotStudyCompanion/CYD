#include <Arduino.h>
#include "FaceRenderer.h"
#include "LED_Solution.h"
#include "TouchHandler.h"
#include "DisplayInit.h"
#include "DebugOverlay.h"
#include "Config.h"

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(50);
    printMemoryReport();
    initConfig(); //must come before initDisplay so persisted config applies
    initDisplay();
    initTouch();
    setupLed();
    
    Serial.println("RSC ready");
}

void loop() {
    serviceConfig();
    serviceFaceRenderer();
    serviceDebugOverlay();
}