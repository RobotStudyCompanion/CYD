#include "FaceRenderer.h"
#include <Grobot_Animations.h>
#include "Config.h"

// ----- Sprite + Grobot state -----
static TFT_eSPI    *_tft     = nullptr;
static TFT_eSprite *_canvas  = nullptr;
static GrobotEyes  *_eyes    = nullptr;

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
    delay(800);
    _tft->fillScreen(config.bgColour);
}

void serviceFaceRenderer()
{
    if (!_tft || !_canvas || !_eyes) return;
    if (config.moodAutoCycle) _eyes->moodSwitch(true);
    _eyes->renderEmotions(*_canvas);
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