#ifndef SLAB_H
#define SLAB_H

/*
 *  slab.c and slab.h are allocator backends for kmalloc.c and kmalloc.h
*/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <printk.h>
#include <boot/boot.h>
#include "slab_defs.h"

struct kmem_cache *__kmem_cache_new(const char *name, size_t size, int alignment);

#endif // SLAB_H
