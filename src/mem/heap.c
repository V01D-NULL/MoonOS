// Kernel heap functions, also provides
// a placement malloc() for use before the heap is initialised

#include "paging.h"
#include "heap.h"
#include "../panic.h"

extern u32int __end;
u32int placement_address = ( u32int ) &__end;

extern u32int __code;
extern u32int __data;
extern u32int __bss;
u32int section_text = (u32int)&__code;
u32int section_data = (u32int)&__data;
u32int section_bss = (u32int)&__bss;

extern page_directory_t *kernel_directory;

heap_t *kheap = 0;


u32int kmalloc_ ( u32int size, int align, u32int *physical_address )
{
	/* This will eventually call malloc() on the kernel heap.
	   For now, though, we just assign memory at placement_address
	   and increment it by sz. Even when we've coded our kernel
	   heap, this will be useful for use before the heap is initialised
	*/

	// Pass calls to alloc if kheap != 0  ??
	if ( kheap != 0 )
	{
		void *address = alloc( size, ( u8int ) align, kheap );

		if ( physical_address != 0 )
		{
			page_t *page = get_page( ( u32int ) address, 0, kernel_directory );

			*physical_address = page->frame * PAGE_SIZE + ( ( u32int ) address & 0xFFF );
		}

		return ( u32int ) address;
	}

	//
	else
	{
		// Page align the address
		if ( align == 1 && ( ( placement_address & PAGE_ALIGN_MASK ) != 0 ) )
		{
			debug("kmalloc: Current address space: ( %x )\nkmalloc: Aligning address space.\n", placement_address);
			placement_address &= PAGE_ALIGN_MASK;
			placement_address += PAGE_SIZE;
			debug("kmalloc: Aligned address space.\nkmalloc: New, aligned address space: ( %x )\n", placement_address);
		}

		u32int allocated = placement_address;

		// Share physical address
		if ( physical_address )
		{
			*physical_address = allocated;
		}

		placement_address += size;
		debug("kmalloc: Allocated memory with a size of %d from %x through %x\n", size, placement_address, allocated);
		return allocated; 
	}
}

// Page align
u32int kmalloc_aligned ( u32int size )
{
	return kmalloc_( size, 1, 0 );
}

// Return a physical address
u32int kmalloc_p ( u32int size, u32int *physical_address )
{
	return kmalloc_( size, 0, physical_address );
}

// Page align and return a physical address
u32int kmalloc_alignedp ( u32int size, u32int *physical_address )
{
	return kmalloc_( size, 1, physical_address );
}

// Vanilla
u32int kmalloc ( u32int size )
{
	return kmalloc_( size, 0, 0 );
}

//
void kfree ( void *p )
{
    free( p, kheap );
}


// Find smallest hole that will fit a certain number of bytes
static s32int find_smallest_hole ( u32int size, u8int page_align, heap_t *heap )
{
	u32int i = 0;

	while ( i < heap -> index.size )
	{
		header_t *header = ( header_t * ) orderedArray_lookup( i, &heap -> index );

		// If the user has requested the memory be page-aligned
		if ( page_align > 0 )
		{
			/* Something about user wants page-align to start from part
			   have access to and not header */
			u32int location = ( u32int ) header;
			u32int hole_start = location + sizeof( header_t );
			s32int offset = 0;

			if ( ( hole_start & PAGE_ALIGN_MASK ) != 0 )
			{
				offset = PAGE_SIZE - ( hole_start % PAGE_SIZE );  // ? why mod
			}

			s32int hole_size = ( s32int ) header -> size - offset;

			// If can fit, break
			if ( hole_size >= ( s32int ) size )
			{
				break;
			}
		}

		// If can fit, break
		else if ( header -> size >= size )
		{
			break;
		}

		i += 1;
	}

	// Handle loop exit
	if ( i == heap -> index.size )
	{
		return - 1;  // got to the end and didn't find anything
	}
	else
	{
		return i;
	}
}

// Custom less_than function to sort by size
static s8int header_t_less_than ( void *a, void *b )
{
	return ( ( header_t * ) a ) -> size <
	       ( ( header_t * ) b ) -> size ? 1 : 0;
}

