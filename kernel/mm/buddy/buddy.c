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
static bool __can_alloc(int order, struct BuddyZone *zone, int *node_idx);
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

// TODO: Mark all pages used by the bitmap allocator (pmm) as allocated!
struct page *buddy_alloc(int order)
{
	// 1. Traverse every memory zone
	list_foreach(zone, list, memory_zones)
	{
		// debug(true, "zone#%d (%s) [%d buddy zones]\n", zone->zone_nr, zone->name, zone->num_buddy_zones);
		// 2. Traverse every buddy zone in the current memory zone 'zone', if it exists.
		if (zone->buddy_zones != NULL)
		{
			list_foreach(buddy_zone, list, zone->buddy_zones)
			{
				int node_idx = 0;
				// Is this allocation possible?
				if (!__can_alloc(order, buddy_zone, &node_idx))
					continue;

				debug(false, "Allocation is possible (node_idx = %d)!\n", node_idx);

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

				else
				{
					debug(false, "Was able to allocate an order %d node (size: 0x%lX):\n"
								 "== zone ==\n"
								 "  start: 0x%lX | end: 0x%lX | num_buddy_zones: %ld\n"
								 "== buddy zone ==\n"
								 "  zone_nr: %ld\n",
						  order, ORDER_TO_SIZE(order),
						  zone->start, zone->start + zone->len, zone->num_buddy_zones,
						  buddy_zone->zone_nr);
					struct page *page = (struct page *)(zone->start * buddy_zone->zone_nr * node_idx);
					page->order = order;
					page->ptr = (uint64_t*)((zone->start * buddy_zone->zone_nr) + (ORDER_TO_SIZE(order) + sizeof(struct page)) * node_idx);

					return page;
				}
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
		// Note: The usage of pmm/slab_alloc is temporary, in the future I will likely modify
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
	if ((base + len) - base < max_allocation_size)
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

// Recursive preorder traversal
static void print_tree(struct BuddyZone *zone)
{
	int parent_index = 0;
	debug(false, "[0] %ld (Root)\n", __check_bit(zone->alloc_map, 0));

	for (int current_order = 1; current_order <= MM_MAX_ORDER - 1; current_order++)
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

// recursive preorder traversal (Output looks kinda messy imo)
void print(struct BuddyZone *zone, int node_idx, int order)
{
	if (node_idx >= 512 /* last order 9 node */ || !zone || node_idx == 511)
		return;

	int nodes_at_this_order = (1 << order) / 2;
	if (node_idx > nodes_at_this_order)
		order++;

	// indent(order);
	// debug(false, "(parent)node_idx: %d | order: %d\n", node_idx, order);
	indent(order);
	debug(false, "[L] ORDER=%d | child_node_idx: %d | parent_node_idx: %d\n", order, get_left_child_node(node_idx), node_idx);
	print(zone, get_left_child_node(node_idx), order);

	indent(order);
	debug(false, "[R] ORDER=%d | child_node_idx: %d | parent_node_idx: %d\n", order, get_right_child_node(node_idx), node_idx);
	print(zone, get_right_child_node(node_idx), order);
}

void foo()
{
	list_foreach(zone, list, memory_zones)
	{
		// debug(true, "zone#%d (%s) [%d buddy zones]\n", zone->zone_nr, zone->name, zone->num_buddy_zones);
		// 2. Traverse every buddy zone in the current memory zone 'zone', if it exists.
		if (zone->buddy_zones != NULL)
		{
			list_foreach(buddy_zone, list, zone->buddy_zones)
			{
				if (buddy_zone->zone_nr == 1)
				{
					print_tree(buddy_zone);
					// print(buddy_zone, 0, 0);
					return;
				}
			}
		}
	}
}

static bool __can_alloc(int order, struct BuddyZone *zone, int *node_idx)
{
	if (zone->is_full)
		return false;

	int parent = 0;					  // bit 0 == root node.
	int free_nodes[MM_MAX_ORDER - 1]; // The offsets in the bitmap of free nodes are stored here. MM_MAX_ORDER - 1 because the root node isn't stored here, so we only reserve space for 9 nodes instead of 10
	int node_i = 0;

	// Check if the root node can be allocated
	if (!__check_bit(zone->alloc_map, 0) && order == 0)
	{
		for (int current_order = 1; current_order <= MM_MAX_ORDER - 1; current_order++)
		{
			// 1 << current_order get's the number of nodes for the 'current_order'
			// It's divided by two because the Left and Right nodes are printed on the same line.
			for (int i = 0; i < (1 << current_order) / 2; i++)
			{
				// Retrieve the offset & value of the left and right node in the bitmap
				int left = get_left_child_node(parent);
				bool left_state_allocated = __check_bit(zone->alloc_map, left);

				int right = get_right_child_node(parent);
				bool right_state_allocated = __check_bit(zone->alloc_map, right);

				if (right_state_allocated || left_state_allocated)
					return false; // Tree is not empty

				parent++;
			}
		}
		zone->is_full = true;
		return true;
	}

	bool go_right_from_root = false;
	int current_order = 1;

	// Test: Allocate all nodes at order 2.
	// __set_bit(zone->alloc_map, 3);
	// __set_bit(zone->alloc_map, 4);
	// __set_bit(zone->alloc_map, 5);
	// __set_bit(zone->alloc_map, 6);

	// 1. Traverse the tree and find free nodes.
	// Always prefer the left node, if that fails it'll try the right node of the root node.
	while (current_order <= MM_MAX_ORDER - 1)
	{
		int left = get_left_child_node(parent);
		int right = get_right_child_node(parent);
		bool left_set = __check_bit(zone->alloc_map, left);
		bool right_set = __check_bit(zone->alloc_map, right);

		debug(true, "order: %d, left: %d, right: %d\n", current_order, left, right);

		// This should happen when the previous traversal to the left hand side of the tree fails (both child nodes are set)
		if (go_right_from_root && parent == 0)
		{
			debug(true, "Starting traversal from the right...\n");

			parent = right;
			current_order++;

			if (!right_set)
				free_nodes[node_i++] = right;
			else
				return false; // The left node couldn't handle the allocation request, the right node is reserved. Cannot serve this allocation request.

			continue;
		}

		if (!left_set)
		{
			free_nodes[node_i++] = left;
		}
		else if (!right_set)
		{
			free_nodes[node_i++] = right;
		}
		else
		{
			// The left and right hand side of the tree has been traversed unsuccessfully (no free nodes for this allocation request)
			if (go_right_from_root)
				return false;

			if (current_order == order)
				go_right_from_root = true;

			parent = node_i = 0;
			current_order = 1;
			continue;
		}

		parent = free_nodes[node_i - 1];
		current_order++;
	}

	// Step 1. Allocate the node by marking it as allocated (1) in the bitmap.
	int node = free_nodes[node_i - 1];
	__set_bit(zone->alloc_map, node); // Perform the allocation.
	debug(true, "Set: %d\n", node);

	// Converts a node offset in the tree (example: 511) to an offset of the node at order `n`, in this case 0.
	int normalized_node = node - (1 << order) + 2; // + 2 because buddy_alloc uses this to generate an offset in page->ptr, hence counting from 0 won't work
	*node_idx = normalized_node;

	// Traverse the tree from the bottom to the top and set the nodes accordingly
	for (int i = node_i - 1; i >= 0; i--)
	{
		// Step 2. Gather the parent and sibling node. If both children are allocated, the parent can be marked as allocated as well.
		node = free_nodes[i];

		parent = get_parent_of(node);
		int r = get_right_child_node(parent);
		int l = get_left_child_node(parent);
		bool lset = __check_bit(zone->alloc_map, l);
		bool rset = __check_bit(zone->alloc_map, r);
		if (lset && rset)
		{
			if (parent == 0)
				zone->is_full = true;
			debug(true, "lset && rset, set parent(%d)\n", parent);
			__set_bit(zone->alloc_map, parent);
		}
	}

	return true;
}