#pragma once
#include "ILedEffect.hpp"
#include "app/service/LED/NeoPixelService.hpp"
#include "TurnOffEffect.hpp"

class FadeOutEffect : public ILedEffect
{
    public:

        FadeOutEffect(NeoPixelService* ledService, uint16_t delayTime) : 
            ledService(ledService), 
            delayTime(delayTime),
            turnOff(ledService) 
        {};

        void run() override
        {
            int originalBrightness = ledService->strip->getBrightness();
            for (int brightness = ledService->strip->getBrightness(); brightness >= 0; brightness--) {
                ledService->strip->setBrightness(brightness);
                ledService->strip->show();
                vTaskDelay(delayTime / portTICK_PERIOD_MS);
            }

            turnOff.run();
            ledService->strip->setBrightness(originalBrightness);

        }

    private:
        NeoPixelService* ledService;
        uint16_t delayTime;
        TurnOffEffect turnOff;
};