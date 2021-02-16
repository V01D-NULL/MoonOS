#ifndef KHEAP_H
#define KHEAP_H


#include "../common.h"
#include "../orderedArray.h"
#include "../drivers/screen/monitor.h"
#include "../multiboot.h"

#define KHEAP_START         0xC0000000  // arbitrary
#define KHEAP_INITIAL_SIZE  0x100000    // arbitrary

#define HEAP_INDEX_SIZE     0x20000     // arbitrary
#define HEAP_MAGIC          0xDEADBEEF  // unusual number that will stand out from others
#define HEAP_MIN_SIZE       0x70000     // arbitrary

extern u32int placement_address;

// Page align
u32int kmalloc_aligned ( u32int size );

// Return a physical address
u32int kmalloc_p ( u32int size, u32int *physical_address );

// Page align and return a physical address
u32int kmalloc_alignedp ( u32int size, u32int *physical_address );

// Vanilla
u32int kmalloc ( u32int size );

// Deallocate
void kfree ( void *p );


//
typedef struct
{
	u32int magic;    // Magic number, used for error checking and identification
	u8int  is_hole;  // 1 if hole, 0 if block
	u32int size;     // Size of block, including header(this) and footer
}
header_t;

typedef struct
{
	u32int   magic;    // Magic number, same as in header_t
	header_t *header;  // Pointer to block header
}
footer_t;


// Heap type (can have more than one heap)
typedef struct
{
	ordered_array_t index;          //
	u32int          start_address;  // The start of our allocated space
	u32int          end_address;    // The end of our allocated space. May be expanded up to max_address
	u32int          max_address;    // The maximum address the heap can be expanded to
	u8int           supervisor;     // Should extra pages requested by us be mapped as supervisor-only?
	u8int           readonly;       // Should extra pages requested by us be mapped as read-only?
}
heap_t;

// Create a new heap
heap_t *create_heap ( u32int start, u32int end, u32int max, u8int supervisor, u8int readonly );

// Allocates a contiguous region of memory 'size' in size.
// If page_align, creates that block starting on a page boundary.
void *alloc ( u32int size, u8int page_align, heap_t *heap );

// Releases a block allocated with 'alloc'
void free ( void *p, heap_t *heap );

//.text, .data, & .bss memory location
u32int get_section_text();
u32int get_section_data();
u32int get_section_bss();

//Current location in memory
u32int get_memory_offset();

//Complete physical memory map (provided by the GRUB bootloader)
u32int get_physical_memory_map(struct multiboot* mboot);

#endif  // KHEAP_H
