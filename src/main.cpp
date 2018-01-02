#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define PIN D1

Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ400);

void setup() {
    strip.begin();
    strip.show();
}

void loop() {

  for(uint16_t i=1; i<strip.numPixels()-1; i++) {
    strip.setPixelColor(i-1, strip.Color(0, 32, 0));
    strip.setPixelColor(i, strip.Color(0, 64, 0));
    strip.setPixelColor(i+1, strip.Color(0, 255, 0));
    strip.show();
    delay(50);
    strip.clear();
  }


  for(uint16_t i=strip.numPixels()-1; i>1; i--) {
    strip.setPixelColor(i-1, strip.Color(0, 255, 0));
    strip.setPixelColor(i, strip.Color(0, 64, 0));
    strip.setPixelColor(i+1, strip.Color(0, 32, 0));
    strip.show();
    delay(50);
    strip.clear();
  }


}