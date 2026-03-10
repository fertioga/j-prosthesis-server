#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

struct QueuesConfig
{
    // -----------------------------
    // Queues
    // -----------------------------
    QueueHandle_t ledQueue;
    // New queues can be added here

private:
    // Private constructor to singleton
    QueuesConfig()
    {
        // Filas
        ledQueue    = xQueueCreate(5, sizeof(uint8_t));
        // New queues can be initialized here
    }

public:
    // Access global instance
    static QueuesConfig& instance()
    {
        static QueuesConfig _instance;
        return _instance;
    }

    // Delete copy constructor and assignment operator
    QueuesConfig(const QueuesConfig&) = delete;
    void operator=(const QueuesConfig&) = delete;
};
