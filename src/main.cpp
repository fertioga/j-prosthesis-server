
#include "bootstrap.hpp"

#include "app/actions/readQueueLedBleAction.hpp"

BleService ble;

ReadQueueLedBleAction readLed;

void setup()
{
    HardwareMapConf();
    
    ble.begin();

    readLed.run();
    
}

void loop()
{
    // delete this task
    vTaskDelete(nullptr);
}