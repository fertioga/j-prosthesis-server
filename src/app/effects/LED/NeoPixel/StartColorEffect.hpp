#include "ILedEffect.hpp"
#include "app/service/LED/NeoPixelService.hpp"
#include "TurnOffEffect.hpp"

class StartColorEffect : public ILedEffect
{
    public:

        StartColorEffect(NeoPixelService* ledService, uint32_t color) : ledService(ledService), color(color) {};
        void run() override
        {
            TurnOffEffect turnOffEffect(ledService);
            turnOffEffect.run();

            for(uint16_t i=0; i < ledService->strip->numPixels(); i++) {
                ledService->strip->setPixelColor(i, color); //  Update delay time  
                pdMS_TO_TICKS(1000); 
            }
            ledService->strip->show();  //  Update strip to match
        }

    private:
        NeoPixelService* ledService;
        uint32_t color;
};