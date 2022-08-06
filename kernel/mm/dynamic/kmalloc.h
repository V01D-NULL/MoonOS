#ifndef KMALLOC_H
#define KMALLOC_H

#include "slab.h"

enum
{
#if defined(DEBUG_MM) || defined(DEBUG_KMALLOC)
    KMEM_VERBOSE_ALLOC = (1 << 0), // Adds debug logging, but it butchers performance and is thus only enabled when debugging the mm
#endif

    KMEM_PANIC = (1 << 1),    // Kernel panic if something goes wrong instead of returning NULL
    KMEM_HIGH_VMA = (1 << 2), // Add $high_vma to the value returned by a kmem allocation routine
    KMEM_NO_GROW = (1 << 3),  // Don't automatically grow the cache if it's OOM
    KMEM_NO_FLAGS = (1 << 20) // Arbitrary shift count so that this flag won't collide with others
};

typedef int kmem_flags_t;

void kalloc_init(void);
void *kalloc(int sz, kmem_flags_t flags);
void kfree(void *ptr, int sz);

struct kmem_cache *kmem_cache_create(string_view name, size_t size, int alignment);
void *kmem_cache_alloc(struct kmem_cache *cachep, kmem_flags_t flags);
void kmem_cache_free(struct kmem_cache *cachep, void *ptr);
void kmem_cache_dump(struct kmem_cache *cachep, int n);
void kmem_cache_destroy(struct kmem_cache *cachep);

#endif // KMALLOC_H