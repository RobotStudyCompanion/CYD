#include <Arduino.h>
#include "UIManager.h"
#include "TouchHandler.h"
#include "Config.h"
#include "FaceRenderer.h"
#include "MenuApp.h"
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

static UIMode _mode = MODE_FACE;
static uint32_t _lastActivityMs = 0;
extern void refreshStatsText();

static void enterMenu() {
    _mode = MODE_MENU;
    pauseFace();
    tft.fillScreen(TFT_BLACK);
    gslc_SetPageCur(&m_gui, MENU_PG_ROOT);
    gslc_PageRedrawSet(&m_gui, true);   // force full redraw on entry
    _lastActivityMs = millis();
    refreshStatsText();
}

static void exitMenu() {
    _mode = MODE_FACE;
    tft.fillScreen(config.bgColour);
    resetFaceState();
    resumeFace();
}

void initUI() {
    _mode = MODE_FACE;
}

void serviceUI() {
    TouchEvent ev;
    bool gotEvent = pollTouchEvent(ev);

    // Enter menu on long-press from face (one-way)
    if (gotEvent && ev.kind == TOUCH_LONG_PRESS && _mode == MODE_FACE) {
        enterMenu();
        Serial.println("OK: entered MENU");
    }

    if (_mode == MODE_MENU) {
        // Any finger contact resets activity timer (taps, drags, holds all count)
        uint16_t tx, ty, tz;
        if (peekLastTouch(tx, ty, tz)) {
            _lastActivityMs = millis();
        }

        // Non-blocking idle check
        if (!config.touchDebug && config.menuTimeoutSec > 0 && (millis() - _lastActivityMs) >= (uint32_t)config.menuTimeoutSec * 1000) {
            exitMenu();
            Serial.println("OK: returned to FACE (idle)");
            return;
        }

        // 1Hz stats refresh while in menu
        static uint32_t _lastStatsMs = 0;
        if (millis() - _lastStatsMs >= 1000) {
            refreshStatsText();
            _lastStatsMs = millis();
        }

        gslc_Update(&m_gui);
    }
}

UIMode getUIMode() { return _mode; }

void setUIMode(UIMode m) {
    if (m == MODE_MENU && _mode == MODE_FACE) enterMenu();
    else if (m == MODE_FACE && _mode == MODE_MENU) exitMenu();
}

void menuBack() {
    if (_mode == MODE_MENU) exitMenu();
}

bool menuSelect(uint8_t idx) {
    (void)idx;
    Serial.println("ERR: menu_select deprecated — use touch UI");
    return false;
}

void cmdMenuBack() {
    menuBack();
    Serial.println("OK");
}

void cmdMenuState() {
    printMenuState();
}

void printMode() {
    Serial.printf("mode:           %s\n", _mode == MODE_FACE ? "FACE" : "MENU");
}

void printMenuState() {
    Serial.printf("menu:           %s (GUIslice-driven)\n",
                  _mode == MODE_MENU ? "MENU" : "FACE");
}