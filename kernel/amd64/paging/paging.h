#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <amd64/moon.h>

// Offsets of the bits in paging structures used for PAT indexing
enum pat_indexing_4kpages {
    PG_PAT = 7, // Bit 7 = PAT
    PG_PCD = 4, // Bit 4 = PCD
    PG_PWT = 3   // Bit 3 = PWT
};

STATIC_INLINE void paging_cache_disable_set(uint64_t *pml, int level, int bit)
{
    uint64_t val = bit & PG_PCD;
    pml[level] |= (val << PG_PCD);
}

STATIC_INLINE void paging_write_through_set(uint64_t *pml, int level, int bit)
{
    bool val = bit & PG_PWT;
    pml[level] |= (val << PG_PWT);
}

STATIC_INLINE void paging_pat_set(uint64_t *pml, int level, int bit)
{
    uint64_t val = bit & PG_PAT;
    pml[level] |= (val << PG_PAT);
}

#endif // PAGING_H