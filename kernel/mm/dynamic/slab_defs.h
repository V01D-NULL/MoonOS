#ifndef SLAB_DEFS_H
#define SLAB_DEFS_H

#include <ds/linked_list.h>
#include <mm/mm.h>

#define SMALL_SLAB(x) (x <= (PAGE_SIZE / 8))

// Hierarchy:
//  cache -> slab -> freelist -> bufctl

struct kmem_bufctl
{
    struct slist next;
    struct kmem_slab *parent_slab; // Owner or parent of this bufctl
    void *vma_ptr; // Virtual memory address for this slab object
};

struct kmem_slab
{
    struct slist next, prev;
    struct kmem_bufctl *freelist;
    size_t refcount;
};

struct kmem_cache
{
    size_t bufctl_object_size;
    const char *descriptor;
    struct kmem_slab *nodes;
};

#endif // SLAB_DEFS_H