#pragma once

#include "ILedEffect.hpp"
#include "app/service/LED/NeoPixelService.hpp"
#include "app/effects/LED/NeoPixel/TurnOffEffect.hpp"

class StroboEffect: public ILedEffect
{
    public:
        int stage = 0;
        const int TIME_INTERVAL = 200;

        StroboEffect(NeoPixelService* ledService): 
            ledService(ledService),
            turnOffEffect(ledService)
        {}

        void run(uint8_t r, uint8_t g, uint8_t b) override
        {
            if(stage==0)
            {
                turnOn(r, g, b);
                stage = 1;
            }

            if(stage==1)
            {
                
                turnOff();
                stage = 0;
            }
        }

    private:
        NeoPixelService* ledService;
        TurnOffEffect   turnOffEffect;

        void turnOn(uint8_t r, uint8_t g, uint8_t b)
        {
            for(uint16_t i=0; i < ledService->strip->numPixels(); i++) {
                ledService->strip->setPixelColor(i, ledService->strip->Color(r, g, b)); 
            }     
            ledService->strip->show();

            vTaskDelay(pdMS_TO_TICKS(TIME_INTERVAL));
        }

        void turnOff()
        {
            for(uint16_t i=0; i < ledService->strip->numPixels(); i++) {
                ledService->strip->setPixelColor(i, ledService->strip->Color(0, 0, 0)); 
            }     
            ledService->strip->show();  
            
            vTaskDelay(pdMS_TO_TICKS(TIME_INTERVAL));
        }
};