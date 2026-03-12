#pragma once
#include "ILedEffect.hpp"
#include "app/service/LED/NeoPixelService.hpp"

class FadeOutEffect : public ILedEffect
{
    public:

        FadeOutEffect(NeoPixelService* ledService, uint16_t delayTime) : ledService(ledService), delayTime(delayTime) {};
        void run() override
        {
            for (int brightness = ledService->strip->getBrightness(); brightness >= 0; brightness--) {
                ledService->strip->setBrightness(brightness);
                ledService->strip->show();
                vTaskDelay(delayTime / portTICK_PERIOD_MS);
            }
        }

    private:
        NeoPixelService* ledService;
        uint16_t delayTime;
};