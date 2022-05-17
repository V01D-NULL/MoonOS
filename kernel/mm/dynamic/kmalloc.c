#define PR_MODULE "kmalloc"

#include "kmalloc.h"
#include <devices/serial/serial.h>
#include <ds/linked_list.h>
#include <ktypes.h>
#include <panic.h>

// Defined in slab.c
// This function is only needed in kmalloc so it's not placed in slab.h in order to
// discourage it's usage elsewhere since it's confusingly similar to kmem_cache_create.
extern struct kmem_cache *kmem_cache_new(const char *name, size_t size, int alignment);

struct kmem_cache *kmem_cache_create(const char *name, size_t size, int alignment)
{
    if (!alignment)
        alignment = 8;

    if (!size)
        return NULL;

    return kmem_cache_new(name, size, alignment);
}

void *kmem_cache_alloc(struct kmem_cache *cachep, kmem_flags_t flags)
{
    if (!cachep)
        return NULL;

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

        slab->refcount++;
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

void kmem_cache_free(struct kmem_cache *cachep, void *ptr)
{
    if (!ptr || !cachep)
        return;
    
    list_foreach(slab, next, cachep->nodes)
    {
        if (!slab->freelist)
            continue;

        slab->refcount--;

        // Put the bufctl back on the freelist
        *((void **)ptr) = slab->freelist;
        slab->freelist = ptr;
        return;
    }
}

void kmem_cache_destroy(struct kmem_cache *cachep)
{
    if (!cachep)
        return;

    list_foreach(slab, next, cachep->nodes)
    {
        if (slab->refcount != 0)
            panic("Tried to destroy slab '%s' but it's refcount wasn't 0! (refcount: %d)", cachep->descriptor, slab->refcount);

        list_foreach(bufctl, next, slab->freelist)
            pmm_free((void*)bufctl);
    }
    
    memset(cachep, 0, sizeof(struct kmem_cache));
    pmm_free(cachep);
}

// Dump up to 'n' slabs
void kmem_cache_dump(struct kmem_cache *cachep, int n)
{
    pr_info("Dumping bufctl freelists...\n");
    debug(true, "Dumping bufctl freelists...\n");
    int counter = 0;

    list_foreach(slab, next, cachep->nodes)
    {
        pr_info("Found slab at %p\n", slab);
        debug(true, "Found slab at %p\n", slab);

        list_foreach(out, next, slab->freelist)
        {
            debug(true, "out->ptr: %lx\n", out->ptr);
            pr_info("out->ptr: %lx\n", out->ptr);
        }

        if (n > 0)
        {
            counter++;
            if (counter == n)
                return;
        }
    }
}