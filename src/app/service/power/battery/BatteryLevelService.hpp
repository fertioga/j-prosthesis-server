#pragma once

#include "app/service/power/battery/BatteryLevel.hpp"
#include <Arduino.h>

class BatteryLevelService
{
public:
    void begin();
    BatteryLevel sample();

#ifdef BATTERY_LEVEL_INJECT
    void inject(uint8_t percent)
    {
        if (percent > 100)
        {
            injected = false;
            return;
        }

        injected = true;
        injectedPercent = percent;
    }

    void clearInject()
    {
        injected = false;
    }

private:
    bool injected{false};
    uint8_t injectedPercent{0};
#endif
};
