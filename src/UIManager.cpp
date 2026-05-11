#include <Arduino.h>
#include "UIManager.h"
#include "TouchHandler.h"
#include "Config.h"
#include "FaceRenderer.h"
#include "MenuApp.h"
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

static UIMode _mode = MODE_FACE;

static void enterMenu() {
    _mode = MODE_MENU;
    pauseFace();
    tft.fillScreen(TFT_BLACK);
    gslc_SetPageCur(&m_gui, MENU_PG_ROOT);
    gslc_PageRedrawSet(&m_gui, true);   // force full redraw on entry
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
    bool gotEvent = pollTouchEvent(ev);   // also refreshes peekLastTouch cache

    // Long-press toggles mode in either direction.
    // TAP events fall through — GUIslice handles taps via peekLastTouch.
    if (gotEvent && ev.kind == TOUCH_LONG_PRESS) {
        if (_mode == MODE_FACE) {
            enterMenu();
            Serial.println("OK: entered MENU");
        } else {
            exitMenu();
            Serial.println("OK: returned to FACE");
        }
    }

    if (_mode == MODE_MENU) {
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

void printMode() {
    Serial.printf("mode:           %s\n", _mode == MODE_FACE ? "FACE" : "MENU");
}

void printMenuState() {
    Serial.printf("menu:           %s (GUIslice-driven)\n",
                  _mode == MODE_MENU ? "MENU" : "FACE");
}