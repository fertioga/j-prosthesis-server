#include "TaskService.hpp"

TaskService::TaskService()
{
}

void TaskService::create(TaskFunction_t taskFunction, const char* name, uint16_t stackSize, void* parameters, UBaseType_t priority, TaskHandle_t* handle, BaseType_t coreID=1)
{
    xTaskCreatePinnedToCore(   
        taskFunction,               
        name,                       
        stackSize,                  
        parameters,                 
        priority,                   
        handle,
        coreID                     
    );
}

TaskService::~TaskService()
{
    vTaskDelete(nullptr);
}