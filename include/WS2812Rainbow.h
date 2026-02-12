#ifndef WS2812_RAINBOW_H
#define WS2812_RAINBOW_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class WS2812Rainbow {
public:
    WS2812Rainbow(uint16_t numLeds, uint8_t pin);
    void begin();
    void showRainbow(uint8_t wait = 20);
    void loop();
    void setBrightness(uint8_t brightness); // 0-255

    void setSolidColor(uint8_t r, uint8_t g, uint8_t b); // 🆕 feste Farbe

private:
    Adafruit_NeoPixel strip;
    uint16_t numLeds;
    uint8_t rainbowStep = 0;
    uint8_t wait;
    uint8_t brightness = 20; // default brightness

    bool solidMode = false;        // 🆕 Modus
    uint32_t solidColor = 0;       // 🆕 gespeicherte Farbe


    void rainbowCycle();
    uint32_t Wheel(byte WheelPos);

};

#endif // WS2812_RAINBOW_H