// Create a new heap
heap_t *create_heap ( u32int start, u32int end, u32int max, u8int supervisor, u8int readonly )
{
	monitor_write("Creating heap\n", true, false);

	//
	heap_t *heap = ( heap_t * ) kmalloc( sizeof( heap_t ) );

	// All our assumptions rely on startAddress and endAddress being page-aligned
	ASSERT( start % PAGE_SIZE == 0 );
	ASSERT(   end % PAGE_SIZE == 0 );

	// Initialize the index
	heap->index = orderedArray_place( ( void * ) start, HEAP_INDEX_SIZE, &header_t_less_than );

	// Shift the start address forward to resemble where we can start putting data
	start += sizeof( type_t ) * HEAP_INDEX_SIZE;

	// Make sure the start address is page-aligned
	if ( ( start & PAGE_ALIGN_MASK ) != 0 )
	{
		start &= PAGE_ALIGN_MASK;
		start += PAGE_SIZE;
	}

	// Write the start, end, and max addresses into the heap structure
	heap->start_address = start;
	heap->end_address   = end;
	heap->max_address   = max;
	heap->supervisor    = supervisor;
	heap->readonly      = readonly;

	// We start off with one large hole in the index
	header_t *hole = ( header_t * ) start;

	hole->size    = end - start;
	hole->magic   = HEAP_MAGIC;
	hole->is_hole = 1;

	orderedArray_insert( ( void * ) hole, &heap -> index );

	debug("create_heap: start: %x\ncreate_heap: end: %x\ncreate_heap: max: %x\ncreate_heap: supervisor: %d\ncreate_heap: readonly: %d\n",
	heap->start_address, heap->end_address, heap->max_address, heap->supervisor, heap->readonly);
	debug("create_heap: size: %x\ncreate_heap: magic: %x\ncreate_heap: is_hole: %d\n", hole->size, hole->magic, hole->is_hole);
	// kprintf("Heap:\nStart=>%x\nEnd=>%x\nMax=>%x\n", heap->start_address, heap->end_address, heap->max_address);
	return heap;
}

//
static void expand ( u32int new_size, heap_t *heap )
{
	u32int i;

	ASSERT( new_size > heap->end_address - heap->start_address );

	// Get the nearest following page boundary
	if ( ( new_size & PAGE_ALIGN_MASK ) != 0 )
	{
		new_size &= PAGE_ALIGN_MASK;
		new_size += PAGE_SIZE;
	}

	// Make sure we are not overreaching
	ASSERT( ( heap->start_address + new_size ) <= heap->max_address );

	// This should always be on a page boundary
	u32int old_size = heap->end_address - heap->start_address;

	// ?
	i = old_size;

	while ( i < new_size )
	{
		// ?
		alloc_frame(

			get_page( heap->start_address + i, 1, kernel_directory ),
			heap->supervisor ? 1 : 0,
			heap->readonly ? 0 : 1
		);

		i += PAGE_SIZE;
	}

	//
	heap -> end_address = heap -> start_address + new_size;
}

//
static u32int contract (u32int new_size, heap_t *heap)
{
	u32int i;

	ASSERT(new_size < heap->end_address - heap->start_address);

	// Get the nearest following page boundary
	if (new_size & PAGE_SIZE)
	{
		new_size &= PAGE_SIZE;
		new_size += PAGE_SIZE;
	}

	// Make sure we are not overreaching
	if (new_size < HEAP_MIN_SIZE)
	{
		new_size = HEAP_MIN_SIZE;
	}

	// This should always be on a page boundary
	u32int old_size = heap->end_address - heap->start_address;

	i = old_size - PAGE_SIZE;

	while (new_size < i)
	{
		free_frame(get_page(heap->start_address + i, 0, kernel_directory ));

		i -= PAGE_SIZE;
	}

	//
	heap->end_address = heap->start_address + new_size;

	return new_size;
}

