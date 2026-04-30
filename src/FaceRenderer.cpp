#include "FaceRenderer.h"

static Arduino_GFX *_gfx = nullptr;

// Orbiting dot state
static const int   ORBIT_RADIUS    = 65;
static const int   DOT_RADIUS      = 7;
static const float ANGULAR_SPEED   = 0.0015f;  // was 0.004f — slower
static const uint32_t FRAME_INTERVAL_MS = 16;  // ~60 fps cap
static const uint16_t DOT_COLOUR   = 0xFFFF;  // yellow
static const uint16_t BG_COLOUR    = 0x0000;  // black

static int _centreX = 0;
static int _centreY = 0;
static int _lastX = -100;
static int _lastY = -100;

void initFaceRenderer(Arduino_GFX *gfx)
{
    _gfx = gfx;
    _centreX = _gfx->width()  / 2;
    _centreY = _gfx->height() / 2;
}

void showSplash()
{
    if (!_gfx) return;
    _gfx->fillScreen(BG_COLOUR);
    _gfx->setTextColor(0xFFFF);
    _gfx->setTextSize(3);
    _gfx->setCursor(_centreX - 30, _centreY - 12);
    _gfx->print("RSC");
    delay(800);
    _gfx->fillScreen(BG_COLOUR);
}

void serviceFaceRenderer()
{
    if (!_gfx) return;

    static uint32_t lastFrame = 0;
    uint32_t now = millis();
    if (now - lastFrame < FRAME_INTERVAL_MS) return;
    lastFrame = now;

    float angle = now * ANGULAR_SPEED;
    int x = _centreX + (int)(cosf(angle) * ORBIT_RADIUS);
    int y = _centreY + (int)(sinf(angle) * ORBIT_RADIUS);

    if (x == _lastX && y == _lastY) return;  // no movement, no redraw

    // Draw new first so there's no "off" window at the dot location
    _gfx->fillCircle(x, y, DOT_RADIUS, DOT_COLOUR);

    // Erase old only if it doesn't overlap the new dot
    if (_lastX != -100) {
        int dx = x - _lastX;
        int dy = y - _lastY;
        int distSq = dx*dx + dy*dy;
        int minSq = (2 * DOT_RADIUS) * (2 * DOT_RADIUS);
        if (distSq >= minSq) {
            // Fully disjoint — safe to erase whole old dot
            _gfx->fillCircle(_lastX, _lastY, DOT_RADIUS, BG_COLOUR);
        } else if (distSq > 0) {
            // Partial overlap — erase old, redraw new
            _gfx->fillCircle(_lastX, _lastY, DOT_RADIUS, BG_COLOUR);
            _gfx->fillCircle(x, y, DOT_RADIUS, DOT_COLOUR);
        }
    }
    _lastX = x;
    _lastY = y;
}