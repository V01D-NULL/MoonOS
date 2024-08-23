#ifndef KMEM_CACHE_H
#define KMEM_CACHE_H

#include "slab.h"

enum
{
#if defined(DEBUG_MM) || defined(DEBUG_KMALLOC)
    KMEM_VERBOSE_ALLOC = (1 << 0),  // Adds debug logging
#endif

    KMEM_PANIC = (1 << 1),  // Kernel panic if something goes wrong instead of
                            // returning NULL
    KMEM_NO_GROW  = (1 << 2),  // Don't automatically grow the cache if it's OOM
    KMEM_NO_FLAGS = (1 << 20)
};

typedef int kmem_flags_t;

struct slab_cache *kmem_cache_create(string_view name, size_t size,
                                     int alignment);
void *kmem_cache_alloc(struct slab_cache *cache, kmem_flags_t flags);
void  kmem_cache_free(struct slab_cache *cache, void *ptr);
void  kmem_cache_dump(struct slab_cache *cache, int n);
void  kmem_cache_destroy(struct slab_cache *cache);

#endif  // KMEM_CACHE_H