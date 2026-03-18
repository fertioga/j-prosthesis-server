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

        void run(uint8_t r, uint8_t g, uint8_t b) override
        {
            int originalBrightness = ledService->strip->getBrightness();
            for (int brightness = ledService->strip->getBrightness(); brightness >= 0; brightness--) {
                ledService->strip->setBrightness(brightness);
                ledService->strip->show();
                vTaskDelay(delayTime / portTICK_PERIOD_MS);
            }

            turnOff.run(r, g, b);
            ledService->strip->setBrightness(originalBrightness);

        }

    private:
        NeoPixelService* ledService;
        uint16_t delayTime;
        TurnOffEffect turnOff;
};