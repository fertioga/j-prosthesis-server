#pragma once
#include "bootstrap.hpp"

class IAction
{
    public:
        virtual void task(void* pvParameters) = 0;
        virtual void run() = 0; 

    /**
     * Used to create a static entry point for FreeRTOS tasks, which require a function with C linkage.
     * This function will call the non-static task() method of the derived class.
     */
    static void entry(void* param)
    {
        IAction* self = static_cast<IAction*>(param);

        self->task(param);
        
    }
};