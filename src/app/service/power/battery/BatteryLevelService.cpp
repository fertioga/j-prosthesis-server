#include "app/service/power/battery/BatteryLevelService.hpp"
#include "app/configs/HardwareMapConf.hpp"

namespace
{
constexpr int kSamples = 8;
constexpr float kAdcFullScale = 4095.0f;
constexpr float kAdcVref = 3.3f;
constexpr float kDividerRatio = 2.0f;
constexpr float kCellEmptyVolts = 3.2f;
constexpr float kCellFullVolts = 4.2f;
constexpr float kPinMinVolts = 1.30f;
constexpr float kPinMaxVolts = 2.30f;
}

void BatteryLevelService::begin()
{
    analogReadResolution(12);
    analogSetPinAttenuation(BATTERY_ADC_PIN, ADC_11db);
}

BatteryLevel BatteryLevelService::sample()
{
#ifdef BATTERY_LEVEL_INJECT
    if (injected)
    {
        return BatteryLevel::fromRaw(static_cast<float>(injectedPercent), millis());
    }
#endif

    (void)analogRead(BATTERY_ADC_PIN);

    long sum = 0;
    for (int i = 0; i < kSamples; i++)
    {
        sum += analogRead(BATTERY_ADC_PIN);
        delay(2);
    }

    const float pinVolts = (sum / static_cast<float>(kSamples)) * kAdcVref / kAdcFullScale;
    if (pinVolts < kPinMinVolts || pinVolts > kPinMaxVolts)
    {
        return BatteryLevel::unavailable();
    }

    const float cellVolts = pinVolts * kDividerRatio;
    float percent = ((cellVolts - kCellEmptyVolts) / (kCellFullVolts - kCellEmptyVolts)) * 100.0f;
    if (percent < 0.0f)
    {
        percent = 0.0f;
    }
    else if (percent > 100.0f)
    {
        percent = 100.0f;
    }

    return BatteryLevel::fromRaw(percent, millis());
}
