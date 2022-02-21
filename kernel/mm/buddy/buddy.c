#include <mm/buddy/buddy.h>
#include <mm/buddy/buddy_utils.h>
#include <devices/serial/serial.h>
#include <mm/pmm.h>
#include <mm/slab.h>
#include <util/common.h>
#include <printk.h>
#include <stdint.h>
#include <panic.h>

#define NODE_FREE 1

static struct zone *memory_zones;

static void traverse_and_patch_tree_on_init(struct BuddyZone *zone);
static struct zone *init_zone(size_t base, size_t length, int zone_nr, int type);
static size_t how_many_buddy_zones(size_t base, size_t len);
static void traverse_tree(struct BuddyZone *zone);

void buddy_init(struct stivale2_mmap_entry *mmap, int entries)
{
	// Step 1: Calculate the size of the tree
	// TODO: Preallocate some memory of size 'tree_size' bytes and use that instead of pmm_alloc
	size_t tree_size = 0;
	for (int i = 0; i < entries; i++)
	{
		if (mmap[i].type != STIVALE2_MMAP_USABLE)
			continue;

		size_t num_trees_for_this_zone = how_many_buddy_zones(mmap[i].base, mmap[i].length);
		tree_size += num_trees_for_this_zone * sizeof(long) * 8; // The binary tree bitmap is a long*, aka 64 bits.
	}
	debug(true, "Tree size: %llx (%lld)\n", tree_size, tree_size);

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

	debug(true, "Displaying memory zones...\n");
	list_foreach(zone, list, memory_zones)
	{
		debug(false, "Zone #%d (%s)\n", zone->zone_nr, zone->name);
		if (zone->buddy_zones != NULL)
		{
			list_foreach(buddy_zone, list, zone->buddy_zones)
				traverse_and_patch_tree_on_init(buddy_zone);
		}
	}

	// Parse tree for debugging purposes.
	// list_foreach(zone, list, memory_zones)
	// {
	// 	debug(false, "Zone #%d (%s)\n", zone->zone_nr, zone->name);
	// 	if (zone->buddy_zones != NULL)
	// 	{
	// 		list_foreach(buddy_zone, list, zone->buddy_zones)
	// 			traverse_tree(buddy_zone);
	// 	}
	// }
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
	struct BuddyZone *tail = NULL;

	// Setup buddy zone(s)
	for (size_t i = 0; i < zone->num_buddy_zones; i++)
	{
		// Note: The usage of pmm_alloc is temporary, in the future I will likely modify 
		// Note: The highest order will be used since memory blocks have to be split as needed,
		// there is no point in splitting them now for no reason
		struct BuddyZone *buddy = (struct BuddyZone *)pmm_alloc();
		buddy->map = pmm_alloc();

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

	for (size_t ptr = base; ptr < base + len; ptr += max_allocation_size)
		num_buddy_zones++;

	return num_buddy_zones;
}

static void indent(int n)
{
	for (int i = 0; i < n; i++)
		debug(false, " ");
}

static void traverse_tree(struct BuddyZone *zone)
{
	int index = 0;
	debug(false, "[0] %ld (Root)\n", __check_bit(zone->map, 0));

	for (int current_order = 1; current_order < MM_MAX_ORDER - 1; current_order++)
	{
		// 1 << current_order get's the number of nodes for the 'current_order'
		// It's divided by two because the Left and Right nodes are printed on the same line.
		for (int i = 0; i < (1 << current_order) / 2; i++)
		{
			indent(current_order);

			// Retrieve the offset & value of the left and right node in the bitmap
			int left = get_left_child_node(index);
			__check_bit(zone->map, left);
			// debug(false, "index: %d | left: %d\n", index, left);
			bool left_state = __check_bit(zone->map, left);
			// debug(false, "%d, %d\n", index, __check_bit(zone->map, 1));

			int right = get_right_child_node(index);
			__check_bit(zone->map, right);
			// debug(false, "index: %d | right: %d\n", index, right);
			bool right_state = __check_bit(zone->map, right);

			debug(false, "[%d] (L) %d -- (R) %d\n", current_order, left_state, right_state);
			index += 2;
		}
	}
}

// For some reason there are two rouge bits at the second and third right nodes at order 8 that are always set, everything else is fine
// so just run it through this function to clear every single bit when the buddy zone is initialized- cpu's are fast, right? right?
static void traverse_and_patch_tree_on_init(struct BuddyZone *zone)
{
	int index = 0;
	for (int current_order = 1; current_order < MM_MAX_ORDER - 1; current_order++)
	{
		for (int i = 0; i < (1 << current_order) / 2; i++)
		{
			// Retrieve the offset of the left and right node in the bitmap
			int left = get_left_child_node(index);
			__clear_bit(zone->map, left);

			int right = get_right_child_node(index);
			__clear_bit(zone->map, right);
			index += 2;
		}
	}
}