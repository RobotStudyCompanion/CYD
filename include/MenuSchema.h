#pragma once
#include <stdint.h>

enum ActionKind : uint8_t { ACT_PUSH, ACT_INVOKE, ACT_BACK };

struct MenuScreen;
struct MenuItem {
    const char* label;
    ActionKind  kind;
    const void* payload;   // MenuScreen* or const char*
};
struct MenuScreen {
    const char*     title;
    const MenuItem* items;
    uint8_t         count;
};

extern const MenuScreen ROOT_MENU;
extern const MenuScreen MOOD_MENU;
extern const MenuScreen BRIGHTNESS_MENU;

const MenuScreen* getActiveRoot();
void resetRuntimeSchema();
bool runtimeBegin();
bool runtimeAddScreen(const char* title);
bool runtimeAddItem(ActionKind kind, const char* label, const char* payload);
bool runtimeEnd();