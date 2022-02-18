#include <mm/buddy/buddy.h>
#include <devices/serial/serial.h>
#include <mm/pmm.h>
#include <mm/slab.h>
#include <util/common.h>
#include <printk.h>
#include <stdint.h>
#include <panic.h>

#define NODE_SPLIT 1

static struct zone *memory_zones;

static struct zone *init_zone(size_t base, size_t length, int zone_nr, int type);
static size_t how_many_buddy_zones(size_t base, size_t len);

void buddy_init(struct stivale2_mmap_entry *mmap, int entries)
{
	typeof(memory_zones) tail = NULL;
	int zone_nr = 0;

	for (int i = 0; i < entries; i++)
	{
		if (mmap[i].type == STIVALE2_MMAP_RESERVED)
			continue;
		
		struct zone *zone = init_zone(mmap[i].base, mmap[i].length, zone_nr++, mmap[i].type);
		
		if (!tail)
			memory_zones = zone;
		else
			list_set_next(zone, list, tail);
		tail = zone;
	}

	// debug(true, "Displaying memory zones...\n");
	list_foreach(zone, list, memory_zones)
	{
		debug(false, "Zone #%d (%s)\n", zone->zone_nr, zone->name);
		if (zone->buddy_zones)
		{
			// debug(false, "%p\n", zone->buddy_zones->list.next->next);
			list_foreach(buddy_zone, list, zone->buddy_zones)
				debug(false, "map: %d\n", buddy_zone->map);
		}
	}
	// 	debug(false, "=== Zone #%d ===\n(%s)\nbase: %p | top: %p\n\n", zone->zone_nr, zone->name, zone->start, zone->start + zone->len);
}

void buddy_free(struct page *page)
{
}

// TODO: Mark all pages used by the bitmap allocator as allocated!
struct page *buddy_alloc(int order)
{
	return NULL;
}

static struct zone *init_zone(size_t base, size_t length, int zone_nr, int type)
{
	struct zone *zone = (struct zone *)slab_alloc(sizeof(struct zone));
	if (!zone)
	{
		printk("", "Failed to allocate ~%ld bytes for a zone structure", sizeof(struct zone));
		__asm__("hlt");
	}

	zone->start = base;
	zone->len = length;
	zone->name = get_mmap_type(type);
	zone->zone_nr = zone_nr;
	zone->page_count = (base + length) / PAGE_SIZE;
	zone->num_buddy_zones = 0;

	// Just focus on usable memory for now. I'll implement other things like kernel and module memory later.
	if (type != STIVALE2_MMAP_USABLE)
		return zone;

	zone->num_buddy_zones = how_many_buddy_zones(zone->start, zone->len);

	// typeof (memory_zones) tail = NULL;
	// struct zone *zone = init_zone(base, length, zone_nr++, type);
	// if (!tail)
	// 	memory_zones = zone;
	// else
	// 	list_set_next(zone, list, tail);
	// tail = zone;

	struct BuddyZone *tail = NULL;

	// Setup buddy zone(s)
	for (size_t i = 0; i < zone->num_buddy_zones; i++)
	{
		// Note: The highest order will be used since memory blocks have to be split as needed,
		// there is no point in splitting them now for no reason
		struct BuddyZone *buddy = (struct BuddyZone *)slab_alloc(sizeof(struct BuddyZone));
		buddy->map = !NODE_SPLIT;

		if (!tail)
			zone->buddy_zones = buddy;
		else
			list_set_next(buddy, list, tail);
		tail = buddy;
		// list_set_next(block, freelist, (struct BuddyZone*)page);// zone->buddy_zone.freelist
		// zone->blocks.freelist[MM_MAX_ORDER]
	}

	return zone;
}

// TODO: Store any memory that won't fit in a buddy zone in a 'struct zone' and either try to add a new buddy zone after setting up the normal ones or give this memory to some other allocator
// Determine how many items 'zone.buddy_zones.list' needs to cover the entirety of the zone's memory.
static size_t how_many_buddy_zones(size_t base, size_t len)
{
	size_t max_allocation_size = (1 << (MM_MAX_ORDER - 1)) * PAGE_SIZE;
	size_t num_buddy_zones = 0;

	for (int ptr = base; ptr < base + len; ptr += max_allocation_size)
		num_buddy_zones++;

	return num_buddy_zones;
}