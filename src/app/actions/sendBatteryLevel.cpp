
// #include "bootstrap.hpp"

// void sendBatterryLevel(BleServer& ble)
// {
//     static unsigned long lastTelemetry = 0;

//     if (millis() - lastTelemetry > 5000)
//     {
//         lastTelemetry = millis();

//         string telemetry = "BAT:87";   // Exemplo fixo
//         ble.notifyTelemetry(telemetry.c_str());

//         Serial.println("Telemetria enviada.");
//     }
// }