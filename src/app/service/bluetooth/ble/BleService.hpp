#pragma once

#include "bootstrap.hpp"
#include <NimBLEDevice.h>

class BleService
{
public:
    void begin();
    void notifyBattery(const std::string& message);

private:

    QueueService queueService;
    NimBLECharacteristic* ledCharacteristic{nullptr};
    NimBLECharacteristic* batteryCharacteristic{nullptr};

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
        void onConnect(NimBLEServer* server,
                       NimBLEConnInfo& connInfo) override;

        void onDisconnect(NimBLEServer* server,
                          NimBLEConnInfo& connInfo,
                          int reason) override;
    };
};