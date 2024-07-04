#define PR_MODULE "slab"

#include "slab.h"
#include <base/align.h>
#include <base/base-types.h>
#include <base/string.h>
#include <mm/phys.h>
#include <moon-io/serial.h>
#include <panic.h>

static struct kmem_slab *__kmem_create_slab(struct kmem_cache *cachep,
                                            bool               small_slab);

bool kmem_cache_grow(struct kmem_cache *cachep, int count)
{
    for (int i = 0; i < count; i++)
    {
        /* Prepare bufctl */
        struct kmem_slab *slab = __kmem_create_slab(cachep, true);
        if (!slab)
            return false;
        struct kmem_bufctl *buf = slab->freelist;

        /* Initialize the bufctl freelist */
        int elements             = cachep->bufctl_object_size / cachep->size;
        struct kmem_bufctl *tail = buf;

        for (int i = 0; i < elements; i++)
        {
            uintptr_t offset        = ((uintptr_t)buf) + (cachep->size * i);
            struct kmem_bufctl *new = (struct kmem_bufctl *)offset;
            new->parent_slab        = slab;
            new->ptr                = (void *)offset;

            if (!tail)
                buf = new;
            else
                list_set_next(new, next, tail);

            tail = new;
        }

        tail->next = (struct slist){};
    }

    return true;
}

struct kmem_cache *kmem_cache_new(string_view name, size_t size, int alignment)
{
    if (size % alignment != 0)
        size = align(size, alignment);

    if (!SMALL_SLAB(size))
        panic("Large slabs are not supported yet");

    /* Note: This entire code works only for small slabs */
    struct kmem_cache *cache = (struct kmem_cache *)arch_alloc_page();
    if (!cache)
        return NULL;

    cache->size               = size;
    cache->descriptor         = name;
    cache->bufctl_object_size = PAGE_SIZE - sizeof(struct kmem_cache);
    cache->nodes              = NULL;

    /* Initialize the bufctl freelist */
    kmem_cache_grow(cache, 1);

    return cache;
}

static struct kmem_slab *__kmem_create_slab(struct kmem_cache *cachep,
                                            bool               small_slab)
{
    if (!small_slab)
    {
        trace(
            "__kmem_create_slab: '%s' is a large slab. They aren't supported "
            "yet.\n",
            cachep->descriptor);
        return NULL;
    }

    /* Prepare bufctl */
    struct kmem_bufctl *buf = (struct kmem_bufctl *)arch_alloc_page();
    if (!buf)
        return NULL;

    buf->next = (struct slist){};

    /* Position slab at the end of the page */
    struct kmem_slab *slab = (struct kmem_slab *)(((uintptr_t)buf + PAGE_SIZE) -
                                                  sizeof(struct kmem_slab));
    slab->next             = (struct slist){};
    slab->refcount         = 0;
    slab->freelist         = buf;

    /* Append the slab to the cache */
    if (!cachep->nodes)
    {
        cachep->nodes = slab;
    }
    else
    {
        struct kmem_slab *tail = NULL;

        list_foreach(out, next, cachep->nodes) tail = out;

        list_set_next(slab, next, tail);
        tail->next = (struct slist){};
    }

    return slab;
}