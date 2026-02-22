#ifndef CAPABILITY_H
#define CAPABILITY_H

#include <mm/tlsf.h>
#include <moon-extra/range.h>
#include <service/object.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum
{
    CAP_BAD           = 0,
    CAP_READ          = (1 << 1),
    CAP_WRITE         = (1 << 2),
    CAP_MEMORY_REGION = (1 << 3),
    CAP_LOG           = (1 << 4),
} CapabilityType;

typedef union
{
    struct
    {
        Range  range;
        tlsf_t pool;
    } memory_region;

    Object object;
} CapabilityData;

typedef struct
{
    CapabilityType type;
    bool           revoked;
    CapabilityData data;
} Capability;

Capability capability_create_memory_region(void *region, int sz, bool rw);
void      *capability_alloc_from(Capability cap, size_t size);

#endif  // CAPABILITY_H