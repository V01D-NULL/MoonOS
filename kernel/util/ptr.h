#ifndef PTR_UTIL_H
#define PTR_UTIL_H

#include <stdint.h>

#define VAR_TO_VOID_PTR(type, data) ((void*)((type)data))

#define GENERIC_CAST(type, var) ((type) var)

#define VOID_PTR_TO_64(ptr) ((uint64_t)ptr)
#define VOID_PTR_TO_32(ptr) ((uint32_t)ptr)
#define VOID_PTR_TO_16(ptr) ((uint16_t)ptr)
#define VOID_PTR_TO_8(ptr) ((uint8_t )ptr)

#endif