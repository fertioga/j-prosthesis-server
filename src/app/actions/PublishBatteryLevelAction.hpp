#pragma once

#include "app/actions/IAction.hpp"
#include "app/service/bluetooth/ble/BleService.hpp"
#include "app/service/power/battery/BatteryLevelService.hpp"

#define PUBLISH_BATTERY_TASK_NAME "BatteryAdv"
#define PUBLISH_BATTERY_TASK_STACK 4096
#define PUBLISH_BATTERY_TASK_PRIORITY 1
#define PUBLISH_BATTERY_TASK_CORE 1
#define PUBLISH_BATTERY_INTERVAL_MS 10000
#define PUBLISH_BATTERY_STALE_MS 120000

class PublishBatteryLevelAction : public IAction
{
public:
    PublishBatteryLevelAction(BleService* ble, BatteryLevelService* battery);

    void task(void* pvParameters) override;
    void run() override;

private:
    TaskService taskService;
    BleService* ble;
    BatteryLevelService* battery;

    bool sawValid{false};
    bool announced{false};
    uint8_t announcedPercent{0};
    uint32_t lastValidMs{0};

#ifdef BATTERY_LEVEL_INJECT
    String injectLine;
    void pollInject();
    void handleInjectLine(const String& line);
#endif
};
