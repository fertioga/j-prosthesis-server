
#include "bootstrap.hpp"

#include "app/actions/ReadQueueLedBleAction.hpp"
#include "app/service/LED/NeoPixelService.hpp"

// Actions
#include "app/actions/StartingDeviceAction.hpp"

BleService ble;
NeoPixelService ledService(LED_PIN, LED_COUNT);

ReadQueueLedBleAction readLed;
StartingDeviceAction startingDeviceAction(&ledService);


void setup()
{
    HardwareMapConf();
    
    ble.begin();
    ledService.begin();

    //readLed.run();


    startingDeviceAction.run();
    
}

void loop()
{

    // startColorEffect.run();
    //  delay(1000);

    //  ledService.strip->setBrightness(100);
    //  ledService.strip->show();

    

}


