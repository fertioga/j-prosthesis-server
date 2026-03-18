#pragma once

#include "ILedEffect.hpp"
#include "app/service/LED/NeoPixelService.hpp"

class PolicyEffect: public ILedEffect
{
    public:
        int redBeginPixel;
        int blueBeginPixel;
        int stage = 0;
        const int TIME_INTERVAL = 100;

        PolicyEffect(NeoPixelService* ledService): ledService(ledService)
        {
            redBeginPixel = 0;
            blueBeginPixel = (ledService->strip->numPixels() / 2);
        }

        void run(uint8_t r, uint8_t g, uint8_t b)
        {
            
            if(stage==0){
                blueOff();
                redOn();
                stage=1;
                vTaskDelay(pdMS_TO_TICKS(TIME_INTERVAL));
            }

            if(stage==1){
                redOff();
                blueOn();
                stage=0;
                vTaskDelay(pdMS_TO_TICKS(TIME_INTERVAL));
            }

        }

        void redOn()
        {
            for(int i=redBeginPixel; i < (ledService->strip->numPixels() / 2); i++)
            {
                ledService->strip->setPixelColor(i,0xFF0000);
            }
            ledService->strip->show();
        }

        void redOff()
        {
            for(int i=redBeginPixel; i < (ledService->strip->numPixels() / 2); i++)
            {
                ledService->strip->setPixelColor(i,0x000000);
            }
            ledService->strip->show();
        }

        void blueOn()
        {
            for(int i=blueBeginPixel; i < ledService->strip->numPixels(); i++)
            {
                ledService->strip->setPixelColor(i,0x0000FF);
            }
            ledService->strip->show();
        }

        void blueOff()
        {
            for(int i=blueBeginPixel; i < ledService->strip->numPixels(); i++)
            {
                ledService->strip->setPixelColor(i,0x000000);
            }
            ledService->strip->show();
        }

    private:
        NeoPixelService* ledService;
};