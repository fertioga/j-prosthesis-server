#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define QUEUE_SIZE 5
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
        ledQueue    = xQueueCreate(QUEUE_SIZE, sizeof(uint64_t));
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
