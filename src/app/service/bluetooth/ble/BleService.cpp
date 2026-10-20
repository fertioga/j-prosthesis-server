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
    server->setCallbacks(new ServerCallbacks(this));

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
    clearAdvertisedBattery();
    resumeAdvertising();

    Serial.println("Iniciando advertising...");
    Serial.println(NimBLEDevice::getAddress().toString().c_str());
}

void BleService::applyAdvertisedBattery(uint8_t percent)
{
    if (percent > 100)
    {
        return;
    }

    rebuildAdvertising(true, percent);
    if (NimBLEDevice::getAdvertising()->isAdvertising())
    {
        NimBLEDevice::getAdvertising()->refreshAdvertisingData();
    }
}

void BleService::clearAdvertisedBattery()
{
    rebuildAdvertising(false, 0);
    if (NimBLEDevice::getAdvertising()->isAdvertising())
    {
        NimBLEDevice::getAdvertising()->refreshAdvertisingData();
    }
}

void BleService::rebuildAdvertising(bool includeLevel, uint8_t percent)
{
    levelInAdvertisement = includeLevel && percent <= 100;
    if (levelInAdvertisement)
    {
        advertisedPercent = percent;
    }

    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
    const NimBLEUUID batteryUuid((uint16_t)BLE_CONF_BATTERY_SERVICE_UUID);

    advertising->clearData();
    advertising->enableScanResponse(false);

    if (!advertising->setName(BLE_CONF_DEVICE_NAME))
    {
        Serial.println("Anuncio sem espaco para o nome.");
        return;
    }

    advertising->addServiceUUID(batteryUuid);

    if (levelInAdvertisement)
    {
        const uint8_t level = advertisedPercent;
        if (!advertising->setServiceData(batteryUuid, &level, 1))
        {
            Serial.println("Anuncio sem espaco para o percentual.");
            levelInAdvertisement = false;
        }
    }

    advertising->enableScanResponse(true);
    if (!advertising->addServiceUUID(BLE_CONF_MAIN_SERVICE_UUID))
    {
        Serial.println("Scan response sem UUID de comando.");
    }
}

void BleService::resumeAdvertising()
{
    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
    if (!advertising->isAdvertising())
    {
        advertising->refreshAdvertisingData();
    }

    if (!advertising->start())
    {
        Serial.println("Anuncio durante a sessao recusado.");
    }
}

void BleService::restoreAdvertising()
{
    if (levelInAdvertisement)
    {
        applyAdvertisedBattery(advertisedPercent);
    }
    else
    {
        clearAdvertisedBattery();
    }

    resumeAdvertising();
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
    (void)server;
    (void)connInfo;
    Serial.println("Cliente conectado.");
    bleService->resumeAdvertising();
}

void BleService::ServerCallbacks::onDisconnect(
    NimBLEServer* server,
    NimBLEConnInfo& connInfo,
    int reason)
{
    (void)server;
    (void)connInfo;
    Serial.print("Cliente desconectado. Motivo: ");
    Serial.println(reason);

    bleService->restoreAdvertising();
}
