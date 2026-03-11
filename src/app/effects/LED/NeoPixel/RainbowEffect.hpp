#include "ILedEffect.hpp"
#include "app/service/LED/NeoPixelService.hpp"

#define RainbowLED_COUNT 60 // Total Number of Pixels
#define RainbowPixelInterval 50 // Pixel Interval (ms)
#define RainbowPixelCycle 0 // Pattern Pixel Cycle

class RainbowEffect : public ILedEffect
{
    public:

        RainbowEffect(NeoPixelService* ledService, uint8_t wait) : ledService(ledService), wait(wait) {}
        void run() override
        {
            if(pixelInterval != wait)
                pixelInterval = wait;                   
            for(uint16_t i=0; i < pixelNumber; i++) {
                ledService->strip->setPixelColor(i, ledService->wheel((i + pixelCycle) & 255)); //  Update delay time  
            }
            ledService->strip->show();  //  Update strip to match
            pixelCycle++;               //  Advance current cycle
            if(pixelCycle >= 256)
                pixelCycle = 0;         // Loop the cycle back to the begining
        }

    private:
        NeoPixelService* ledService;
        uint8_t wait;
        int pixelInterval = RainbowPixelInterval; // Pixel Interval (ms) xx
        int pixelCycle = RainbowPixelCycle;       // Pattern Pixel Cycle xx
        uint16_t pixelNumber = RainbowLED_COUNT;  // Total Number of Pixels xx
};