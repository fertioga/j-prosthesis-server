#pragma once
#include "bootstrap.hpp"

class IAction
{
    public:
        virtual void run() = 0; // Pure virtual function to be implemented by derived classes
};