#include "FaceRenderer.h"
#include <Grobot_Animations.h>
#include "Config.h"

// ----- Sprite + Grobot state -----
static TFT_eSPI    *_tft     = nullptr;
static TFT_eSprite *_canvas  = nullptr;
static GrobotEyes  *_eyes    = nullptr;
static int16_t _lookX = 0, _lookY = 0;
static bool _hudOn = false;
static FaceShape _customL = {0, 0, 0, 30, 45};   // default = MOOD_NEUTRAL
static FaceShape _customR = {0, 0, 0, 30, 45};
static uint32_t _splashUntil = 0;             // 0 = inactive; else millis() at which to clear
static constexpr uint32_t SPLASH_DURATION_MS = 800;
static bool _paused = false;

// Internal helper — (re)builds the eyes with current colour state.
static void rebuildEyes()
{
    if (_eyes) { delete _eyes; _eyes = nullptr; }
    _eyes = new GrobotEyes(config.eyeColour, config.bgColour);
    _eyes->setEmotion(String(config.mood));
}

void initFaceRenderer(TFT_eSPI *tft)
{
    _tft = tft;

    _canvas = new TFT_eSprite(_tft);
    if (!_canvas->createSprite(320, 140)) {     // was (320, 120)
        Serial.println("FaceRenderer: sprite alloc failed (heap fragmented?)");
        delete _canvas;
        _canvas = nullptr;
        return;
    }
    _canvas->fillSprite(config.bgColour);

    rebuildEyes();
    _hudOn = config.hudOn;
}

void showSplash()
{
    if (!_tft) return;
    _tft->fillScreen(config.bgColour);
    _tft->setTextColor(config.eyeColour);
    _tft->setTextSize(3);
    int cx = _tft->width() / 2;
    int cy = _tft->height() / 2;
    _tft->setCursor(cx - 65, cy - 12);
    _tft->print("RSC-CYD");
    _splashUntil = millis() + SPLASH_DURATION_MS;
}

void serviceFaceRenderer()
{
    if (!_tft || !_canvas || !_eyes) return;

    if (_splashUntil != 0) {
        if (millis() < _splashUntil) return;     // still showing splash, skip render
        _tft->fillScreen(config.bgColour);       // splash done, clear before face takes over
        _splashUntil = 0;
    }
    if (_paused) return;   // <-- new
    if (config.moodAutoCycle) _eyes->moodSwitch(true);
    _eyes->renderEmotions(*_canvas);
    if (_hudOn) _eyes->HUD(*_tft);
    
}

void setEyeColour(uint16_t rgb565)
{
    if (config.eyeColour == rgb565) return;
    config.eyeColour = rgb565;
    if (_eyes) rebuildEyes();
}

void setBackgroundColour(uint16_t rgb565)
{
    if (config.bgColour == rgb565) return;
    config.bgColour = rgb565;
    if (_canvas) _canvas->fillSprite(config.bgColour);
    if (_eyes) rebuildEyes();
}

void drawTouchMarker(int x, int y)
{
    if (!_tft) return;
    _tft->fillCircle(x, y, 5, TFT_GREEN);
}

void setMood(const char *moodName)
{
    strncpy(config.mood, moodName, sizeof(config.mood) - 1);
    config.mood[sizeof(config.mood) - 1] = '\0';
    if (_eyes) _eyes->setEmotion(String(moodName));
}

void setLookAt(int16_t x, int16_t y) { if (!_eyes) return; _lookX = x; _lookY = y; _eyes->lookAt(x, y); }
void getLookAt(int16_t &x, int16_t &y) { x = _lookX; y = _lookY; }
void triggerBlink() { if (_eyes) _eyes->blink(); }
void setHud(bool on) { _hudOn = on; }

static MoodData toMood(const FaceShape &s) {
    return MoodData{s.topH, s.botH, s.tilt, s.pR, s.radius};
}

void setFaceCustom(const FaceShape &shape) {
    _customL = shape;
    _customR = shape;
    if (_eyes) _eyes->setEmotion(toMood(shape));
}
void setFaceLeft(const FaceShape &shape) {
    _customL = shape;
    if (_eyes) _eyes->setEmotion(toMood(_customL), toMood(_customR));
}
void setFaceRight(const FaceShape &shape) {
    _customR = shape;
    if (_eyes) _eyes->setEmotion(toMood(_customL), toMood(_customR));
}
void getFaceLeft (FaceShape &out) { out = _customL; }
void getFaceRight(FaceShape &out) { out = _customR; }
void pauseFace()    { _paused = true; }
void resumeFace()   { _paused = false; }
bool isFacePaused() { return _paused; }