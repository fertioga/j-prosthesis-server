#include "ILedEffect.hpp"
#include "app/service/LED/NeoPixelService.hpp"

class TurnOffEffect : public ILedEffect
{
    public:

        TurnOffEffect(NeoPixelService* ledService) : ledService(ledService) {};
        void run() override
        {
            for(uint16_t i=0; i < ledService->strip->numPixels(); i++) {
                ledService->strip->setPixelColor(i, ledService->strip->Color(0, 0, 0)); // Turn off LED
            }
            ledService->strip->show();  // Update strip to match
        }

    private:
        NeoPixelService* ledService;
};