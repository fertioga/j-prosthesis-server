#include "Mt6580Service.hpp"

void Mt6580Service::begin()
{
    resetI2C();

    Wire.begin(ACCE_SDA_PORT, ACCE_SCL_PORT);
    Wire.setClock(ace_i2c_speed); 
    Wire.setTimeOut(ace_i2c_timeout);

    if (!mpu.begin()) {
        Serial.println("Failed to find MPU6050 chip");
        ESP.restart();
    }
    
    Serial.println("MPU6050 Found!");


    /** Internal Clock */ 
    mpu.setClock(MPU6050_PLL_GYROX);

    /** Internl Filter */
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    /** Sensitivity */
    mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
}

void Mt6580Service::resetI2C() {
    pinMode(ACCE_SCL_PORT, OUTPUT);
    pinMode(ACCE_SDA_PORT, INPUT_PULLUP);

    for (int i = 0; i < 9; i++) {
        digitalWrite(ACCE_SCL_PORT, HIGH);
        delayMicroseconds(5);
        digitalWrite(ACCE_SCL_PORT, LOW);
        delayMicroseconds(5);
    }
}

float Mt6580Service::calcMov()
{
    sensors_event_t a, g, temp;
    // filter
    static float acce_mov_filtered = 0;

    mpu.getEvent(&a, &g, &temp);

    delay(1);

    float ax = a.acceleration.x;
    float ay = a.acceleration.y;
    float az = a.acceleration.z;

    float accelMag = sqrt(ax*ax + ay*ay + az*az);

    acce_mov_filtered = 0.9 * acce_mov_filtered + 0.1 * accelMag;

    return acce_mov_filtered;
}

void Mt6580Service::detectStep(float mov, ISound& callback)
{
    float dynamicAccel = fabs(mov - 9.8); /** onle positive values */
    
    unsigned long now = millis();

    switch(state) {

        case STABLE:
            if(dynamicAccel > lift_threshold) {
                state = LIFTING;
                stateChangeTime = now;
                /** do nothing */
            }
        break;

        case LIFTING:
            // evita pular direto
            if(now - stateChangeTime < step_debounce) break;

            if(dynamicAccel > air_threshold) {
                state = IN_AIR;
                stateChangeTime = now;
            }
            else if(dynamicAccel < step_debounce) {
                state = STABLE;
            }
        break;

        case IN_AIR:
            Serial.print("IN_AIR: mov - grav: ");
            Serial.println(dynamicAccel);
            if(now - stateChangeTime < step_debounce) break;

            if(dynamicAccel > impact_threshold &&
               (now - lastStepTime > step_cooldown_ms)) {

                state = STABLE;
                lastStepTime = now;
                stateChangeTime = now;
                
                Serial.print("--------PISOU-----: mov - grav: ");
                Serial.print(dynamicAccel);
                Serial.print(" IMPACT_THRESHOLD ");
                Serial.println(impact_threshold);

                Serial.print(" Valor config: ");
                Serial.println(config.getFloat("accelerometer.impact_threshold",0.0));
                /***********************/
                /** CALL SOUND METHOD **/
                /***********************/
                callback.run();
            }
        break;
    }
}
