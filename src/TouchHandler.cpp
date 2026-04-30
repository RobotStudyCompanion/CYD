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
    Serial.print("IRQ = ");
    Serial.print(digitalRead(TOUCH_IRQ));

    if (!touchscreen.touched())
    {
        Serial.println(" | not touched");
        return false;
    }

    TS_Point p = touchscreen.getPoint();

    Serial.print(" | RAW X = ");
    Serial.print(p.x);
    Serial.print(" | RAW Y = ");
    Serial.print(p.y);
    Serial.print(" | RAW Z = ");
    Serial.println(p.z);

    x = p.x;
    y = p.y;
    z = p.z;

    return true;
}