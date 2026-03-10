#include "HardwareMapConf.hpp"

void HardwareMapConf()
{
    Serial.begin(115200);
    pinMode(2, OUTPUT);
    ledcSetup(0, 5000, 8);   // canal 0, 5kHz, 8 bits
    ledcAttachPin(2, 0);     // GPIO 2 no canal 0
} 