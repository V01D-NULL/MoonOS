#include "bytes.h"
#include "../util/common.h"

uint8_t byte_order_reverse_char8(uint8_t byte)
{
    uint8_t res = 0;
    for (int i = 0; i < 8; i++)
    {
        res <<= 1;
        if (byte & 1)
        {
            res ^= 1;
        }
        byte >>= 1;
    }
    return res;
}

uint16_t byte_order_reverse_short16(uint16_t byte)
{
    //Extract bits
    int b1 = (byte >> 8) & BYTE_EXTRACTION_MASK;
    int b2 = (byte)&BYTE_EXTRACTION_MASK;

    //Concatenate them in reverse order
    return (b2 << 8) | (b1);
}

uint32_t byte_order_reverse_int32(uint32_t byte)
{
    int b1 = (byte >> 16) & BYTE_EXTRACTION_MASK;
    int b2 = (byte >> 8) & BYTE_EXTRACTION_MASK;
    int b3 = (byte)&BYTE_EXTRACTION_MASK;
    return (b3 << 16) | (b2 << 8) | (b1);
}

uint64_t byte_order_reverse_long64(uint64_t byte)
{
    int b1 = (byte >> 24) & BYTE_EXTRACTION_MASK;
    int b2 = (byte >> 16) & BYTE_EXTRACTION_MASK;
    int b3 = (byte >> 8) & BYTE_EXTRACTION_MASK;
    int b4 = (byte)&BYTE_EXTRACTION_MASK;
    return (b4 << 24) | (b3 << 16) | (b2 << 8) | (b1);
}