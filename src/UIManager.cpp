#include <Arduino.h>
#include "UIManager.h"
#include "MenuSchema.h"
#include "TouchHandler.h"
#include "Config.h"
#include <TFT_eSPI.h>
#include "FaceRenderer.h"

extern TFT_eSPI tft;

static bool _dirty = false;
static unsigned long _lastMenuRender = 0;

static UIMode _mode = MODE_FACE;
static const MenuScreen* _stack[8];
static uint8_t _depth = 0;

static String _lastRowText[8];   // matches MAX_RT_ITEMS in MenuSchema.cpp

static int  hitTest(uint16_t x, uint16_t y);
static void renderMenu();
static void renderRows();

static void enterMenu() {
    _mode = MODE_MENU;
    _stack[0] = getActiveRoot();
    _depth = 1;
    pauseFace();
    tft.fillScreen(TFT_BLACK);
    _dirty = true;
}
static void exitMenu() {
    _mode = MODE_FACE;
    _depth = 0;
    tft.fillScreen(config.bgColour);
    resetFaceState();
    resumeFace();
}

void initUI() { _mode = MODE_FACE; _depth = 0; }

void serviceUI() {
    TouchEvent ev;
    if (pollTouchEvent(ev)) {
        #ifdef TOUCH_DEBUG
        Serial.printf("touch: %s at (%u, %u)\n",
                      ev.kind == TOUCH_TAP ? "TAP" : "LONG_PRESS", ev.x, ev.y);
        #endif

        if (ev.kind == TOUCH_LONG_PRESS && _mode == MODE_FACE) {
            enterMenu();
            Serial.println("OK: entered MENU");
        }
        else if (ev.kind == TOUCH_LONG_PRESS && _mode == MODE_MENU) {
            exitMenu();
            Serial.println("OK: returned to FACE");
        }
        else if (ev.kind == TOUCH_TAP && _mode == MODE_MENU) {
            int idx = hitTest(ev.x, ev.y);
            if (idx >= 0) {
                const MenuScreen* s = _stack[_depth - 1];
                const MenuItem& it = s->items[idx];
                const char* k = it.kind == ACT_PUSH   ? "PUSH"
                              : it.kind == ACT_INVOKE ? "INVOKE"
                              : it.kind == ACT_BACK   ? "BACK" : "NONE";
                Serial.printf("MENU: tap row %d (%s, %s)\n", idx, it.label, k);
                menuSelect((uint8_t)idx);
            } else {
                Serial.printf("MENU: tap miss at (%u, %u)\n", ev.x, ev.y);
            }
}
    }
    auto screenHasFormatters = []() -> bool {
    if (_depth == 0) return false;
        const MenuScreen* s = _stack[_depth - 1];
        for (uint8_t i = 0; i < s->count; i++) if (s->items[i].formatCmd) return true;
        return false;
    };

    if (_mode == MODE_MENU && _dirty) {
        renderMenu();
        _lastMenuRender = millis();
        _dirty = false;
    }
    else if (_mode == MODE_MENU && screenHasFormatters() && (millis() - _lastMenuRender) > 250) {
        renderRows();
        _lastMenuRender = millis();
    }
}

UIMode getUIMode() { return _mode; }

void setUIMode(UIMode m) {
    if (m == MODE_MENU && _mode == MODE_FACE) enterMenu();
    else if (m == MODE_FACE && _mode == MODE_MENU) exitMenu();
}

void menuBack() {
    if (_mode != MODE_MENU || _depth == 0) return;
    _depth--;
    if (_depth == 0) exitMenu();
    else _dirty = true;
}

bool menuSelect(uint8_t idx) {
    if (_mode != MODE_MENU || _depth == 0) return false;
    const MenuScreen* s = _stack[_depth - 1];
    if (idx >= s->count) return false;
    const MenuItem& it = s->items[idx];

    switch (it.kind) {
        case ACT_PUSH:
            if (_depth >= 8) return false;
            _stack[_depth++] = (const MenuScreen*)it.payload;
            _dirty = true;
            return true;
        case ACT_BACK:
            menuBack();
            return true;
        case ACT_INVOKE: {
            String line = (const char*)it.payload;
            int colon = line.indexOf(':');
            String key = colon >= 0 ? line.substring(0, colon) : line;
            String val = colon >= 0 ? line.substring(colon + 1) : String();
            const Command* c = findCommand(key);
            if (!c) { Serial.printf("ERR: invoke '%s'\n", line.c_str()); return false; }
            if (colon >= 0 && c->set) c->set(val);
            else if (c->get) c->get();
            return true;
        }
        case ACT_NONE:
            return true;  // acknowledged, no action
    }
    return false;
}

void printMode() {
    Serial.printf("mode:           %s\n", _mode == MODE_FACE ? "FACE" : "MENU");
}

void printMenuState() {
    if (_mode != MODE_MENU || _depth == 0) {
        Serial.println("menu:           (FACE mode)");
        return;
    }
    const MenuScreen* s = _stack[_depth - 1];
    Serial.printf("menu:           %s (depth %u)\n", s->title, _depth);
    for (uint8_t i = 0; i < s->count; i++) {
        const MenuItem& it = s->items[i];
        const char* k = it.kind == ACT_PUSH   ? "PUSH"
                      : it.kind == ACT_INVOKE ? "INVOKE"
                      : it.kind == ACT_BACK   ? "BACK" : "NONE";
        Serial.printf("  [%u] %-16s %s\n", i, it.label, k);
    }
}

static int hitTest(uint16_t x, uint16_t y) {
    if (_mode != MODE_MENU || _depth == 0) return -1;
    const MenuScreen* s = _stack[_depth - 1];
    const int top = 48, row_h = 36;
    if (y < top) return -1;
    int idx = (y - top) / row_h;
    return (idx < 0 || idx >= s->count) ? -1 : idx;
}

static void renderRows() {
    if (_mode != MODE_MENU || _depth == 0) return;
    const MenuScreen* s = _stack[_depth - 1];
    tft.setTextSize(1);
    tft.setFreeFont(&FreeSans12pt7b);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextDatum(TL_DATUM);
    char buf[64];
    int y = 48;
    for (uint8_t i = 0; i < s->count; i++) {
        const MenuItem& it = s->items[i];
        if (it.formatCmd) {
            const Command* c = findCommand(it.formatCmd);
            if (c && c->format) {
                snprintf(buf, sizeof(buf), "%u  %s: %s", i, it.label, c->format().c_str());
            } else {
                snprintf(buf, sizeof(buf), "%u  %s", i, it.label);
            }
        } else {
            snprintf(buf, sizeof(buf), "%u  %s", i, it.label);
        }
        if (_lastRowText[i] != buf) {
            tft.fillRect(0, y - 4, 320, 32, TFT_BLACK);
            tft.drawString(buf, 12, y);
            _lastRowText[i] = buf;
        }
        y += 36;
    }
}

static void renderMenu() {
    if (_mode != MODE_MENU || _depth == 0) return;
    const MenuScreen* s = _stack[_depth - 1];
    tft.fillScreen(TFT_BLACK);
    for (int i = 0; i < 8; i++) _lastRowText[i] = "";
    tft.setTextSize(1);
    tft.setFreeFont(&FreeSansBold12pt7b);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextDatum(TL_DATUM);
    tft.drawString(s->title, 12, 8);
    tft.drawFastHLine(0, 36, 320, TFT_DARKGREY);
    renderRows();
}