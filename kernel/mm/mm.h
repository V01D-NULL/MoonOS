#ifndef MEMDEFS_H
#define MEMDEFS_H

#include <stdint.h>
#include "pmm.h"
#include <util/ptr.h>
#include <mm/cpu/CR.h>
#include <libk/kassert.h>
#include <ds/linked_list.h>

// Used by the buddy allocator
#define MM_MAX_ORDER 10 // Largest allocation: 2^10-1 => 0x200000 => 2MiB
#define PAGE_SIZE 4096

struct page
{
	int8_t order;
	void *ptr;
};

struct BuddyZone
{
	long *map;		   // Linearized binary tree
	struct slist list; // A list of buddy zones to span the entire memory for a given zone
};

struct zone
{
	// Vital information
	struct slist list;			   // A pointer to the next zone
	struct BuddyZone *buddy_zones; // A buddy zone is what stores the linear binary tree

	// Statistics
	size_t start;			// Base address for this zone
	size_t len;				// Length of this zone in bytes.
	size_t page_count;		// Number of pages for this zone.
	size_t num_buddy_zones; // Number of buddy zones for this zone
	int zone_nr;			// Note: zone.list->next will be 'zone_nr + 1'
	const char *name;
};

static const uintptr_t VMEM_DIRECT_MAPPING = 0x0; // Identity mapping
static const uintptr_t VMEM_LV5_BASE = 0xff00000000000000UL;
static const uintptr_t VMEM_LV4_BASE = 0xffff800000000000UL;
static const uintptr_t VMEM_CODE_BASE = 0xffffffff80000000;

STATIC_INLINE bool check_la57(void) { return (cr_read(CR4) >> 12) & 1; }

enum mapping_offset
{
	CODE,
	DATA
};

STATIC_INLINE size_t from_higher_half(size_t vaddr, enum mapping_offset off)
{
	assert(off == CODE || off == DATA);
	if (off == CODE)
		return vaddr - VMEM_CODE_BASE;

	return vaddr - (check_la57() ? VMEM_LV5_BASE : VMEM_LV4_BASE);
}

STATIC_INLINE size_t to_higher_half(size_t vaddr, enum mapping_offset off)
{
	assert(off == CODE || off == DATA);
	if (off == CODE)
		return vaddr + VMEM_CODE_BASE;

	return vaddr + (check_la57() ? VMEM_LV5_BASE : VMEM_LV4_BASE);
}

STATIC_INLINE bool is_page_aligned(void *addr) { return GENERIC_CAST(size_t, addr) % PAGE_SIZE == 0; }
STATIC_INLINE size_t get_page_count(void *addr, size_t n)
{
	size_t pagecount = 0;

	for (size_t i = 0; i < n; i += PAGE_SIZE)
	{
		if (is_page_aligned(addr))
		{
			pagecount++;
		}
	}

	return pagecount;
}

#endif // MEMDEFS_H