#pragma once

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "app/effects/Sound/ISound.hpp"
#include "app/configs/Mt6580Config.hpp"
#include "app/configs/HardwareMapConf.hpp"


/** INITIAL VALUES, CAN BE CHANGE WITH BLUETOOTH INTEGRATION */
#define LIFT_THRESHOLD        1.5 // Acceleration threshold to detect a lift or upward motion
#define AIR_THRESHOLD         3.0 // Acceleration threshold indicating the device is in free motion / airborne
#define IMPACT_THRESHOLD      1.0 // Acceleration threshold to detect a strong impact or collision
#define STABLE_THRESHOLD      0.6 // Maximum acceleration variation to consider the device stable

#define STEP_COOLDOWN_MS      250 // Minimum time between detected steps to avoid double counting
#define STEP_DEBOUNCE         80  // Minimum duration a state must persist to be considered valid (debounce)

#define ACE_I2C_SPEED         100000 // I2C bus speed in Hz for accelerometer communication
#define ACE_I2C_TIMEOUT       50     // I2C communication timeout in milliseconds

enum StepState {
    STABLE,
    LIFTING,
    IN_AIR
};

class Mt6580Service
{
    public:
        void begin();
        void resetI2C();
        float calcMov();
        void detectStep(float mov, ISound& callback);

    private:
        Adafruit_MPU6050 mpu;
        unsigned long lastStepTime = 0;
        unsigned long stateChangeTime = 0;
        StepState state = STABLE;

        float lift_threshold    = LIFT_THRESHOLD;
        float air_threshold     = AIR_THRESHOLD;
        float impact_threshold  = IMPACT_THRESHOLD;
        int16_t step_cooldown_ms= STEP_COOLDOWN_MS;
        int step_debounce       = STEP_DEBOUNCE;
        int ace_i2c_speed       = ACE_I2C_SPEED;
        int ace_i2c_timeout     = ACE_I2C_TIMEOUT;
};