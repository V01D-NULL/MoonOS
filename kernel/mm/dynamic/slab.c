#include <libk/kstring.h>
#include <util/common.h>
#include <stdbool.h>
#include <panic.h>
#include <mm/mm.h>
#include <devices/serial/serial.h>
#include "slab.h"

static bool is_power_of_two(int n);
static int32_t slab_align(int sz);

static size_t heap_loc = $high_vma_heap;

struct kmem_cache *__kmem_cache_new(const char *name, size_t size, int alignment)
{
    if (!is_power_of_two(size))
        size = slab_align(size);

    if (!SMALL_SLAB(size))
        panic("Large slabs are not supported yet");

    /* Note: This entire code works only for small slabs */
    struct kmem_cache *cache = (struct kmem_cache *)heap_loc;
    cache->descriptor = name;
    cache->bufctl_object_size = (alignment * ((size-1)/alignment + 1));

    /* Prepare bufctl */
    struct kmem_bufctl *buf = (struct kmem_bufctl *)heap_loc;
    int len = PAGE_SIZE - sizeof(struct kmem_slab);
    
    /* Offset for the small cache structure + memory for the small slab itself */
    heap_loc += align(sizeof(struct kmem_cache), alignment) + PAGE_SIZE;

    /* Position slab at the end of the page */
    struct kmem_slab *slab = (struct kmem_slab *)(((uintptr_t)buf + PAGE_SIZE) - sizeof(struct kmem_slab));
    slab->next = slab->prev = (struct slist) {};
    slab->refcount = 0;
    slab->freelist = buf;

    /* Initialize this slab's freelist */
    int elements = len / size;
    struct kmem_bufctl *tail = NULL;

    for (int i = 0; i < elements; i++)
    {
        uintptr_t offset = ((uint8_t*)buf) + (size * i);
        struct kmem_bufctl *new = (struct kmem_bufctl *)(offset);
        new->parent_slab = slab;
        new->vma_ptr = (void*)(offset);

        if (!tail)
            buf = new;
        else
            list_set_next(new, next, tail);

        tail = new;
    }
    list_set_next((struct kmem_bufctl*)NULL, next, tail);

    return cache;
}

static bool is_power_of_two(int n)
{
    return (n > 0) && ((n & (n - 1)) == 0);
}

static int32_t slab_align(int sz)
{
    sz--;
    sz |= sz >> 1;
    sz |= sz >> 2;
    sz |= sz >> 4;
    sz |= sz >> 8;
    sz |= sz >> 16;
    return ++sz;
}