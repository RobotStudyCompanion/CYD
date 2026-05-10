#include "MenuSchema.h"
#include <Arduino.h>
#include <string.h>

#define MAX_RT_SCREENS  8
#define MAX_RT_ITEMS    8
#define RT_POOL_SIZE    1024

static MenuScreen _rtScreens[MAX_RT_SCREENS];
static MenuItem   _rtItems[MAX_RT_SCREENS * MAX_RT_ITEMS];
static char       _rtPool[RT_POOL_SIZE];
static int        _rtScreenCount = 0;
static size_t     _rtPoolUsed = 0;
static int        _rtCurScreen = -1;
static bool       _rtLoading = false;
static bool       _rtActive  = false;

static const char* poolString(const char* s) {
    size_t n = strlen(s) + 1;
    if (_rtPoolUsed + n > RT_POOL_SIZE) return nullptr;
    char* d = &_rtPool[_rtPoolUsed];
    memcpy(d, s, n);
    _rtPoolUsed += n;
    return d;
}

static MenuItem* rtItemsFor(int screenIdx) {
    return &_rtItems[screenIdx * MAX_RT_ITEMS];
}

extern const MenuScreen ROOT_MENU;
extern const MenuScreen MOOD_MENU;
extern const MenuScreen BRIGHTNESS_MENU;
extern const MenuScreen STATUS_MENU;

static const MenuItem BRIGHTNESS_ITEMS[] = {
    {"25%",  ACT_INVOKE, "bright:25"},
    {"50%",  ACT_INVOKE, "bright:50"},
    {"75%",  ACT_INVOKE, "bright:75"},
    {"100%", ACT_INVOKE, "bright:100"},
    {"Back", ACT_BACK,   nullptr},
};
const MenuScreen BRIGHTNESS_MENU = {"Brightness", BRIGHTNESS_ITEMS, 5};

static const MenuItem MOOD_ITEMS[] = {
    {"Neutral", ACT_INVOKE, "mood:NEUTRAL"},
    {"Happy",   ACT_INVOKE, "mood:HAPPY"},
    {"Sad",     ACT_INVOKE, "mood:SAD"},
    {"Angry",   ACT_INVOKE, "mood:ANGRY"},
    {"Back",    ACT_BACK,   nullptr},
};
const MenuScreen MOOD_MENU = {"Mood", MOOD_ITEMS, 5};

static const MenuItem STATUS_ITEMS[] = {
    {"Bright", ACT_NONE, nullptr, "bright"},
    {"Mood",   ACT_NONE, nullptr, "mood"},
    {"Up",     ACT_NONE, nullptr, "uptime"},
    {"Mem",    ACT_NONE, nullptr, "mem"},
    {"Back",   ACT_BACK, nullptr},
};
const MenuScreen STATUS_MENU = {"Status", STATUS_ITEMS, 5};

static const MenuItem ROOT_ITEMS[] = {
    {"Brightness", ACT_PUSH,   &BRIGHTNESS_MENU, "bright"},
    {"Mood",       ACT_PUSH,   &MOOD_MENU,       "mood"},
    {"Status",     ACT_PUSH,   &STATUS_MENU,     nullptr},
    {"Blink",      ACT_INVOKE, "blink"},
    {"Resume",     ACT_BACK,   nullptr},
};
const MenuScreen ROOT_MENU = {"Menu", ROOT_ITEMS, 5};


void resetRuntimeSchema() {
    _rtScreenCount = 0; _rtPoolUsed = 0;
    _rtCurScreen = -1; _rtLoading = false; _rtActive = false;
}

bool runtimeBegin() { resetRuntimeSchema(); _rtLoading = true; return true; }

bool runtimeAddScreen(const char* title) {
    if (!_rtLoading || _rtScreenCount >= MAX_RT_SCREENS) return false;
    const char* t = poolString(title);
    if (!t) return false;
    _rtCurScreen = _rtScreenCount++;
    _rtScreens[_rtCurScreen] = {t, rtItemsFor(_rtCurScreen), 0};
    return true;
}

bool runtimeAddItem(ActionKind kind, const char* label, const char* payload, const char* formatCmd) {
    if (!_rtLoading || _rtCurScreen < 0) return false;
    MenuScreen& s = _rtScreens[_rtCurScreen];
    if (s.count >= MAX_RT_ITEMS) return false;
    const char* lbl = poolString(label);
    if (!lbl) return false;
    MenuItem& it = rtItemsFor(_rtCurScreen)[s.count++];
    it.label = lbl;
    it.kind = kind;
    if (kind == ACT_PUSH) {
        it.payload = (const void*)(uintptr_t)atoi(payload);   // resolved in runtimeEnd
    } else if (kind == ACT_INVOKE) {
        const char* p = poolString(payload);
        if (!p) return false;
        it.payload = p;
    } else {
        it.payload = nullptr;
    }
    if (formatCmd && *formatCmd) {
        const char* f = poolString(formatCmd);
        if (!f) return false;
        it.formatCmd = f;
    } else {
        it.formatCmd = nullptr;
    }
    return true;
}

bool runtimeEnd() {
    if (!_rtLoading) return false;
    // Fix up PUSH payloads from indices to pointers
    for (int s = 0; s < _rtScreenCount; s++) {
        MenuItem* items = rtItemsFor(s);
        for (int i = 0; i < _rtScreens[s].count; i++) {
            if (items[i].kind == ACT_PUSH) {
                int idx = (int)(uintptr_t)items[i].payload;
                if (idx < 0 || idx >= _rtScreenCount) return false;
                items[i].payload = &_rtScreens[idx];
            }
        }
    }
    _rtLoading = false;
    _rtActive = true;
    return true;
}

const MenuScreen* getActiveRoot() {
    return _rtActive ? &_rtScreens[0] : &ROOT_MENU;
}