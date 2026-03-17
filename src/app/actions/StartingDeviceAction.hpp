#pragma once

#include "IAction.hpp"
#include "app/service/LED/NeoPixelService.hpp"
#include "app/effects/LED/NeoPixel/StartColorEffect.hpp"
#include "app/effects/LED/NeoPixel/FadeOutEffects.hpp"

class StartingDeviceAction : public IAction
{
    public:

        StartingDeviceAction(NeoPixelService* ledService) : ledService(ledService) {}

        void task(void* pvParameters) override
        {
            // dont used
        }

        void run() override
        {
            Serial.println("Starting device...");

            StartColorEffect startColorEffect(ledService, 0xFFFFFF); // White colors
            startColorEffect.run();

            FadeOutEffect fadeOutEffect(ledService, FADE_OUT_DELAY); //  Update delay time
            fadeOutEffect.run();
        }
    
    private:
        NeoPixelService* ledService;
};