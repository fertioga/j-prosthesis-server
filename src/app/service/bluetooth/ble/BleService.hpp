#pragma once

#include "bootstrap.hpp"
#include <NimBLEDevice.h>
#include "app/structs/LedPayloadBle.hpp"

class BleService
{
public:
    void begin();
    void notifyBattery(const std::string& message);
    void applyAdvertisedBattery(uint8_t percent);
    void clearAdvertisedBattery();

private:

    QueueService queueService;
    NimBLECharacteristic* ledCharacteristic{nullptr};
    NimBLECharacteristic* batteryCharacteristic{nullptr};
    bool levelInAdvertisement{false};
    uint8_t advertisedPercent{0};

    void rebuildAdvertising(bool includeLevel, uint8_t percent);
    void resumeAdvertising();
    void restoreAdvertising();

    class LedCallbacks : public NimBLECharacteristicCallbacks
    {
    public:
        LedCallbacks(BleService* service) : bleService(service) {}

        void onWrite(NimBLECharacteristic* characteristic,
                     NimBLEConnInfo& connInfo) override;

    private:
        BleService* bleService;
    };

    class ServerCallbacks : public NimBLEServerCallbacks
    {
    public:
        explicit ServerCallbacks(BleService* service) : bleService(service) {}

        void onConnect(NimBLEServer* server,
                       NimBLEConnInfo& connInfo) override;

        void onDisconnect(NimBLEServer* server,
                          NimBLEConnInfo& connInfo,
                          int reason) override;

    private:
        BleService* bleService;
    };
};