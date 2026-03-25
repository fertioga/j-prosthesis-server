#pragma once

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "app/effects/Sound/ISound.hpp"
#include "app/configs/HardwareMapConf.hpp"
#include "app/service/fileSystem/IFileSystemConfig.hpp"


/** INITIAL VALUES, CAN BE CHANGE WITH BLUETOOTH INTEGRATION */
// #define LIFT_THRESHOLD        1.5 // Acceleration threshold to detect a lift or upward motion
// #define AIR_THRESHOLD         3.0 // Acceleration threshold indicating the device is in free motion / airborne
// #define IMPACT_THRESHOLD      1.0 // Acceleration threshold to detect a strong impact or collision
// #define STABLE_THRESHOLD      0.6 // Maximum acceleration variation to consider the device stable

// #define STEP_COOLDOWN_MS      250 // Minimum time between detected steps to avoid double counting
// #define STEP_DEBOUNCE         80  // Minimum duration a state must persist to be considered valid (debounce)

// #define ACE_I2C_SPEED         100000 // I2C bus speed in Hz for accelerometer communication
// #define ACE_I2C_TIMEOUT       50     // I2C communication timeout in milliseconds

enum StepState {
    STABLE,
    LIFTING,
    IN_AIR
};

class Mt6580Service
{
    public:
        Mt6580Service(IFileSystemConfig& config): config(config)
        {
            /* Get values from data/config.json or apply default values*/ 
            lift_threshold   = config.getFloat("accelerometer.lift_threshold",1.5);
            air_threshold    = config.getFloat("accelerometer.air_threshold",3.0);
            impact_threshold = config.getFloat("accelerometer.impact_threshold",1.0);
            stable_threshold = config.getFloat("accelerometer.stable_threshold",0.6);
            step_cooldown_ms = config.getInt("accelerometer.step_cooldown_ms", 250);
            step_debounce    = config.getInt("accelerometer.step_debounce", 80);
            ace_i2c_speed    = config.getInt("accelerometer.ace_i2c_speed", 100000);
            ace_i2c_timeout  = config.getInt("accelerometer.ace_i2c_timeout", 50);
        }
        void begin();
        void resetI2C();
        float calcMov();
        void detectStep(float mov, ISound& callback);

    private:
        Adafruit_MPU6050 mpu;
        unsigned long lastStepTime = 0;
        unsigned long stateChangeTime = 0;
        StepState state = STABLE;

        IFileSystemConfig& config;
        float lift_threshold;
        float air_threshold;
        float impact_threshold;
        float stable_threshold;
        int16_t step_cooldown_ms;
        int step_debounce;
        int ace_i2c_speed;
        int ace_i2c_timeout;
};