#include <amd64/paging/paging.h>
#include <ktypes.h>

inline bool check_flag(int flags, int bit) { return (flags >> bit) & 1; }

struct pte paging_create_entry(uint64_t paddr, int flags)
{
    return (struct pte) {
        .present = check_flag(flags, 0),
        .readwrite = check_flag(flags, 1),
        .supervisor = check_flag(flags, 2),
        .writethrough = 0,
        .cache_disabled = 0,
        .accessed = 0,
        .dirty = 0,
        .ignore = 0,
        .global = 0,
        .avail = 0,
        .address = paddr >> PAGE_SHIFT // PFN
    };
}

struct pte paging_purge_entry()
{
    return paging_create_entry(0, 0);
}
