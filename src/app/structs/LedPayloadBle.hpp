#pragma once

#pragma pack(push, 1) // make sure the struct is packed without padding
struct LedPayloadBle
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t brigthness;
    uint8_t effect;
};
#pragma pack(pop) // restore default packing
