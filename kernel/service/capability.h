#ifndef CAPABILITY_H
#define CAPABILITY_H

#include <mm/buddy.h>
#include <mm/tlsf.h>
#include <moon-extra/range.h>
#include <service/object.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define CAPABILITY_HAS_ACCESS(cap, access) ((cap.type & access) == access)

typedef enum
{
    CAP_BAD                       = 0,
    CAP_READ                      = (1 << 1),
    CAP_WRITE                     = (1 << 2),
    CAP_MEMORY_REGION             = (1 << 3),
    CAP_ALLOCATABLE_MEMORY_REGION = (1 << 4),
} CapabilityType;

typedef enum
{
    CAP_MEMORY_REGION_RAM,
    CAP_MEMORY_REGION_FRAMEBUFFER,
} CapabilityMemoryRegionType;

typedef union
{
    struct
    {
        Range         range;
        struct buddy *pool;
    } allocatable_memory_region;

    struct
    {
        CapabilityMemoryRegionType type;
        Range                      range;
    } memory_region;

    Object object;
} CapabilityData;

typedef struct
{
    CapabilityType type;
    CapabilityData data;
    bool           revoked;
} Capability;

Capability capability_create_memory_region(void *region, int sz, bool rw,
                                           CapabilityMemoryRegionType type);
Capability capability_make_region_allocatable(Capability region_cap);
void      *capability_alloc_from(Capability cap, size_t size);
void      *capability_alloc_from_aligned(Capability cap, size_t size,
                                         size_t alignment);

#endif  // CAPABILITY_H