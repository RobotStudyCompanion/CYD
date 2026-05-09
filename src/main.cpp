#include <Arduino.h>
#include "FaceRenderer.h"
#include "LED_Solution.h"
#include "TouchHandler.h"
#include "DisplayInit.h"
#include "DebugOverlay.h"
#include "LdrSensor.h"
#include "Config.h"
#include "UIManager.h"

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(50);
    printVersion();
    printMemoryReport();
    initConfig(); //must come before initDisplay so persisted config applies
    initDisplay();
    initTouch();
    initUI();
    setupLed();
    Serial.println("RSC ready");
}

void loop() {
    serviceConfig();
    serviceNvs();
    if (getUIMode() == MODE_FACE) serviceFaceRenderer();
    serviceUI();
    serviceDebugOverlay();
    serviceLdr();
}