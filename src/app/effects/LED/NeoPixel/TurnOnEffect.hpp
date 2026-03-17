#pragma once

#include "app/service/LED/NeoPixelService.hpp"
#include "ILedEffect.hpp"


class TurnOnEffect : public ILedEffect
{   
    public:
        TurnOnEffect(NeoPixelService* ledService) : ledService(ledService) {};

        void setColor(uint8_t r, uint8_t g, uint8_t b) {
            color = ledService->strip->Color(r, g, b);
        }
        void run() override
        {
            for(uint16_t i=0; i < ledService->strip->numPixels(); i++) {
                ledService->strip->setPixelColor(i, color); // Set all pixels to the specified color
                
                vTaskDelay(pdMS_TO_TICKS(50)); //  Update delay time
            }
            ledService->strip->show();  //  Update strip to match
        }
    private:
        NeoPixelService* ledService;
        uint32_t color;
};
