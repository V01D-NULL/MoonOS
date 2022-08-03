#ifndef MEMDEFS_H
#define MEMDEFS_H

#include "addr.h"
#include "page.h"
#include <base/base-types.h>
#include <moon-ds/linked_list.h>

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
    string_view name;
};

#endif // MEMDEFS_H