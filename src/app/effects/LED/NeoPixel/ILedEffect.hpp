#pragma once

class ILedEffect
{
    public:
        virtual void run(uint8_t r, uint8_t g, uint8_t b) = 0;
};