#include "app/service/bluetooth/ble/BleService.hpp"


/* ================================================================ */

void BleService::begin()
{
    NimBLEDevice::init(BLE_CONF_DEVICE_NAME);

    /* ================= SECURITY ================= */

    NimBLEDevice::setSecurityAuth(
        BLE_CONF_BONDING,
        BLE_CONF_MITM,
        BLE_CONF_SECURE
    );

    NimBLEDevice::setSecurityIOCap(BLE_CONF_SET_SECURITY_IO_CAPS);
    NimBLEDevice::setSecurityPasskey(BLE_CONF_PASSKEY);

    /* ================= SERVER ================= */
    NimBLEServer* server = NimBLEDevice::createServer();
    server->setCallbacks(new ServerCallbacks());

        /* MAIN SERVICE */
        NimBLEService* mainService =
            server->createService(BLE_CONF_MAIN_SERVICE_UUID);

        /* Characteristics */
        /* LED */
        ledCharacteristic = mainService->createCharacteristic(
            BLE_CONF_LED_UUID,
            NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_ENC
        );

        ledCharacteristic->setCallbacks(new LedCallbacks(this));

        mainService->start();

    /* ================= BATTERY SERVICE ================= */
        NimBLEService* batteryService =
            server->createService((uint16_t)BLE_CONF_BATTERY_SERVICE_UUID);

        /* Characteristic */
        /* Battery */
        batteryCharacteristic = batteryService->createCharacteristic(
            (uint16_t)BLE_CONF_BATTERY_LEVEL_UUID,
            NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::NOTIFY
        );

        batteryService->start();

    /* ================= ADVERTISING ================= */
    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();

    NimBLEAdvertisementData advData;
    advData.setName(BLE_CONF_DEVICE_NAME);
    advData.setCompleteServices(NimBLEUUID(BLE_CONF_MAIN_SERVICE_UUID));
    advertising->setAdvertisementData(advData);

    advertising->addServiceUUID(BLE_CONF_MAIN_SERVICE_UUID);
    advertising->addServiceUUID((uint16_t)BLE_CONF_BATTERY_SERVICE_UUID);

    advertising->start();

    Serial.println("Iniciando advertising...");
    Serial.println(NimBLEDevice::getAddress().toString().c_str());
}

/* ================================================================ */

void BleService::notifyBattery(const std::string& message)
{
    if (!batteryCharacteristic) return;

    batteryCharacteristic->setValue(message);
    batteryCharacteristic->notify();
}

/* ================= LED CALLBACK ================= */
void BleService::LedCallbacks::onWrite(
    NimBLECharacteristic* characteristic,
    NimBLEConnInfo& connInfo)
{
    if (!connInfo.isEncrypted())
    {
        Serial.println("Write bloqueado: conexão não criptografada.");
        return;
    }

    //std::string value = characteristic->getValue();


    // uint8_t brightness = static_cast<uint64_t>(value[0]);
    //     Serial.print("BLE SERVER: Comando recebido. Brilho: ");
    //     Serial.println(brightness);

    LedPayloadBle payload;
    if (sizeof(payload) != characteristic->getValue().size())
    {
        Serial.println("Invalid payload size. Expected: " + String(sizeof(LedPayloadBle)) + " bytes, Received: " + String(characteristic->getValue().size()) + " bytes.");
        return;
    }

    memcpy(&payload, characteristic->getValue(), sizeof(LedPayloadBle)); // copy the data into the struct

    /** Send to queue */
    this->bleService->queueService.send(QueuesConfig::instance().ledQueue, payload);
}

/* ================= SERVER CALLBACK ================= */

void BleService::ServerCallbacks::onConnect(
    NimBLEServer* server,
    NimBLEConnInfo& connInfo)
{
    Serial.println("Cliente conectado.");
}

void BleService::ServerCallbacks::onDisconnect(
    NimBLEServer* server,
    NimBLEConnInfo& connInfo,
    int reason)
{
    Serial.print("Cliente desconectado. Motivo: ");
    Serial.println(reason);

    NimBLEDevice::startAdvertising();
}
