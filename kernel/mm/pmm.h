#ifndef BITMAP_PMM_H
#define BITMAP_PMM_H

#include <stdint.h>
#include <amd64/moon.h>
#include <stivale2.h>
#include <stdbool.h>
#include <util/range.h>

#define PAGE_SIZE 4096
#define PMM_INVALID NULL
#define PMM_FREE 0
#define PMM_USED 1
#define PMM_FULL (-1)
#define ALIGN_UP(__number) (((__number) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define ALIGN_DOWN(__number) ((__number) & ~(PAGE_SIZE - 1))

enum
{
    BIT_CLEAR = 0x1,
    BIT_SET
};

void pmm_init(struct stivale2_mmap_entry *mmap, int entries);
void *pmm_alloc(void);
void *find_first_free_block(void);
void *pmm_alloc_any(void *addr);
range_t pmm_alloc_range(size_t pages);
void pmm_free(void *page);
bool in_range(void *_address);
struct memtag_range pmm_find_tag(size_t tag, int retries);
void dump_mmap(void);

struct memtag_range
{
    uint64_t base;
    uint64_t size;
};

typedef struct
{
    /* Used to parse the memory map */
    long entries;
    uint64_t base;
    uint64_t length;

    uint64_t abs_top;  // The highest address specified in the mmap
    uint64_t abs_base; // The lowest address specified in the mmap
} mmap_t;

#endif // BITMAP_PMM_H
