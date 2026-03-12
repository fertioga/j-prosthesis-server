#include "HardwareMapConf.hpp"

#define SERIAL_BAUD_RATE 115200

#define LED_PIN   16
#define LED_COUNT 8

void HardwareMapConf()
{
    Serial.begin(SERIAL_BAUD_RATE);

} 