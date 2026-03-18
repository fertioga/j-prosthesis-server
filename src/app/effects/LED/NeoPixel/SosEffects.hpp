#pragma once

#include "ILedEffect.hpp"
#include "app/service/LED/NeoPixelService.hpp"
#include "app/effects/LED/NeoPixel/TurnOffEffect.hpp"


class SosEffects : public ILedEffect
{
public:
    const int UNIT = 200;

    enum State { S, O, PAUSE };
    State state = S;

    int step = 0;
    unsigned long lastUpdate = 0;

    SosEffects(NeoPixelService* ledService)
        : ledService(ledService), turnoff(ledService) {}

    void run(uint8_t r, uint8_t g, uint8_t b) override
    {
        if (millis() - lastUpdate < UNIT)
            return;

        lastUpdate = millis();

        ledService->strip->setBrightness(255);

        switch(state)
        {
            case S:
                dot();
                step++;
                if(step >= 3)
                {
                    step = 0;
                    state = O;
                }
                
                vTaskDelay(pdMS_TO_TICKS(UNIT));
                turnoff.run(r, g, b);
                vTaskDelay(pdMS_TO_TICKS(UNIT));

                break;

            case O:
                dash();
                step++;
                if(step >= 3)
                {
                    step = 0;
                    state = S;
                }

                vTaskDelay(pdMS_TO_TICKS(UNIT * 3));
                turnoff.run(r, g, b);
                vTaskDelay(pdMS_TO_TICKS(UNIT * 3));

                break;

            case PAUSE:
                turnoff.run(r, g, b);
                state = S;
                break;
        }
    }

private:
    NeoPixelService* ledService;
    TurnOffEffect turnoff;

    void dot()
    {
        ledService->strip->fill(ledService->strip->Color(255,0,0));
        ledService->strip->show();
    }

    void dash()
    {
        ledService->strip->fill(ledService->strip->Color(255,0,0));
        ledService->strip->show();
    }
};