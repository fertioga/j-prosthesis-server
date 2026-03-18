#pragma once
#include "ILedEffect.hpp"
#include "app/service/LED/NeoPixelService.hpp"

class ColorWipeEffect : public ILedEffect
{
    public:

        ColorWipeEffect(NeoPixelService* ledService, uint8_t wait, uint32_t color) : ledService(ledService), wait(wait), color(color) {};
        void run(uint8_t r, uint8_t g, uint8_t b) override
        {
            static uint16_t current_pixel = 0;
            pixelInterval = wait;                                       //  Update delay time
            ledService->strip->setPixelColor(current_pixel++, color);   //  Set pixel's color (in RAM)
            ledService->strip->show();                                  //  Update strip to match
            if(current_pixel >= ledService->strip->numPixels()) {       //  Loop the pattern from the first LED
                current_pixel = 0;
                patternComplete = true;
            }
        }

    private:
        NeoPixelService* ledService;
        uint8_t wait;
        int pixelInterval;      // Pixel Interval (ms) 
        bool patternComplete = false;
        uint32_t color;
};