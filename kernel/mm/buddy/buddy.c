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
#define BUDDY_ZONE_TO_PTR(zone, buddy_zone, order) (zone->start + ORDER_TO_SIZE(order)) * buddy_zone->zone_nr

static struct zone *memory_zones;

static void traverse_and_patch_tree_on_init(struct BuddyZone *zone);
static bool verify_allocation(int order, struct BuddyZone *zone);
static struct zone *init_zone(size_t base, size_t length, int zone_nr, int type);
static size_t how_many_buddy_zones(size_t base, size_t len);
static void print_tree(struct BuddyZone *zone);

void buddy_init(struct stivale2_mmap_entry *mmap, int entries)
{
	// Step 1: Calculate the size of all binary trees
	// TODO: Preallocate some memory of size 'tree_size' bytes and use that instead of pmm_alloc
	size_t tree_size = 0;
	for (int i = 0; i < entries; i++)
	{
		if (mmap[i].type != STIVALE2_MMAP_USABLE)
			continue;

		size_t num_trees_for_this_zone = how_many_buddy_zones(mmap[i].base, mmap[i].length);
		tree_size += num_trees_for_this_zone * sizeof(long) * 8; // The binary tree bitmap is a long*, aka 64 bits.
	}
	debug(true, "Size of all binary trees: %ld KiB\n", tree_size / 1024);


	// Step 2: Create and init the zones and buddy zones
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
		if (zone->buddy_zones != NULL)
		{
			debug(false, "Zone #%d (%s) [%d buddy zones] -- [0x%llX - 0x%llX]\n", zone->zone_nr, zone->name, zone->num_buddy_zones, zone->start, zone->start + zone->len);
			list_foreach(buddy_zone, list, zone->buddy_zones)
				traverse_and_patch_tree_on_init(buddy_zone);
		}
		else
		{
			debug(false, "Zone #%d (%s)\n", zone->zone_nr, zone->name);
		}
	}

	// Parse tree for debugging purposes.
	// list_foreach(zone, list, memory_zones)
	// {
	// 	debug(false, "Zone #%d (%s)\n", zone->zone_nr, zone->name);
	// 	if (zone->buddy_zones != NULL)
	// 	{
	// 		list_foreach(buddy_zone, list, zone->buddy_zones)
	// 			print_tree(buddy_zone);
	// 	}
	// }
}

void buddy_free(struct page *page)
{
}

// TODO: Mark all pages used by the bitmap allocator as allocated!
struct page *buddy_alloc(int order)
{
	// 1. Traverse every memory zone
	list_foreach(zone, list, memory_zones)
	{
		// 2. Traverse every buddy zone in the current memory zone 'zone', if it exists.
		if (zone->buddy_zones != NULL)
		{
			list_foreach(buddy_zone, list, zone->buddy_zones)
			{
				// Is this allocation possible?
				if (!verify_allocation(order, buddy_zone))
					// continue;
					for (;;) // Debugging..
						;

				debug(false, "Allocation is possible!\n");

				// The root node was not set, meaning the entire tree (2MiB) can be allocated.
				// Note: It is safe to check for is_full here because verify_allocation ensures that this
				// buddy_zone safe to allocate from, so there is no need to worry about allocating the same memory.
				if (buddy_zone->is_full)
				{
					debug(false, "Was able to allocate full tree:\n"
								 "== zone ==\n"
								 "  start: 0x%lX | end: 0x%lX | num_buddy_zones: %ld\n"
								 "== buddy zone ==\n"
								 "  zone_nr: %ld\n",
						  zone->start, zone->start + zone->len, zone->num_buddy_zones,
						  buddy_zone->zone_nr);
					
					struct page *page = (struct page*)(zone->start * buddy_zone->zone_nr);
					page->order = order;
					page->ptr = (uint64_t*)((zone->start * buddy_zone->zone_nr) + sizeof(struct page));
					return page;
				}


				for (;;) // Debugging..
					;
				return NULL;
			}
		}
	}
	return NULL; // Failed to serve memory request.
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
		buddy->is_full = false;
		buddy->zone_nr = i + 1; // Don't start counting at zero since we should not multiply anything by zero in the 'BUDDY_ZONE_TO_PTR' macro

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

	// Anything < 2MiB is unable to be used by the buddy allocator
	if ((base + len) - base < 0x200000)
	{
		return num_buddy_zones; // 0
	}

	// Todo: Should I modify the zone's len property to exclude any memory that won't fit the buddy to avoid confusion?
	// It won't affect the buddy since zone->start/len is only used after finding a free node, but it might be a little confusing...
	for (size_t ptr = base; ptr < base + len; ptr += max_allocation_size)
		num_buddy_zones++;

	return num_buddy_zones;
}

static void indent(int n)
{
	for (int i = 0; i < n; i++)
		debug(false, " ");
}

static void print_tree(struct BuddyZone *zone)
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

// Check if an allocation at order 'order' for the buddy zone 'zone' is allowed
static bool verify_allocation(int order, struct BuddyZone *zone)
{
	bool allocation_allowed = true;

	if (zone->is_full)
		return !allocation_allowed;

	int index = 0;
	bool root_bit_set = __check_bit(zone->map, 0);

	// The root bit has not been set, that means this node is allocatable!
	if (!root_bit_set && order == 0)
	{
		debug(true, "Root node is unset- the entire tree can be allocated!\n");
		__set_bit(zone->map, 0);
		zone->is_full = true;
		return allocation_allowed;
	}

	// Todo: This is untested and probably doesn't work. Allocating a free root node is the only thing that works atm
	// Todo: After parsing the tree from top to bottom to determine a free node at a given order (if any), traverse the
	// tree from the bottom to the top and set the nodes accordingly.
	for (int current_order = 1; current_order < MM_MAX_ORDER - 1; current_order++)
	{
		for (int i = 0; i < (1 << current_order) / 2; i++)
		{
			// Is the left node free?
			int left = get_left_child_node(index);
			bool left_bit_set = __check_bit(zone->map, left);

			// Left node is free, advance to the next order
			if (!left_bit_set)
			{
				debug(true, "left node @order=%d, target_order=%d ok!\n", current_order, order);
				index += 2;
				break;
			}

			// Ok, maybe the right node is free..
			int right = get_right_child_node(index);
			bool right_bit_set = __check_bit(zone->map, right);

			// Yep, it's free.
			if (!right_bit_set)
			{
				debug(true, "right node @order=%d, target_order=%d ok!\n", current_order, order);
				index += 2;
				break;
			}

			allocation_allowed = false;
			break;
		}
	}

	debug(true, "allocation_allowed: %d\n", allocation_allowed);
	return allocation_allowed;
}