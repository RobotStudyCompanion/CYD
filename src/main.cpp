#include <Arduino.h>
#include "FaceRenderer.h"
#include "LED_Solution.h"
#include "TouchHandler.h"
#include "DisplayInit.h"
#include "DebugOverlay.h"
#include "LdrSensor.h"
#include "Config.h"
#include "UIManager.h"
#include "MenuApp.h"
#include "version.h"

static CydTouchHandler m_touch;

static int16_t DebugOut(char ch) {
    if (ch == '\n') Serial.println("");
    else Serial.write(ch);
    return 0;
}

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(50);
    printVersion();
    printMemoryReport();

    initConfig();    // NVS — must precede initDisplay (persisted brightness etc.)
    initDisplay();   // TFT_eSPI + splash
    initTouch();     // CYD28_TouchR — sole hardware owner

    // GUIslice — re-inits TFT_eSPI internally; brief flicker is expected
    gslc_InitDebug(&DebugOut);
    gslc_InitTouchHandler(&m_touch);
    InitGUIslice_gen();
    gslc_GuiRotate(&m_gui, 3);    // flip menu + touch axes
    reattachBacklight(); // restore PWM control of backlight
    initMenuIcons();
    initUI(); // _mode = FACE
    setupLed();
    Serial.println("RSC ready");
}

void loop() {
    serviceConfig();
    serviceNvs();
    if (getUIMode() == MODE_FACE) serviceFaceRenderer();
    serviceUI();              // mode-orchestrates GUIslice + long-press
    serviceDebugOverlay();
    serviceLdr();
}