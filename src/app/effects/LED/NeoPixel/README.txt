Doc Lib: https://adafruit.github.io/Adafruit_NeoPixel/html/class_adafruit___neo_pixel.html

How use:

#include "app/service/LED/NeoPixelService.hpp"
#include "app/effects/LED/NeoPixel/TheaterChaseEffect.hpp"

#define LED_PIN    16

NeoPixelService ledService;

TheaterChaseRainbowEffect theaterChaseEffect(&ledService, 50);

void loop()
{ 
    theaterChaseEffect.run();
}