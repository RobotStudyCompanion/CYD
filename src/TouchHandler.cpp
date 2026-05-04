#include <Arduino.h>
#include <SPI.h>
#include <XPT2046_Touchscreen.h>
#include "TouchHandler.h"

#define TOUCH_IRQ  36
#define TOUCH_MOSI 32
#define TOUCH_MISO 39
#define TOUCH_CLK  25
#define TOUCH_CS   33

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

SPIClass touchSPI = SPIClass(HSPI);
XPT2046_Touchscreen touchscreen(TOUCH_CS, TOUCH_IRQ);

void initTouch()
{
    pinMode(TOUCH_CS, OUTPUT);
    digitalWrite(TOUCH_CS, HIGH);

    pinMode(TOUCH_IRQ, INPUT);

    touchSPI.begin(TOUCH_CLK, TOUCH_MISO, TOUCH_MOSI, TOUCH_CS);
    touchscreen.begin(touchSPI);
    touchscreen.setRotation(1);
}

bool getTouchPoint(uint16_t &x, uint16_t &y, uint16_t &z)
{
    // XPT2046 IRQ on active-low:
    // IRQ = 1 tähendab ei vajuta
    // IRQ = 0 tähendab vajutab
    if (digitalRead(TOUCH_IRQ) == HIGH)
    {
        return false;
    }

    TS_Point p = touchscreen.getPoint();

    // liiga nõrk või tühi vajutus
    if (p.z < 200)
    {
        return false;
    }

    // raw sanity check
    if (p.x == 0 && p.y == 0)
    {
        return false;
    }

    x = map(p.x, 200, 3700, 0, SCREEN_WIDTH - 1);
    y = map(p.y, 240, 3800, 0, SCREEN_HEIGHT - 1);

    x = constrain(x, 0, SCREEN_WIDTH - 1);
    y = constrain(y, 0, SCREEN_HEIGHT - 1);

    z = p.z;

    return true;
}