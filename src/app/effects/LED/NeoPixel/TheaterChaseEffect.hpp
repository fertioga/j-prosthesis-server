#pragma once
#include "ILedEffect.hpp"
#include "app/service/LED/NeoPixelService.hpp"

class TheaterChaseEffect : public ILedEffect
{
    public:

        TheaterChaseEffect(NeoPixelService* ledService, uint8_t wait, uint32_t color) : ledService(ledService), wait(wait), color(color) {};
        void run() override
        {
                static uint32_t loop_count = 0;
                static uint16_t current_pixel = 0;

                pixelInterval = wait;                   //  Update delay time

                ledService->strip->clear();

                for(int c=current_pixel; c < pixelNumber; c += 3) {
                    ledService->strip->setPixelColor(c, color);
                }
                ledService->strip->show();

                current_pixel++;
                if (current_pixel >= 3) {
                    current_pixel = 0;
                    loop_count++;
                }

                if (loop_count >= 10) {
                    current_pixel = 0;
                    loop_count = 0;
                    patternComplete = true;
                }
        };

    private:
        NeoPixelService* ledService;
        uint8_t wait;
        int pixelInterval; // Pixel Interval (ms) 
        uint16_t pixelNumber;  // Total Number of Pixels 
        bool patternComplete = false;
        uint32_t color;
};