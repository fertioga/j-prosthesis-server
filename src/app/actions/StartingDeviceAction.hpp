#include "IAction.hpp"
#include "app/service/LED/NeoPixelService.hpp"
#include "app/effects/LED/NeoPixel/StartColorEffect.hpp"
#include "app/effects/LED/NeoPixel/FadeOutEffects.hpp"

class StartingDeviceAction : public IAction
{
    public:

        StartingDeviceAction(NeoPixelService* ledService) : ledService(ledService) {}
        void run() override
        {
            Serial.println("Starting device...");
            turnOnLeds();
            fadeOutLeds();
        }

    private:
        NeoPixelService* ledService;

        void turnOnLeds()
        {
            StartColorEffect startColorEffect(ledService, 0xFFFFFF); // White colors
            startColorEffect.run();
        }

        void fadeOutLeds()
        {
            FadeOutEffect fadeOutEffect(ledService, FADE_OUT_DELAY); //  Update delay time
            fadeOutEffect.run();
        }
};