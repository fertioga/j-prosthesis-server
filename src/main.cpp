
#include "bootstrap.hpp"
#include "app/configs/HardwareMapConf.hpp"

#include "app/actions/ReadQueueLedBleAction.hpp"
#include "app/service/LED/NeoPixelService.hpp"

// Actions
#include "app/actions/StartingDeviceAction.hpp"


BleService ble;
NeoPixelService ledService(LED_PIN, LED_COUNT);

ReadQueueLedBleAction readLedAction(&ledService);
StartingDeviceAction startingDeviceAction(&ledService);

void setup()
{
    HardwareMapConf();
    
    ble.begin();
    ledService.begin();
    
    startingDeviceAction.run();

    readLedAction.run();
}

void loop()
{   
    /* loop */
}


