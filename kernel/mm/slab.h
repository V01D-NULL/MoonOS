#ifndef SLAB_H
#define SLAB_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <printk.h>
#include <boot/bootloader_stivale2.h>

#define LOG(...) printk("slab-log", __VA_ARGS__)
#ifdef LOG_VERBOSE
#define LOGV(...) fmt_puts("\e[1;34m"); fmt_puts(__VA_ARGS__); fmt_puts("\e[0m")
#else
#define LOGV(...)
#endif

#define BUG(...) { fmt_puts("\033[31mBUG:\033[39m "); fmt_puts(__VA_ARGS__); }

#define MAX_OBJECTS 128
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

#endif // SLAB_H
