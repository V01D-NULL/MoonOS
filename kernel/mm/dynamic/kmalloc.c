#include "kmalloc.h"

void *kmem_cache_alloc(struct kmem_cache *cachep, int flags)
{
    (void)flags;
    
}

struct kmem_cache *kmem_cache_create(const char *name, size_t size, int alignment)
{
    if (!alignment)
        alignment = 8;

    if (!size)
        return NULL;

    return __kmem_cache_new(name, size, alignment);
}