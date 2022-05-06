#ifndef MEMDEFS_H
#define MEMDEFS_H

#include "pmm.h"
#include <stdint.h>
#include <mm/cpu/CR.h>
#include <libk/kassert.h>
#include <ds/linked_list.h>

#define PAGE_SIZE 4096

// Used by the buddy allocator
#define MM_MAX_ORDER 10 // Largest allocation: 2^10-1 => 0x200000 => 2MiB

// Order -> words so that buddy_alloc(9) -> buddy_alloc(BUDDY_SIZE_4K)
#define BUDDY_SIZE_4K 9
#define BUDDY_SIZE_8K 8
#define BUDDY_SIZE_16K 7
#define BUDDY_SIZE_32K 6
#define BUDDY_SIZE_64K 5
#define BUDDY_SIZE_128K 4
#define BUDDY_SIZE_256K 3
#define BUDDY_SIZE_512K 2
#define BUDDY_SIZE_512K 2
#define BUDDY_SIZE_1MB 1
#define BUDDY_SIZE_2MB 0

#define ORDER_TO_SIZE(order) (4096 * (1 << ((MM_MAX_ORDER - 1) - order))) // size = MINIMUM_SIZE * (2 ^ (MAXIMUM_ORDER - order)) (Thanks johnkaS)

struct page
{
    int8_t order;
    uint64_t *ptr;
};

struct BuddyZone
{
    size_t base_addr;
    bool is_full;
    long zone_nr;      // zone_nr denotes the offset/index of this buddy zone in the list of buddy zones
    long *alloc_map;   // Linearized binary tree (1 allocated, 0 not allocated)
    struct slist list; // A list of buddy zones to span the entire memory for a given zone
};

struct zone
{
    // Vital information
    struct slist list;             // A pointer to the next zone
    uint8_t *bitmap;
    
    // Statistics
    size_t start;           // Base address for this zone
    size_t len;             // Length of this zone in bytes.
    size_t page_count;      // Number of pages for this zone.
    size_t num_buddy_zones; // Number of buddy zones for this zone
    int zone_nr;            // Note: zone.list->next will be 'zone_nr + 1'
    const char *name;

    // struct page *freelist[MM_MAX_ORDER];
};

struct buddy_range
{
    int element_count;   // Actual no. of elements
    size_t elements[10]; // 2MiB per mm_range_element, leaves a max of 20MiB per buddy_range
};

static const size_t $identity_vma    = 0x0;                /* Regular, physical memory offset   */
static const size_t $high_vma        = 0xffff800000000000; /* Kernel data structures offset     */
static const size_t $high_vma_heap   = 0xffffd00000000000; /* Kernel heap area offset           */
static const size_t $high_vma_code   = 0xffffffff80000000; /* Kernel code offset                */

STATIC_INLINE uint64_t *va(size_t paddr)
{
    return (uint64_t*) (paddr + ($high_vma));
}

STATIC_INLINE uint64_t *pa(size_t vaddr)
{
    return (uint64_t*) (vaddr - ($high_vma));
}

STATIC_INLINE bool is_page_aligned(void *addr) { return ((size_t)addr) % PAGE_SIZE == 0; }

#endif // MEMDEFS_H