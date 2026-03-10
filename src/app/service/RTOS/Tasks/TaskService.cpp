#include "TaskService.hpp"

TaskService::TaskService()
{
}

void TaskService::create(TaskFunction_t taskFunction, const char* name, uint16_t stackSize, void* parameters, UBaseType_t priority, TaskHandle_t* handle)
{
    xTaskCreate(   
        taskFunction,               
        name,                       
        stackSize,                  
        parameters,                 
        priority,                   
        handle                     
    );
}

TaskService::~TaskService()
{
    vTaskDelete(nullptr);
}