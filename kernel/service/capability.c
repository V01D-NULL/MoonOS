#define PR_MODULE "cap"
#include "capability.h"
#include <base/align.h>
#include <mm/addr.h>
#include <mm/buddy.h>
#include <printk.h>

Capability capability_create(CapabilityType type, CapabilityData data,
                             bool revoked)
{
    Capability cap;
    cap.type    = type;
    cap.revoked = revoked;
    cap.data    = data;
    return cap;
}

Capability capability_create_memory_region(void *region, int sz, bool rw,
                                           CapabilityMemoryRegionType type)
{
    CapabilityData data;
    data.memory_region.range = (Range){.base = region, .limit = sz};
    data.memory_region.type  = type;

    trace(TRACE_CAP,
          "Creating memory region capability: base=%p, limit=%p, rw=%d\n",
          data.memory_region.range.base,
          data.memory_region.range.limit,
          rw);

    return capability_create(
        (rw ? CAP_WRITE : 0) | CAP_READ | CAP_MEMORY_REGION, data, false);
}

Capability capability_make_region_allocatable(Capability region_cap)
{
    if (!CAPABILITY_HAS_ACCESS(region_cap, CAP_MEMORY_REGION))
        return (Capability){.type = CAP_BAD};

    Range range = region_cap.data.memory_region.range;

    trace(TRACE_CAP,
          "Making region allocatable: base=%p, limit=%zu\n",
          va(range.base),
          range.limit);

    CapabilityData data;
    data.allocatable_memory_region.range = range;
    data.allocatable_memory_region.pool =
        buddy_embed(va(range.base), range.limit);

    return capability_create(
        CAP_ALLOCATABLE_MEMORY_REGION | CAP_READ | CAP_WRITE, data, false);
}

void *capability_alloc_from(Capability cap, size_t size)
{
    if (!CAPABILITY_HAS_ACCESS(cap, CAP_ALLOCATABLE_MEMORY_REGION))
        return NULL;

    void *ptr = buddy_malloc(cap.data.allocatable_memory_region.pool, size);
    if (ptr)
        memset(ptr, 0, size);

    return ptr;
}

void capablity_free_from(Capability cap, void *ptr)
{
    if (!CAPABILITY_HAS_ACCESS(cap, CAP_ALLOCATABLE_MEMORY_REGION))
        return;

    buddy_free(cap.data.allocatable_memory_region.pool, ptr);
}