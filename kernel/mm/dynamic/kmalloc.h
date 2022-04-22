#ifndef KMALLOC_H
#define KMALLOC_H

#include "slab.h"

struct kmem_cache *kmem_cache_create(const char *name, size_t size, int alignment);
void *kmem_cache_alloc(struct kmem_cache *cachep, int flags);

#endif // KMALLOC_H