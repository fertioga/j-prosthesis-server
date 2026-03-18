#pragma once

#include "app/service/LED/NeoPixelService.hpp"
#include "ILedEffect.hpp"


class TurnOnEffect : public ILedEffect
{   
    public:
        TurnOnEffect(NeoPixelService* ledService) : ledService(ledService) {};

        void run(uint8_t r, uint8_t g, uint8_t b) override
        {
            for(uint16_t i=0; i < ledService->strip->numPixels(); i++) {
                ledService->strip->setPixelColor(i, ledService->strip->Color(r, g, b)); 
                
                vTaskDelay(pdMS_TO_TICKS(50)); 
            }
            ledService->strip->show(); 
        }
    private:
        NeoPixelService* ledService;
};
