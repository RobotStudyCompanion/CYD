#pragma once
#include <stdint.h>
#include "MenuSchema.h"

enum UIMode : uint8_t { MODE_FACE, MODE_MENU };

void initUI();
void serviceUI();

UIMode getUIMode();
void   setUIMode(UIMode m);

void   menuBack();
bool   menuSelect(uint8_t idx);
void   printMode();
void   printMenuState();