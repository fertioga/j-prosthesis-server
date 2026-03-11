#pragma once

#include "app/configs/QueuesConfig.hpp"

class QueueService
{
public:

    template<typename T>
    void send(QueueHandle_t queue, T value)
    {
        if (xQueueSend(queue, &value, 0) != pdPASS)
        {
            Serial.println("Full queue. Failed to send value.");
        }
        else
        {
            Serial.println("Command sent to queue successfully.");
        }
    }
};