#define PR_MODULE "slab_cache"

#include "kmem_cache.h"
#include <base/base-types.h>
#include <base/mem.h>
#include <mm/phys.h>
#include <moon-ds/linked_list.h>
#include <moon-io/serial.h>
#include <panic.h>

struct slab_cache *kmem_cache_create(string_view name, size_t size,
                                     int alignment)
{
    if (!alignment)
        alignment = 8;

    if (!size)
        return NULL;

    return slab_create_cache(name, size, alignment);
}

void *kmem_cache_alloc(struct slab_cache *cache, kmem_flags_t flags)
{
    if (!cache)
        return NULL;

    if (!cache->nodes)
    {
        if (flags & KMEM_PANIC)
            panic(
                "No nodes for this cache (This shouldn't happen unless the "
                "cache has been destroyed)");

        return NULL;
    }

    // Iterate through all the slabs and find a freelist with available
    // slab_bufctl's
    list_foreach(slab, next, cache->nodes)
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

        slab->refcount++;
        return ptr;
    }

    // Are we allowed to automatically grow the cache?
    if (flags & KMEM_NO_GROW)
        return NULL;

    // There are no available slabs in this cache, so we grow it.
    bool success = slab_cache_grow(cache, 1);
    if (!success)
    {
        if (flags & KMEM_PANIC)
            panic("Tried to grow cache for '%s' but ran out of memory!",
                  cache->descriptor);
        return NULL;
    }

    return kmem_cache_alloc(cache, flags);
}

void kmem_cache_free(struct slab_cache *cache, void *ptr)
{
    if (!ptr || !cache)
        return;

    list_foreach(slab, next, cache->nodes)
    {
        if (!slab->freelist)
            continue;

        slab->refcount--;

        // Put the bufctl back on the freelist
        *((void **)ptr) = slab->freelist;
        slab->freelist  = ptr;
        return;
    }
}

void kmem_cache_destroy(struct slab_cache *cache)
{
    if (!cache)
        return;

    list_foreach(slab, next, cache->nodes)
    {
        if (slab->refcount != 0)
            panic(
                "Tried to destroy slab '%s' but it's refcount wasn't 0! "
                "(refcount: %d)",
                cache->descriptor,
                slab->refcount);

        list_foreach(bufctl, next, slab->freelist)
            arch_free_page((void *)bufctl);
    }

    memset(cache, 0, sizeof(struct slab_cache));
    arch_free_page(cache);
}

// Dump up to 'n' slabs
void kmem_cache_dump(struct slab_cache *cache, int n)
{
    trace(TRACE_SLAB, "Dumping bufctl freelists...\n");
    debug(true, "Dumping bufctl freelists...\n");
    int counter = 0;

    list_foreach(slab, next, cache->nodes)
    {
        trace(TRACE_SLAB, "Found slab at %p\n", slab);
        debug(true, "Found slab at %p\n", slab);

        list_foreach(out, next, slab->freelist)
        {
            debug(true, "out->ptr: %lx\n", out->ptr);
            trace(TRACE_SLAB, "out->ptr: %lx\n", out->ptr);
        }

        // If we've dumped 'n' slabs, return
        // (if n == 0, dump all slabs)
        if (++counter == n)
            return;
    }
}