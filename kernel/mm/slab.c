#include "slab.h"
#include <stdbool.h>
#include <libk/kstring.h>
#include <panic.h>
#include <mm/pmm.h>
#include <devices/term/fallback/fterm.h>

/* Utility functions */
bool is_power_of_two(int n);
slab_t create_system_slab(size_t size);
static int32_t find_free(int which);
static int32_t find_allocated(int which);
static int32_t slab_align(int sz);

static slab_t slabs[10];
static int numpages = 0;

/* Core functions */
void slab_init(void)
{
    slabs[0] = create_system_slab(2);
    slabs[1] = create_system_slab(4);
    slabs[2] = create_system_slab(8);
    slabs[3] = create_system_slab(16);
    slabs[4] = create_system_slab(32);
    slabs[5] = create_system_slab(64);
    slabs[6] = create_system_slab(128);
    slabs[7] = create_system_slab(256);
    slabs[8] = create_system_slab(512);
    slabs[9] = create_system_slab(1024);
    printk("slab", "Allocated %d pages for the slab-heap\n", numpages);
}

uint64_t *slab_alloc(int sz)
{
    if (!is_power_of_two(sz))
        sz = slab_align(sz);

    for (int i = 0; i < SLABCOUNT; i++)
    {
        if (slabs[i].size != sz)
            continue;

        if (slabs[i].exhausted)
            return NULL;
        
        uint64_t *ret = NULL;

        int32_t idx = find_free(i);
        if (idx == -1)
        {
            slabs[i].exhausted = true;
			
        }
        else
        {
            ret = slabs[i].objects[idx].mem;
            slabs[i].objects[idx].mem = NULL;
            slabs[i].free_objects--;
            slabs[i].used_objects++;
        }
        return ret;
    }
    return NULL;
}

void slab_free(uint64_t *ptr, int sz)
{
    if (!ptr)
        return;

    if (!is_power_of_two(sz))
        sz = slab_align(sz);

    for (int i = 0; i < SLABCOUNT; i++)
    {
        if (slabs[i].size != sz)
            continue;

        int32_t idx = find_allocated(i);
        if (idx == -1)
            return;

        slabs[i].objects[idx].mem = ptr;
        slabs[i].exhausted = false;
        slabs[i].free_objects++;
        slabs[i].used_objects--;
    }
}

bool is_power_of_two(int n)
{
    return (n > 0) && ((n & (n - 1)) == 0);
}

uint8_t *ptr = NULL;
uint64_t base = 0;
uint64_t len = 0;
slab_t create_system_slab(size_t size)
{
    slab_t slab;
    slab.descriptor = "system";
    slab.free_objects = MAX_OBJECTS;
    slab.used_objects = 0;
    slab.size = size;
    slab.exhausted = false;

    if (!ptr)
    {
        numpages++;
        ptr = pmm_alloc();
        base = (uint64_t)ptr;
        len = (uint64_t)base + PAGE_SIZE;
    }

    for (int i = 0; i < MAX_OBJECTS; i++)
    {
        if ((uint64_t)ptr == len)
        {
            numpages++;
            ptr = pmm_alloc();
            base = (uint64_t)ptr;
            len = (uint64_t)base + PAGE_SIZE;
        }

        slab.objects[i] = (slab_object_t){.mem = (uint64_t *)ptr};
        ptr += size;
    }

    return slab;
}

// Find an allocated object in the objects and return it's index.
// Returns -1 on error.
static int32_t find_allocated(int which)
{
    for (int i = 0; i < MAX_OBJECTS; i++)
    {
        // Found an allocated object. It doesn't matter whom it belongs to since
        // there is no metadata, and there are no concerns about fragmentation.
        if (slabs[which].objects[i].mem == NULL)
            return i;
    }

    return -1; // Invalid free(), objects are completely free.
}

static int32_t find_free(int which)
{
    for (int i = 0; i < MAX_OBJECTS; i++)
    {
        // Found a free object. It doesn't matter whom it belongs to since
        // there is no metadata, and there are no concerns about fragmentation.
        if (slabs[which].objects[i].mem != NULL)
            return i;
    }

    return -1; // Objects are completely full
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