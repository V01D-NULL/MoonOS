#ifndef MEMDEFS_H
#define MEMDEFS_H

#include "addr.h"
#include "page.h"
#include <stdint.h>
#include <ds/linked_list.h>

#define ALIGN_UP(__number) (((__number) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define ALIGN_DOWN(__number) ((__number) & ~(PAGE_SIZE - 1))

struct zone
{
    // Vital information
    struct slist list; // A pointer to the next zone
    uint8_t *bitmap;

    // Statistics
    size_t start;      // Base address for this zone
    size_t len;        // Length of this zone in bytes.
    size_t page_count; // Number of pages for this zone.
    int zone_nr;
    const char *name;
};

#endif // MEMDEFS_H