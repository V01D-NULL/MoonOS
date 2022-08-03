#ifndef SLAB_H
#define SLAB_H

/*
 *  slab.c and slab.h are allocator backends for kmalloc.c and kmalloc.h
*/
#include <base/base-types.h>
#include <printk.h>
#include <boot/boot.h>
#include "slab_defs.h"

bool kmem_cache_grow(struct kmem_cache *cachep, int count);

#endif // SLAB_H
