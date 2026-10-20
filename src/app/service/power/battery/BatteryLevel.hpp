#pragma once

#include <cstdint>

struct BatteryLevel
{
    bool valid{false};
    uint8_t percent{0};
    uint32_t sampledAtMs{0};

    static BatteryLevel unavailable()
    {
        return BatteryLevel{};
    }

    static BatteryLevel fromRaw(float raw, uint32_t nowMs)
    {
        if (raw < 0.0f || raw > 100.0f)
        {
            return unavailable();
        }

        const int rounded = static_cast<int>(raw + 0.5f);
        if (rounded < 0 || rounded > 100)
        {
            return unavailable();
        }

        BatteryLevel level;
        level.valid = true;
        level.percent = static_cast<uint8_t>(rounded);
        level.sampledAtMs = nowMs;
        return level;
    }
};
