#include "app/actions/PublishBatteryLevelAction.hpp"

PublishBatteryLevelAction::PublishBatteryLevelAction(
    BleService* ble,
    BatteryLevelService* battery)
    : taskService()
    , ble(ble)
    , battery(battery)
{
}

void PublishBatteryLevelAction::task(void* pvParameters)
{
    (void)pvParameters;

    while (true)
    {
#ifdef BATTERY_LEVEL_INJECT
        pollInject();
#endif
        const BatteryLevel level = battery->sample();

        if (level.valid)
        {
            sawValid = true;
            lastValidMs = level.sampledAtMs;

            if (!announced || announcedPercent != level.percent)
            {
                ble->applyAdvertisedBattery(level.percent);
                announced = true;
                announcedPercent = level.percent;
            }
        }
        else if (sawValid && announced &&
                 (millis() - lastValidMs >= PUBLISH_BATTERY_STALE_MS))
        {
            ble->clearAdvertisedBattery();
            announced = false;
        }

        vTaskDelay(pdMS_TO_TICKS(PUBLISH_BATTERY_INTERVAL_MS));
    }
}

void PublishBatteryLevelAction::run()
{
    taskService.create(
        IAction::entry,
        PUBLISH_BATTERY_TASK_NAME,
        PUBLISH_BATTERY_TASK_STACK,
        this,
        PUBLISH_BATTERY_TASK_PRIORITY,
        nullptr,
        PUBLISH_BATTERY_TASK_CORE
    );
}

#ifdef BATTERY_LEVEL_INJECT

void PublishBatteryLevelAction::pollInject()
{
    while (Serial.available() > 0)
    {
        const char c = static_cast<char>(Serial.read());
        if (c == '\n' || c == '\r')
        {
            if (injectLine.length() > 0)
            {
                handleInjectLine(injectLine);
                injectLine = "";
            }
        }
        else if (injectLine.length() < 16)
        {
            injectLine += c;
        }
    }
}

void PublishBatteryLevelAction::handleInjectLine(const String& line)
{
    String text = line;
    text.trim();
    if (!text.startsWith("BAT"))
    {
        return;
    }

    String arg = text.substring(3);
    arg.trim();

    if (arg.equalsIgnoreCase("OFF"))
    {
        battery->clearInject();
        Serial.println("Battery inject off");
        return;
    }

    if (arg.length() == 0)
    {
        return;
    }

    for (unsigned int i = 0; i < arg.length(); i++)
    {
        if (!isDigit(arg.charAt(i)))
        {
            return;
        }
    }

    const long value = arg.toInt();
    if (value < 0 || value > 100)
    {
        battery->clearInject();
        Serial.println("Battery inject off");
        return;
    }

    battery->inject(static_cast<uint8_t>(value));
    Serial.printf("Battery inject %u\n", static_cast<unsigned>(value));
}

#endif
