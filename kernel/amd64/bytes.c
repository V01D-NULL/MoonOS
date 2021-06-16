#include "bytes.h"
#include "../common.h"

uint16_t byte_order_reverse_short16(uint16_t byte)
{
    //Extract bits
    int b1 = (byte >> 8) & BYTE_EXTRACTION_MASK;
    int b2 = (byte) & BYTE_EXTRACTION_MASK;
    
    //Concatenate them in reverse order
    return (b2 << 8) | (b1);
}

uint32_t byte_order_reverse_int32(uint32_t byte)
{
    int b1 = (byte >> 16) & BYTE_EXTRACTION_MASK;
    int b2 = (byte >> 8) & BYTE_EXTRACTION_MASK;
    int b3 = (byte) & BYTE_EXTRACTION_MASK;
    return (b3 << 16) | (b2 << 8) | (b1);
}

uint64_t byte_order_reverse_long64(uint64_t byte)
{
    int b1 = (byte >> 24) & BYTE_EXTRACTION_MASK;
    int b2 = (byte >> 16) & BYTE_EXTRACTION_MASK;
    int b3 = (byte >> 8) & BYTE_EXTRACTION_MASK;
    int b4 = (byte) & BYTE_EXTRACTION_MASK;
    return (b4 << 24) | (b3 << 16) | (b2 << 8) | (b1);
}