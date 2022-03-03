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
		tree_size += num_trees_for_this_zone * (sizeof(long) * 8); // sizeof(long) * 8 to get the number of bits.
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
	// 			debug(true, "INIT: %d\n", buddy_zone->zone_nr);
	// 			// print_tree(buddy_zone);
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
		debug(true, "zone#%d (%s) [%d buddy zones]\n", zone->zone_nr, zone->name, zone->num_buddy_zones);
		// 2. Traverse every buddy zone in the current memory zone 'zone', if it exists.
		if (zone->buddy_zones != NULL)
		{
			list_foreach(buddy_zone, list, zone->buddy_zones)
			{			
				// Is this allocation possible?
				if (!verify_allocation(order, buddy_zone))
					continue;

				debug(false, "Allocation is possible!\n");

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

					struct page *page = (struct page *)(zone->start * buddy_zone->zone_nr);
					page->order = order;
					page->ptr = (uint64_t *)((zone->start * buddy_zone->zone_nr) + sizeof(struct page));
					return page;
				}

				// for (;;) // Debugging..
				// 	;
				// return NULL;
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
	if (zone->num_buddy_zones == 0)
		return zone;
	
	struct BuddyZone *tail = NULL;

	// Setup buddy zone(s)
	for (size_t i = 0; i < zone->num_buddy_zones; i++)
	{
		// Note: The usage of pmm_alloc is temporary, in the future I will likely modify
		// Note: The highest order will be used since memory blocks have to be split as needed,
		// there is no point in splitting them now for no reason
		struct BuddyZone *buddy = (struct BuddyZone *)from_higher_half(pmm_alloc(), DATA);
		if (buddy == NULL)
			panic("OOM while initializing buddy");
		
		buddy->alloc_map = slab_alloc(sizeof(long));
		buddy->is_full = false;
		buddy->zone_nr = i + 1; // Don't start counting at zero since we should not multiply anything by zero in the 'BUDDY_ZONE_TO_PTR' macro

		if (!tail)
			zone->buddy_zones = buddy;
		else
			list_set_next(buddy, list, tail);

		tail = buddy;
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
	int parent_index = 0;
	debug(false, "[0] %ld (Root)\n", __check_bit(zone->alloc_map, 0));

	for (int current_order = 1; current_order < MM_MAX_ORDER - 1; current_order++)
	{
		// 1 << current_order get's the number of nodes for the 'current_order'
		// It's divided by two because the Left and Right nodes are printed on the same line.
		for (int i = 0; i < (1 << current_order) / 2; i++)
		{
			indent(current_order);

			// Retrieve the offset & value of the left and right node in the bitmap
			int left = get_left_child_node(parent_index);
			bool left_state_allocated = __check_bit(zone->alloc_map, left);

			int right = get_right_child_node(parent_index);
			bool right_state_allocated = __check_bit(zone->alloc_map, right);

			// indent(current_order);
			debug(false, "[%d] (L-%d) %d  -- (R-%d) %d\n", current_order, left, left_state_allocated, right, right_state_allocated);

			parent_index++;
		}
	}
}

// For some reason there are some rouge bits in random zones that are always set, everything else is fine
// so just run it through this function to clear every single bit when the buddy zone is initialized- cpu's are fast, right? right?
static void traverse_and_patch_tree_on_init(struct BuddyZone *zone)
{
	int index = 0;
	__clear_bit(zone->alloc_map, index); // Root node

	for (int current_order = 1; current_order <= MM_MAX_ORDER - 1; current_order++)
	{
		for (int i = 0; i < (1 << current_order) / 2; i++)
		{
			// Retrieve the offset of the left and right node in the bitmap
			int left = get_left_child_node(index);
			__clear_bit(zone->alloc_map, left);

			int right = get_right_child_node(index);
			__clear_bit(zone->alloc_map, right);
			index++;
		}
	}
}

struct recursive_check_output
{
	bool free_node;
	int index;
};

static struct recursive_check_output recursive_check(long *map, bool prefer_left, int index)
{
	if (prefer_left)
	{
		// Is the left node free?
		int left = get_left_child_node(index);
		bool left_bit_set = __check_bit(map, left);

		// Left node is free, advance to the next order
		if (!left_bit_set)
		{
			return (struct recursive_check_output){true, left};
		}

		// Ok, maybe the right node is free..
		int right = get_right_child_node(index);
		bool right_bit_set = __check_bit(map, right);

		// Yep, it's free.
		if (!right_bit_set)
		{
			return (struct recursive_check_output){true, right};
		}

		return (struct recursive_check_output){false, 0};
	}

	int right = get_right_child_node(index);
	bool right_bit_set = __check_bit(map, right);

	if (!right_bit_set)
	{
		return (struct recursive_check_output){true, right};
	}

	int left = get_left_child_node(index);
	bool left_bit_set = __check_bit(map, left);

	if (!left_bit_set)
	{
		return (struct recursive_check_output){true, left};
	}

	return (struct recursive_check_output){false, 0}; // Both the left and right child nodes are reserved. Either start again and go to the right or quit.
}

// Check if an allocation at order 'order' for the buddy zone 'zone' is allowed
static bool verify_allocation(int order, struct BuddyZone *zone)
{
	bool allocation_allowed = true;

	if (zone->is_full)
		return !allocation_allowed;

	int index = 0;
	bool root_bit_set = __check_bit(zone->alloc_map, 0);

	// The root bit has not been set, that means this entire tree may be allocatable.
	if (!root_bit_set && order == 0)
	{
		// Is the tree completely empty?
		for (int current_order = 1, parent = 0; current_order < MM_MAX_ORDER; current_order++)
		{
			for (int i = 0; i < (1 << current_order) / 2; i++, parent++)
			{
				bool left = __check_bit(zone->alloc_map, get_left_child_node(parent));
				bool right = __check_bit(zone->alloc_map, get_right_child_node(parent));

				// The tree is not empty, cannot fullfill this request.
				if (left || right)
				{
					debug(true, "Allocation at order 0 is not possible: Tree not empty.\n");
					return !allocation_allowed;
				}
			}
		}
		__set_bit(zone->alloc_map, 0);
		zone->is_full = true;
		return allocation_allowed;
	}
	else
	{
		return !allocation_allowed; // Attempted to allocate order 0 node, but it's already marked as allocated.
	}

	bool root_prefer_left = true; // Left nodes are checked first. If that's not an option this bool is set to false and the the root checks the right.
	int node_offsets[MM_MAX_ORDER - 1];
	int node_i = 0; // Index into node_offsets.

	// This code tests: Allocate all order 2 nodes and see if the traversal continues. (It should fail)
	// __set_bit(zone->alloc_map, 3);
	// __set_bit(zone->alloc_map, 4);
	// __set_bit(zone->alloc_map, 5);
	// __set_bit(zone->alloc_map, 6);

	int check_this_order = 2; // When the left node didn't work, try the left node at order 1, but go to the right. If that doesn't work try the right node at order 1
	for (int current_order = 1; current_order <= MM_MAX_ORDER - 1; current_order++)
	{
		struct recursive_check_output out = recursive_check(zone->alloc_map, root_prefer_left, index);

		// Both nodes are allocated...
		if (!out.free_node && !__check_bit(zone->alloc_map, get_sibling_node(index)))
		{
			// We checked both the right AND left nodes from the root, the allocation cannot be made from this zone
			if (!root_prefer_left)
			{
				debug(true, "Cannot allocate a node at order %d (failed at order %d on both the left and right nodes of the root)\n", order, current_order);
				return !allocation_allowed;
			}

			debug(true, "both nodes (L-%d, R-%d) at order %d are allocated, trying the right node from the root...\n", get_left_child_node(index), get_right_child_node(index), current_order);
			root_prefer_left = false;
			node_i = 0;
			index = 0;
			current_order = 1; // Restart the loop.
		}
		else
		{
			if (!out.free_node)
				out.index = get_sibling_node(index); // The sibling node is free, let's use that one and continue.
			
			debug(true, "current_order: %d | (node is free) | node_index: %d\n", current_order, out.index);
			index = out.index;
			node_offsets[node_i++] = out.index; // Todo: Do I need to record the node offsets after the desired order has been reached?
		}
	}

	// Perform the allocation
	__set_bit(zone->alloc_map, node_offsets[order]);
	
	// Traverse the tree from the target order to the top and set all bits(nodes) accordingly.
	for (int current_order = order; current_order >= 0; current_order--)
	{
		int original_node = node_offsets[current_order];

		int parent_node = get_parent_of(original_node);
		__set_bit(zone->alloc_map, parent_node);
		debug(true, "parent node of %d: %d\n", original_node, parent_node);
		// debug(true, "2. [%d]: P: %d, L: %d, R: %d\n", current_order, parent_node, left_node, right_node);
	}

	debug(true, "allocation_allowed: %d\n", allocation_allowed);
	// print_tree(zone);
	// for (;;)
	// 	;

	return allocation_allowed;
}