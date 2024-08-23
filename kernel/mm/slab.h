#ifndef SLAB_H
#define SLAB_H

#include <base/base-types.h>
#include <mm/page.h>
#include <moon-ds/linked_list.h>
#include <printk.h>

#define SMALL_SLAB(x) (x <= (PAGE_SIZE / 8))

// Hierarchy:
//  cache -> slab -> freelist -> bufctl

struct slab_bufctl
{
    struct slist next;
    void        *ptr;          // Memory address of this slab object
    struct slab *parent_slab;  // Owner / parent of this bufctl
};

struct slab
{
    struct slist        next, prev;
    struct slab_bufctl *freelist;
    int                 refcount;
};

struct slab_cache
{
    int          size;
    int          bufctl_object_size;
    string_view  descriptor;
    struct slab *nodes;
};

bool               slab_cache_grow(struct slab_cache *cache, int count);
struct slab_cache *slab_create_cache(string_view name, size_t size,
                                     int alignment);

#endif  // SLAB_H
