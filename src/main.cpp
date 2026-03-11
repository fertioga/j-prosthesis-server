
#include "bootstrap.hpp"

#include "app/actions/ReadQueueLedBleAction.hpp"
#include "app/service/LED/NeoPixelService.hpp"
#include "app/effects/LED/NeoPixel/TheaterChaseRainbowEffect.hpp"
#include "app/effects/LED/NeoPixel/TheaterChaseEffect.hpp"
#include "app/effects/LED/NeoPixel/ColorWipeEffect.hpp"
#include "app/effects/LED/NeoPixel/StartColorEffect.hpp"

BleService ble;

ReadQueueLedBleAction readLed;

#define LED_PIN   16
#define LED_COUNT 5

NeoPixelService ledService(LED_PIN, LED_COUNT);

StartColorEffect startColorEffect(&ledService, ledService.strip->Color(0, 0, 255)); // Blue

// TheaterChaseRainbowEffect theaterChaseRainbowEffect(&ledService, 50);

// TheaterChaseRainbowEffect theaterChaseEffect(&ledService, 50);

// ColorWipeEffect colorWipeEffect(&ledService, 50, ledService.strip->Color(255, 255, 255)); // Red

//RainbowEffect rainbowEffect(&ledService, 50);





void setup()
{
    HardwareMapConf();
    
    ble.begin();

    //readLed.run();

    ledService.begin();

    startColorEffect.run();
    
}

void loop()
{

    // colorWipeEffect.run();
    // delay(1000);

}


