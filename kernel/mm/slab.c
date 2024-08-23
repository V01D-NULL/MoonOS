#define PR_MODULE "slab"

#include "slab.h"
#include <base/align.h>
#include <base/base-types.h>
#include <base/string.h>
#include <mm/phys.h>
#include <moon-io/serial.h>
#include <panic.h>

static struct slab *create_slab(struct slab_cache *cache, bool small_slab);

bool slab_cache_grow(struct slab_cache *cache, int count)
{
    for (int i = 0; i < count; i++)
    {
        /* Prepare bufctl */
        struct slab *slab = create_slab(cache, true);
        if (!slab)
            return false;
        struct slab_bufctl *buf = slab->freelist;

        /* Initialize the bufctl freelist */
        int                 elements = cache->bufctl_object_size / cache->size;
        struct slab_bufctl *tail     = buf;

        for (int i = 0; i < elements; i++)
        {
            uintptr_t offset        = ((uintptr_t)buf) + (cache->size * i);
            struct slab_bufctl *new = (struct slab_bufctl *)offset;
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

struct slab_cache *slab_create_cache(string_view name, size_t size,
                                     int alignment)
{
    if (size % alignment != 0)
        size = align(size, alignment);

    if (!SMALL_SLAB(size))
        panic("Large slabs are not supported yet");

    /* Note: This entire code works only for small slabs */
    struct slab_cache *cache = (struct slab_cache *)arch_alloc_page();
    if (!cache)
        return NULL;

    cache->size               = size;
    cache->descriptor         = name;
    cache->bufctl_object_size = PAGE_SIZE - sizeof(struct slab_cache);
    cache->nodes              = NULL;

    /* Initialize the bufctl freelist */
    slab_cache_grow(cache, 1);

    return cache;
}

static struct slab *create_slab(struct slab_cache *cache, bool small_slab)
{
    if (!small_slab)
    {
        trace(TRACE_SLAB,
              "__kmem_create_slab: '%s' is a large slab. They aren't supported "
              "yet.\n",
              cache->descriptor);
        return NULL;
    }

    /* Prepare bufctl */
    struct slab_bufctl *buf = (struct slab_bufctl *)arch_alloc_page();
    if (!buf)
        return NULL;

    buf->next = (struct slist){};

    /* Position slab at the end of the page */
    struct slab *slab =
        (struct slab *)(((uintptr_t)buf + PAGE_SIZE) - sizeof(struct slab));
    slab->next     = (struct slist){};
    slab->refcount = 0;
    slab->freelist = buf;

    /* Append the slab to the cache */
    if (!cache->nodes)
    {
        cache->nodes = slab;
    }
    else
    {
        struct slab *tail = NULL;

        list_foreach(out, next, cache->nodes) tail = out;

        list_set_next(slab, next, tail);
        tail->next = (struct slist){};
    }

    return slab;
}