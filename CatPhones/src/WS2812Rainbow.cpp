#include "WS2812Rainbow.h"

WS2812Rainbow::WS2812Rainbow(uint16_t numLeds, uint8_t pin)
    : strip(numLeds, pin, NEO_GRB + NEO_KHZ800), numLeds(numLeds) {}

void WS2812Rainbow::begin() {
    strip.begin();
    strip.show();
}

void WS2812Rainbow::showRainbow(uint8_t wait) {
    this->wait = wait;
    rainbowStep = 0;
}

void WS2812Rainbow::loop() {
    rainbowCycle();
    delay(wait);
}

void WS2812Rainbow::rainbowCycle() {
    for (uint16_t i = 0; i < numLeds; i++) {
        strip.setPixelColor(i, Wheel(((i * 256 / numLeds) + rainbowStep) & 255));
    }
    strip.show();
    rainbowStep = (rainbowStep + 1) % 256;
}

uint32_t WS2812Rainbow::Wheel(byte WheelPos) {
    WheelPos = 255 - WheelPos;
    if (WheelPos < 85) {
        return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if (WheelPos < 170) {
        WheelPos -= 85;
        return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void WS2812Rainbow::setBrightness(uint8_t brightness) {
    this->brightness = brightness;
    strip.setBrightness(brightness);
    strip.show();
}
