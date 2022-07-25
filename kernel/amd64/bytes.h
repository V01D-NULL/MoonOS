#ifndef BYTES_H
#define BYTES_H

#include <stdint.h>
#include <stddef.h>
#include <devices/serial/serial.h>
#include "moon.h"

#define BYTE_EXTRACTION_MASK 0xFF

/* These can be used for both little to big endian conversion and vice versa */
uint8_t byte_order_reverse_char8(uint8_t byte);
uint16_t byte_order_reverse_short16(uint16_t byte);
uint32_t byte_order_reverse_int32(uint32_t byte);
uint64_t byte_order_reverse_long64(uint64_t byte);

inline uint64_t concat64(uint32_t a, uint32_t b)
{
    return ((uint64_t)b << 32) | a;
}

#endif // BYTES_H