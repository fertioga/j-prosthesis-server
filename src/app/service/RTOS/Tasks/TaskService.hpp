#pragma once

#include "bootstrap.hpp"
class TaskService
{
    public:
        TaskService();
        void create(TaskFunction_t taskFunction, const char* name, uint16_t stackSize, void* parameters, UBaseType_t priority, TaskHandle_t* handle);
        ~TaskService();
};


