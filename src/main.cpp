
#include "bootstrap.hpp"
#include "app/configs/HardwareMapConf.hpp"

#include "app/actions/ReadQueueLedBleAction.hpp"
#include "app/service/LED/NeoPixelService.hpp"

// Actions
#include "app/actions/StartingDeviceAction.hpp"
#include "app/actions/SoundMoveRoboAction.hpp"

#include "app/service/position/accelerometer/MT6580/Mt6580Service.hpp"
#include "app/effects/Sound/RoboServoEffect.hpp"

#include "app/service/fileSystem/ArduinoJson/ArduinoJsonService.hpp"

ArduinoJsonService<2048> config;

BleService ble;
NeoPixelService ledService(LED_PIN, LED_COUNT);

ReadQueueLedBleAction readLedAction(&ledService);
StartingDeviceAction startingDeviceAction(&ledService);

Mt6580Service mt6580Service;
RoboServoEffect roboServoEffect;

SoundMoveRoboAction soundMoveRoboAction(&mt6580Service, &roboServoEffect);



void setup()
{
    HardwareMapConf();
    
    ble.begin();
    ledService.begin();

    mt6580Service.begin();
    
    startingDeviceAction.run();

    readLedAction.run();

    soundMoveRoboAction.run();

    //---

    config.begin("/config.json");

    // // Salvar dados
    // config.setString("wifi_ssid", "MinhaRede");
    // config.setString("wifi_pass", "12345678");
    // config.setInt("sensor_interval", 30);
    // config.setBool("ble_enabled", true);

    // // Recuperar dados
    // Serial.println(config.getString("wifi_ssid"));
    // Serial.println(config.getString("wifi_pass"));
    // Serial.println(config.getInt("sensor_interval"));
    // Serial.println(config.getBool("ble_enabled"));

    // config.set<int>("sensor.interval", 30);
    // config.set<bool>("ble.enabled", true);

    //int interval = config.get<int>("sensor.interval", 10);

    // Serial.print(" WIFI: ");
    Serial.println(config.getString("wifi.ssid",""));

    config.setString("wifi.ssid", "test 22 ");
    
    Serial.println(config.getString("wifi.ssid",""));

    config.commit();

    Serial.println(config.getString("wifi.ssid",""));

    //--

   

    
    // listFiles();
    // printFile("/config.json");

    Serial.println("");
    delay(100);

}

void loop()
{   
    
    // sensors_event_t a, g, temp;
    // mpu.getEvent(&a, &g, &temp);

    // float ax = a.acceleration.x;
    // float ay = a.acceleration.y;
    // float az = a.acceleration.z;

    // float accelMag = sqrt(ax*ax + ay*ay + az*az);

    // // filtro
    // static float filtered = 0;
    // filtered = 0.9 * filtered + 0.1 * accelMag;

    // // detecção
    // detectStep(filtered);

    //if(mpu.getMotionInterruptStatus()) {
    /* Get new sensor events with the readings */
    // sensors_event_t a, g, temp;
    // mpu.getEvent(&a, &g, &temp);

    // ax = a.acceleration.x;
    // ay = a.acceleration.y;
    // az = a.acceleration.z;
    // accelMag = sqrt(ax*ax + ay*ay + az*az);

    // filtered = 0.8 * filtered + 0.2 * accelMag;

    // dynamicAccel = abs(filtered - 9.8);

    // if(dynamicAccel > 3.0) {
    //     if(state != 1){
    //         Serial.println("IMPACTO");
    //         state = 1;
    //     }
    // }

    // if(dynamicAccel < 0.5) {
    //     if(state != 0){

    //         Serial.println("ESTÁVEL");
    //         state = 0;
    //     }
    // }

    // Serial.print(" Magnitude Aceleracao: ");
    // Serial.println(accelMag);

    // Serial.print(" Magnitude Filtrada: ");
    // Serial.println(filtered);

    // if(filtered > 15.0) {
    //     Serial.println("PÉ NO CHÃO");
    // }

    // if(filtered < 9.0) {
    //     Serial.println("PERNA NO AR");
    // }

    /* Print out the values */
    // Serial.print("AccelX:");
    // Serial.print(a.acceleration.x);
    // Serial.print(",");
    // Serial.print("AccelY:");
    // Serial.print(a.acceleration.y);
    // Serial.print(",");
    // Serial.print("AccelZ: ");
    // Serial.print(a.acceleration.z);
    // Serial.print(" ");
    // if(a.acceleration.z > 15.0)
    // {
    //     Serial.println("Levantou o pé.");
    // }
    // Serial.print(", ");
    // Serial.print("GyroX:");
    // Serial.print(g.gyro.x);
    // Serial.print(",");
    // Serial.print("GyroY:");
    // Serial.print(g.gyro.y);
    // Serial.print(",");
    // Serial.print(" GyroZ:");
    // Serial.print(g.gyro.z);
    // Serial.println("");
  //}

  delay(10);
}


