#pragma once
#include "ILedEffect.hpp"
#include "app/service/LED/NeoPixelService.hpp"

#define START_COLOR_DELAY 70 // Delay in milliseconds between setting each LED to the start color
#define FADE_OUT_DELAY 30 // Delay in milliseconds between each step of the fade-out effect
class StartColorEffect : public ILedEffect
{
    public:

        StartColorEffect(NeoPixelService* ledService, uint32_t color) : ledService(ledService), color(color) {};
        void run(uint8_t r, uint8_t g, uint8_t b) override
        {     
            turnOnLeds();
        }

        void turnOnLeds()
        {
             for(uint16_t i=0; i < ledService->strip->numPixels(); i++) {
                ledService->strip->setPixelColor(i, color); 
                
                vTaskDelay(START_COLOR_DELAY / portTICK_PERIOD_MS); //  Update delay time

                ledService->strip->show();  //  Update strip to match
            }
        }

        

    private:
        NeoPixelService* ledService;
        uint32_t color;
};