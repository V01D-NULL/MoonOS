#include "capability.h"
#include <mm/addr.h>
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

Capability capability_create_memory_region(void *region, int sz, bool rw)
{
    Range range = {.base = region, .limit = region + sz};
    trace(TRACE_ALL,
          "Creating memory region capability for range [%p - %p]\n",
          range.base,
          range.limit);
    tlsf_t pool = tlsf_create_with_pool(va(region), sz);

    trace(TRACE_ALL,
          "Created TLSF pool at %p for memory region capability\n",
          pool);

    CapabilityData data;
    data.memory_region.range = range;
    data.memory_region.pool  = pool;

    return capability_create(
        CAP_MEMORY_REGION | (rw ? CAP_READ | CAP_WRITE : CAP_READ),
        data,
        false);
}

void *capability_alloc_from(Capability cap, size_t size)
{
    if (cap.type != CAP_MEMORY_REGION)
        return NULL;

    return tlsf_malloc(va(cap.data.memory_region.pool), size);
}