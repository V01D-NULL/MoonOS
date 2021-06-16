#ifndef BYTES_H
#define BYTES_H

#include <stdint.h>
#include <stddef.h>
#include "../drivers/io/serial.h"

#define BYTE_EXTRACTION_MASK 0xFF

/* These can be used for both little to big endian conversion and vice versa */
uint16_t byte_order_reverse_short16(uint16_t byte);
uint32_t byte_order_reverse_int32(uint32_t byte);
uint64_t byte_order_reverse_long64(uint64_t byte);


#endif // BYTES_H