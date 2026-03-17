#include "NeoPixelService.hpp"

NeoPixelService::NeoPixelService(uint8_t pin, uint16_t count) {
                        // Argument 1 = Number of pixels in NeoPixel strip
                        // Argument 2 = Arduino pin number (most are valid)
                        // Argument 3 = Pixel type flags, add together as needed:
                        //   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
                        //   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
                        //   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
                        //   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
                        //   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
  strip = new Adafruit_NeoPixel(count, pin, NEO_GRB + NEO_KHZ800);
}  

void NeoPixelService::begin()
{
    strip->begin();                   // INITIALIZE NeoPixel strip object (REQUIRED) --- IGNORE ---
    strip->show();                    // Turn OFF all pixels ASAP --- IGNORE ---
    strip->setBrightness(Brightness); // Set BRIGHTNESS to about 1/5 (max = 255) --- IGNORE ---
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t NeoPixelService::wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip->Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip->Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip->Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

