#include "bootstrap.hpp"
#include "app/configs/HardwareMapConf.hpp"

#include "app/actions/ReadQueueLedBleAction.hpp"
#include "app/actions/PublishBatteryLevelAction.hpp"
#include "app/service/LED/NeoPixelService.hpp"
#include "app/service/power/battery/BatteryLevelService.hpp"

// Actions
#include "app/actions/StartingDeviceAction.hpp"
// #include "app/actions/SoundMoveRoboAction.hpp"

// #include "app/service/position/accelerometer/MT6580/Mt6580Service.hpp"
// #include "app/effects/Sound/RoboServoEffect.hpp"

// #include "app/service/fileSystem/ArduinoJson/ArduinoJsonService.hpp"

// ArduinoJsonService<2048> config("/config.json");

BleService ble;
NeoPixelService ledService(LED_PIN, LED_COUNT);

ReadQueueLedBleAction readLedAction(&ledService);
StartingDeviceAction startingDeviceAction(&ledService);

BatteryLevelService batteryLevel;
PublishBatteryLevelAction publishBatteryAction(&ble, &batteryLevel);

// Mt6580Service mt6580Service(config);
// RoboServoEffect roboServoEffect;

// SoundMoveRoboAction soundMoveRoboAction(&mt6580Service, &roboServoEffect);

void setup()
{

    HardwareMapConf();

    ble.begin();
    ledService.begin();
    batteryLevel.begin();
    publishBatteryAction.run();

    // mt6580Service.begin();
    
    startingDeviceAction.run();

    readLedAction.run();

    // soundMoveRoboAction.run();

   
    // config.listFiles();
    // config.printFile("/config.json");

    // Serial.print(" - accelerometer: ");
    // Serial.println(config.getFloat("accelerometer.impact_threshold",0.0));
    // delay(100);

}

void loop()
{
    vTaskDelete(NULL);
}


