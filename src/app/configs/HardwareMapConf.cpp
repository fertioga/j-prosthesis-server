#include "HardwareMapConf.hpp"

#define SERIAL_BAUD_RATE 115200

#define LED_PIN 14
#define CHANNEL_LED 0
#define FREQ_LED 5000
#define RESOLUTION_LED 64

void HardwareMapConf()
{
    Serial.begin(SERIAL_BAUD_RATE);

    //pinMode(LED_PIN, OUTPUT);
    //digitalWrite(LED_PIN, LOW);
    //ledcSetup(CHANNEL_LED, FREQ_LED, RESOLUTION_LED); 
    //ledcAttachPin(LED_PIN, CHANNEL_LED); 
} 