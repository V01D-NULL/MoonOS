#include <base/base-types.h>
#include <mm/addr.h>
#include <mm/buddy.h>
#include <moon-ds/linked_list.h>

struct zone
{
    // Vital information
    struct slist  list;   // A pointer to the next zone
    struct buddy *buddy;  // Buddy allocator for this zone

    // Statistics
    size_t      start;       // Base address for this zone
    size_t      len;         // Length of this zone in bytes.
    size_t      page_count;  // Number of pages for this zone.
    int         zone_nr;
    string_view name;
};
