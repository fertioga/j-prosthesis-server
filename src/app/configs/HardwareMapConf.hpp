
#pragma once
#include "bootstrap.hpp"

#define SERIAL_BAUD_RATE 115200

/* LEDS */
#define LED_PIN   16
#define LED_COUNT 8

/** ACELEROMETER */
#define ACCE_SDA_PORT 21
#define ACCE_SCL_PORT 22

/** Bateria: divisor 1:1 no GPIO 34 (ADC1) */
#define BATTERY_ADC_PIN 34

void HardwareMapConf();


