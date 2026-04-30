#pragma once
#include <Arduino_GFX_Library.h>

void initFaceRenderer(Arduino_GFX *gfx);
void showSplash();
void serviceFaceRenderer();  // call every loop()