// Allocates a contiguous region of memory 'size' in size.
// If page_align, creates that block starting on a page boundary.
void *alloc (u32int size, u8int page_align, heap_t *heap)
{
	// Make sure we take the size of header/footer into account
	u32int new_size = size + sizeof(header_t) + sizeof(footer_t);

	// Find smallest hole that wil fit
	s32int i = find_smallest_hole(new_size, page_align, heap);

	// Didn't find a suitable hole
	if (i == - 1)
	{
		// Save some previous data
		u32int old_length = heap->end_address - heap->start_address;
		u32int old_end_address = heap->end_address;

		// We need to allocate more space
		expand(old_length + new_size, heap);

		u32int new_length = heap->end_address - heap->start_address;

		// Find the endmost header
		i = 0;

		u32int idx = - 1;
		u32int value = 0;

		while (i < heap->index.size)
		{
			u32int tmp = (u32int) orderedArray_lookup(i, &heap->index);

			if (tmp > value)
			{
				value = tmp;
				idx = i;
			}

			i += 1;
		}

		// If we didn't find any headers, we need to add one
		if (idx == - 1)
		{
			//
			header_t *header = (header_t *) old_end_address;

			header->size    = new_length - old_length;
			header->magic   = HEAP_MAGIC;
			header->is_hole = 1;

			//
			footer_t *footer = (footer_t *) (old_end_address + header->size - sizeof(footer_t));

			footer->header = header;
			footer->magic  = HEAP_MAGIC;

			//
			orderedArray_insert((void *) header, &heap->index);
		}

		//
		else
		{
			// The last header needs adjusting
			header_t *header = orderedArray_lookup( idx, &heap->index );

			header -> size += new_length - old_length;

			// Rewrite the footer
			footer_t *footer = ( footer_t * ) ( ( u32int ) header + header -> size - sizeof( footer_t ) );

			footer -> header = header;
			footer -> magic  = HEAP_MAGIC;
		}

		// We now have enough space. Recurse, and call the function again
		return alloc( size, page_align, heap );
	}

	// Get header pointer from index given to us by find_smallest_hole ??
	header_t *orig_hole_header = ( header_t * ) orderedArray_lookup( i, &heap -> index );
	u32int    orig_hole_pos    = ( u32int ) orig_hole_header;
	u32int    orig_hole_size   = orig_hole_header -> size;

	// If the leftover is less than the overhead for adding a new hole,
	u32int leftover = orig_hole_size - new_size;
	if ( leftover < sizeof( header_t ) + sizeof( footer_t ) )
	{
		// Then just increase the requested size to the size of the hole we found
		size += leftover;

		new_size = orig_hole_size;
	}

	// If we need to page-align the data, do it now and make a new hole in front of our block
	if ( page_align && ( ( orig_hole_pos & PAGE_ALIGN_MASK ) != 0 ) )
	{
		u32int size2, footer_pos;

		//
		size2 = PAGE_SIZE - ( orig_hole_pos & 0xFFF ) - sizeof( header_t );  // ??

		//
		header_t *hole_header = ( header_t * ) orig_hole_pos;

		hole_header -> size    = size2;  // ??
		hole_header -> magic   = HEAP_MAGIC;
		hole_header -> is_hole = 1;

		//
		footer_pos = orig_hole_pos + size2 - sizeof( footer_t );  // ??

		//
		footer_t *hole_footer = ( footer_t * ) ( footer_pos );

		hole_footer -> header = hole_header;
		hole_footer -> magic  = HEAP_MAGIC;

		//
		orig_hole_pos += size2;
		orig_hole_size -= size2;
	}

	// Else don't need this hole any more, delete it from index
	else
	{
		orderedArray_remove( i, &heap -> index );
	}

	// Overwrite original header and footer
	header_t *block_header = ( header_t * ) orig_hole_pos;

	block_header -> size    = new_size;
	block_header -> magic   = HEAP_MAGIC;
	block_header -> is_hole = 0;

	footer_t *block_footer = ( footer_t * ) ( orig_hole_pos + sizeof( header_t ) + size );

	block_footer -> header = block_header;
	block_footer -> magic  = HEAP_MAGIC;

	// We may need to write a new hole after the allocated block.
	// We do this only if the new hole would have positive size?
	if ( orig_hole_size - new_size > 0 )
	{
		header_t *hole_header = ( header_t * ) ( orig_hole_pos + sizeof( header_t ) + size + sizeof( footer_t ) );

		hole_header -> size    = orig_hole_size - new_size;
		hole_header -> magic   = HEAP_MAGIC;
		hole_header -> is_hole = 1;

		footer_t *hole_footer = ( footer_t * ) ( ( u32int ) hole_header + orig_hole_size - new_size - sizeof( footer_t ) );  // ?

		if ( ( u32int ) hole_footer < heap -> end_address )
		{
			hole_footer -> header = hole_header;
			hole_footer -> magic  = HEAP_MAGIC;
		}

		// Put the new hole in the index;
		orderedArray_insert( ( void * ) hole_header, &heap -> index );
	}

	//
	return ( void * ) ( ( u32int ) block_header + sizeof( header_t ) );
}

