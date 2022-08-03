#ifndef SLAB_DEFS_H
#define SLAB_DEFS_H

#include <moon-ds/linked_list.h>
#include <mm/mm.h>

#define SMALL_SLAB(x) (x <= (PAGE_SIZE / 8))

// Hierarchy:
//  cache -> slab -> freelist -> bufctl

struct kmem_bufctl
{
    struct slist next;
    void *ptr; // Memory address of this slab object
    struct kmem_slab *parent_slab; // Owner / parent of this bufctl
};

struct kmem_slab
{
    struct slist next, prev;
    struct kmem_bufctl *freelist;
    int refcount;
};

struct kmem_cache
{
    int size;
    int bufctl_object_size;
    string_view descriptor;
    struct kmem_slab *nodes;
};

#endif // SLAB_DEFS_H