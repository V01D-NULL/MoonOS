#include "kmalloc.h"
#include <devices/serial/serial.h>
#include <ds/linked_list.h>
#include <ktypes.h>
#include <panic.h>

// Defined in slab.c
// This function is only needed in kmalloc so it's not placed in slab.h in order to discourage it's usage elsewhere.
extern struct kmem_cache *kmem_cache_new(const char *name, size_t size, int alignment);

void *kmem_cache_alloc(struct kmem_cache *cachep, kmem_flags_t flags)
{
    if (!cachep->nodes)
    {
        if (flags & KMEM_PANIC)
            panic("No nodes for this cache (This shouldn't happen unless the cache has been destroyed)");
        
        return NULL;
    }

    // Iterate through all the slabs and find a freelist with available kmem_bufctl's
    list_foreach(slab, next, cachep->nodes)
    {
        if (!slab->freelist)
            continue;

#if defined(DEBUG_MM) || defined(DEBUG_KMALLOC)
        if (flags & KMEM_VERBOSE_ALLOC)
            debug(true, "Found slab at %p\n", slab);
#endif

        // Perform the allocation
        auto ptr = slab->freelist->ptr;
        
        // Remove bufctl from the freelist
        slab->freelist = list_next(slab->freelist, next);

        if (flags & KMEM_HIGH_VMA)
            ptr += $high_vma;

        return ptr;
    }

    // Are we allowed to automatically grow the cache?
    if (flags & KMEM_NO_GROW)
        return NULL;

    // There are no available slabs in this cache, so we grow it.
    bool success = kmem_cache_grow(cachep, 1);
    if (!success)
    {
        if (flags & KMEM_PANIC)
            panic("Tried to grow cache for '%s' but ran out of memory!", cachep->descriptor);
        return NULL;
    }

    return kmem_cache_alloc(cachep, flags);
}

struct kmem_cache *kmem_cache_create(const char *name, size_t size, int alignment)
{
    if (!alignment)
        alignment = 8;

    if (!size)
        return NULL;

    return kmem_cache_new(name, size, alignment);
}