// Releases a block allocated with 'alloc'
void free ( void *p, heap_t *heap )
{
	u32int i;

	// Exit gracefully for null pointers
	if ( p == 0 )
	{
		return;
	}

	// Get the header and footer associated with the pointer
	header_t *header = ( header_t * ) ( ( u32int ) p - sizeof( header_t ) );
	footer_t *footer = ( footer_t * ) ( ( u32int ) header + header -> size - sizeof( footer_t ) );

	//
	ASSERT( header->magic == HEAP_MAGIC );
	ASSERT( footer->magic == HEAP_MAGIC );
	
	// Make us a hole
	header->is_hole = 1;

	// Do we want to add this header into the 'free-holes' index
	char do_add = 1;

	// Unify left
	// If the thing immediately to the left of us is a footer...
	footer_t *test_footer = ( footer_t * ) ( ( u32int ) header - sizeof( footer_t ) );

	if ( test_footer -> magic == HEAP_MAGIC &&
	     test_footer -> header -> is_hole == 1 )
	{
		u32int cache_size = header->size;  // Cache our current size

		header = test_footer -> header;    // Rewrite our header with the new one
		footer -> header = header;         // Rewrite our footer to point to the new header

		header -> size += cache_size;      // Change the header size

		do_add = 0;                        // Since this header is already in the index, we don't want to add it again
	}

	// Unify right
	// If the thing immediately to the right of us is a header...
	
	header_t *test_header = ( header_t * ) ( ( u32int )footer + sizeof( footer_t ) );
	
	if ( test_header -> magic == HEAP_MAGIC && test_header -> is_hole )
	{
		// Increase our size
		header -> size += test_header -> size;
		
		// Rewrite it's footer to point to our header
		test_footer = ( footer_t * ) ( ( u32int )test_header + test_header -> size - sizeof( footer_t ) );
		footer = test_footer;

		// Find and remove this header from the index
		i = 0;
		while ( ( i < heap -> index.size ) &&
		        ( orderedArray_lookup( i, &heap -> index ) != ( void * ) test_header ) )
		{
			i += 1;
		}

		// Make sure we actually found the item
		ASSERT( i < heap -> index.size );

		// Remove it
		orderedArray_remove( i, &heap -> index );
	}

	// If the footer location is the end address, we can contract
	if ( ( u32int ) footer + sizeof( footer_t ) == heap -> end_address )
	{
		u32int old_length = heap -> end_address - heap -> start_address;

		u32int new_length = contract( ( u32int ) header - heap -> start_address, heap );

		// Check how big we will be after resizing

		// We will still exist, so resize us
		if ( header -> size - ( old_length - new_length ) > 0 )
		{
			header -> size -= old_length - new_length;

			footer = ( footer_t * ) ( ( u32int ) header + header -> size - sizeof( footer_t ) );

			footer -> header = header;
			footer -> magic  = HEAP_MAGIC;
		}
		// We will no longer exist. Remove from index
		else
		{
			//
			while ( ( i < heap -> index.size ) &&
			        ( orderedArray_lookup( i, &heap -> index ) != ( void * ) test_header ) )
			{
				i += 1;
			}

			// If we didn't find ourselves, we have nothing to remove
			if ( i < heap -> index.size )
			{
				orderedArray_remove( i, &heap -> index );
			}
		}
	}

	//
	if ( do_add == 1 )
	{
		orderedArray_insert( ( void * ) header, &heap -> index );
	}
}

// //Get a layout of the physical memory with the help of GRUB
u32int get_physical_memory_map(struct multiboot* mboot)
{
    int tmp = mboot->flags; //Check bit 6 of this value
    int is_bit_set = CHECK_BIT(tmp, 6);
    if (!is_bit_set) { ASSERT("The 6th bit of 'flags' isn't set. Aborting\n"); return -1; }
    
    u32int memory_map = mboot->mmap_addr;
    
    return memory_map;
}

u32int get_memory_offset()
{
    return placement_address;
}

//.text
u32int get_section_text() {
    return section_text;
}

//.data
u32int get_section_data() {
    return section_data;
}

//.bss
u32int get_section_bss() {
    return section_bss;
}