#pragma once
#include "ILedEffect.hpp"
#include "app/service/LED/NeoPixelService.hpp"

#define TheaterChaseRainbowLED_COUNT 60 // Total Number of Pixels
#define TheaterChaseRainbowPixelInterval 50 // Pixel Interval (ms)
#define TheaterChaseRainbowPixelCycle 0 // Pattern Pixel Cycle

class TheaterChaseRainbowEffect : public ILedEffect
{
    public:

        TheaterChaseRainbowEffect(NeoPixelService* ledService, uint8_t wait) : ledService(ledService), wait(wait) {};
        void run() override
        {
            if(pixelInterval != wait)
                pixelInterval = wait;                   

            for(int c=0; c < 3; c++) {
                ledService->strip->clear();
                for(int i=c; i < pixelNumber; i += 3) {
                    ledService->strip->setPixelColor(i, ledService->wheel((i + pixelCycle) % 255)); //  Update delay time  
                }
                ledService->strip->show();  //  Update strip to match
                delay(pixelInterval);
            }
            pixelCycle++;               //  Advance current cycle
            if(pixelCycle >= 256)
                pixelCycle = 0;         // Loop the cycle back to the begining
        }

    private:
        NeoPixelService* ledService;
        uint8_t wait;
        int pixelInterval = TheaterChaseRainbowPixelInterval; // Pixel Interval (ms) xx
        int pixelCycle = TheaterChaseRainbowPixelCycle;       // Pattern Pixel Cycle xx
        uint16_t pixelNumber = TheaterChaseRainbowLED_COUNT;  // Total Number of Pixels xx
};
