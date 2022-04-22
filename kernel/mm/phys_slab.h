#ifndef SLAB_H
#define SLAB_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <printk.h>
#include <boot/boot.h>

#define MAX_OBJECTS 512
#define SLABCOUNT 10

// Represents an object of a given slab
typedef struct
{
    uint64_t *mem;
    // Todo: Flags
} slab_object_t;

// Represents a slab itself
typedef struct slab
{
    const char *descriptor;
    int free_objects;
    int used_objects;
    bool exhausted;
    int size;
    slab_object_t objects[MAX_OBJECTS];
} slab_t;

/* Core functions */
void slab_init(void);
uint64_t *slab_alloc(int sz);
void slab_free(uint64_t *ptr, int sz);

void slab_panic(bool status);

#endif // SLAB_H
