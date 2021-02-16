#include "paging.h"
#include "heap.h"
#include "../panic.h"
/*
	32bit address space
	4Kb page size

	Page table entry
		31..12  frame address in physical memory
		 11..9  unused available
		  8..7  reserved by CPU?
		     6  dirty
		     5  accessed
		  4..3  reserved by CPU
		     2  u/s
		     1  user mode read-only/writeable
		     0  present in memory

	Page fault
		. reading from/writing to area of memory that is not mapped
		. user-mode tries to write read-only page
		. user-mode tries to access kernel-only page
		. page table entry corrupted (reserved bits overwritten)

		interrupt no 14
		error code (nibble):
			4  fault occurred during instruction fetch
			3  caused by reserved bits being overwritten
			2  processor was running in user-mode
			1  operation that caused fault was write, else read
			0  fault not due to page not being present. If unset, page wasn't present

	Control registers
		CR0 - if set, bit 31 enables paging
		CR2 - when a page fault occurs, the address the program attempted to access is stored here
		CR3 - stores physical address of first page directory entry?

	Multilevel
		One page directory with 1024 entries (page table base addresses)
		\
		 --> 1024 page tables, each with 1024 entries (page base addresses)
		     \
		      --> 1024 x 1024 pages, each with 4096 entries (physical address assoc w virtual)

		2^10 x 2^10 x 2^12 = 2^32 addresses mapped

		Using multilevel means don't need to store unused page tables and
		associated pages in fast memory

	What is placement address?

	Uses bitmap to allocate pages
*/


// Macros for bit manipulation
#define  INDEX_FROM_BIT( a ) ( a / NBITS )
#define OFFSET_FROM_BIT( a ) ( a % NBITS )

// Defined in kheap.c
extern u32int placement_address;
extern heap_t *kheap;

u32int *frames;  // A bitmap of frames
u32int nframes;

// The kernel's page directory
page_directory_t *kernel_directory = 0;

// The current page directory;
page_directory_t *current_directory = 0;


// Set a bit in the frames bitmap
static void set_frame ( u32int frame_address )
{
	u32int frame     = frame_address / PAGE_SIZE;  // ?
	u32int index     =  INDEX_FROM_BIT( frame );
	u32int offset    = OFFSET_FROM_BIT( frame );
	frames[ index ] |= 0x1 << offset;
}

// Clear a bit in the frames bitmap
static void clear_frame ( u32int frame_address )
{
	u32int frame     = frame_address / PAGE_SIZE;  // ?
	u32int index     =  INDEX_FROM_BIT( frame );
	u32int offset    = OFFSET_FROM_BIT( frame );
	frames[ index ] &= ~ ( 0x1 << offset );
}

// Test a bit in the frames bitmap
// static u32int test_frame ( u32int frame_address )
// {
// 	u32int frame     = frame_address / PAGE_SIZE;  // ?
// 	u32int index     =  INDEX_FROM_BIT( frame );
// 	u32int offset    = OFFSET_FROM_BIT( frame );

// 	return ( frames[ index ] & ( 0x1 << offset ) );  // why no do comp to zero here and return true/false?
// }

// Find first free frame
static u32int first_frame ()
{
	u32int i, j;

	for ( i = 0; i < INDEX_FROM_BIT( nframes ); i += 1 )
	{
		if ( frames[ i ] != 0xFFFFFFFF )  // exit early if nothing free
		{
			for ( j = 0; j < NBITS; j += 1 )
			{
				u32int test = 0x1 << j;

				if ( ! ( frames[ i ] & test ) )  // if free (zero)
				{
					return i * 32 + j;
				}
			}
		}
	}

	// How does this otherwise return -1 when nothing is found ??
	return - 1;
}


// Allocate a frame
void alloc_frame ( page_t *page, int is_kernel, int is_writeable )
{
	if ( page -> frame != 0 )
	{
		return;  // frame already allocated
	}
	else
	{
		u32int index = first_frame();  // index of the first free frame

		if ( index == ( u32int ) - 1 )
		{
			PANIC( "No free page frames!" );
		}

		set_frame( index * PAGE_SIZE );  // ??

		page -> present = 1;
		page -> rw      = ( is_writeable ) ? 1 : 0;
		page -> user    = ( is_kernel ) ? 0 : 1;
		page -> frame   = index;
	}
}

// Deallocate a frame
void free_frame ( page_t *page )
{
	u32int frame;

	frame = page -> frame;

	if ( ! frame )
	{
		return;  // frame was not previously allocated
	}
	else
	{
		clear_frame( frame );

		page -> frame = 0;
	}
}


// Sets up the environment, page directories etc and enables paging
void init_paging ()
{
    monitor_write("Initializing paging...\n", true, false);
	int i;
	u32int tmp;

	// Size of physical memory. For the moment we assume it is 16MB
	u32int mem_size = 0x1000000;
	nframes = mem_size / PAGE_SIZE;

	debug("kmalloc calls coming from ( %s )\n", __FILE__);

	// Track allocated frames (nframes/32 words with each bit representing a frame)
	frames = ( u32int * ) kmalloc( INDEX_FROM_BIT( nframes ) );
	memset( ( u8int * ) frames, 0, INDEX_FROM_BIT( nframes ) );  // fill with zeros

	// Make a page directory
	tmp = kmalloc_aligned( sizeof( page_directory_t ) );
	memset( ( u8int * ) tmp, 0, sizeof( page_directory_t ) );  // fill with zeros
	kernel_directory = ( page_directory_t * ) tmp;

	current_directory = kernel_directory;

	/*
		Map some pages in the kernel heap area.
		Here we call get_page but not alloc_frame. This causes page_table_t's 
		to be created where necessary. We can't allocate frames yet because they
		they need to be identity mapped first below, and yet we can't increase
		placement_address between identity mapping and enabling the heap!
	*/
	for ( i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += PAGE_SIZE )
	{
		get_page( i, 1, kernel_directory );
	}

	/*
		We need to identity map (phys addr = virt addr) from
		0x0 to the end of used memory, so we can access this
		transparently, as if paging wasn't enabled.
		NOTE that we use a while loop here deliberately.
		inside the loop body we actually change placement_address
		by calling kmalloc(). A while loop causes this to be
		computed on-the-fly rather than once at the start.
		Allocate a lil' bit extra so the kernel heap can be initialized properly.
	*/
	i = 0;
	while ( i < placement_address + PAGE_SIZE )
	{
		// Kernel code is readable but not writeable from userspace
		alloc_frame( get_page( i, 1, kernel_directory ), 0, 0 );

		i += PAGE_SIZE;
	}

	// Now allocate those pages we mapped earlier
	for ( i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += PAGE_SIZE )
	{
		alloc_frame( get_page( i, 1, kernel_directory ), 0, 0 );
	}

	// Before we enable paging, we must register our page fault handler
	register_interrupt_handler( 14, page_fault );

	// Enable paging
	switch_to_page_directory( kernel_directory );

	// Initialise the kernel heap.
	kheap = create_heap( KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, 0xCFFFF000, 0, 0 );
}

// Returns a pointer to the page entry of a particular address
/* Can optionally be passed a make parameter. If 1 and the
   page table that the requested page entry should reside in hasn't been created,
   then it will be created.
*/
page_t *get_page ( u32int address, int make, page_directory_t *dir )
{
	// Turn the address into an index
	u32int frame = address / PAGE_SIZE;

	// Find the page table containing this address
	u32int table_idx = frame / TABLE_SIZE;
	u32int page_idx = frame % TABLE_SIZE;

	if ( dir -> tables[ table_idx ] )  // page-table is already assigned
	{
		return &dir -> tables[ table_idx ] -> pages[ page_idx ];
	}
	else if ( make )  // create page-table
	{
		u32int physical_address;
		u32int virtual_address;  // ??

		// Retrieve a memory block which is page aligned and its physical address
		virtual_address = kmalloc_alignedp( sizeof( page_table_t ), &physical_address );
		memset( ( u8int * ) virtual_address, 0, PAGE_SIZE );  // fill with zeros
		
		// Store virtual address
		dir -> tables[ table_idx ] = ( page_table_t * ) virtual_address;

		// Store physical address
		dir -> tables_physical[ table_idx ] = physical_address | 0x7;  // set present, rw, and user-mode bits

		//
		return &dir -> tables[ table_idx ] -> pages[ page_idx ];
	}
	else
	{
		return 0;
	}
}

// Causes the specified page directory to be loaded into the CR3 register
void switch_to_page_directory ( page_directory_t *dir )
{
	/*
		https://en.wikipedia.org/wiki/Control_register
		What are the contents of CR0 and CR3 in relation to paging ???
	*/
	u32int cr0;

	current_directory = dir;

	// CR3 = address of tables_physical
	asm volatile( "mov %0, %%cr3" : : "r" ( &dir -> tables_physical ) );

	// Enable paging
	asm volatile( "mov %%cr0, %0" : "=r" ( cr0 ) );
	debug("Switching page directory ( CR0 = %x )\n", cr0);
	cr0 |= 0x80000000;  // set paging bit
	asm volatile( "mov %0, %%cr0" : : "r" ( cr0 ) );
	debug("Switched page directory ( CR0 = %x )\n", cr0);
}

// Handler for page faults (prints out a nice error message)
void page_fault ( registers_t regs )
{
	// Faulting address is stored in CR2 register
	u32int faulting_address;

	asm volatile( "mov %%cr2, %0" : "=r" ( faulting_address ) );

	// The error code gives us details of what happened
	int present  = ! ( regs.err_code & 0x1 );  // Page not present
	int rw       = regs.err_code & 0x2;        // Write operation?
	int user     = regs.err_code & 0x4;        // Processor was in user-mode?
	int reserved = regs.err_code & 0x8;        // Overwritten CPU-reserved bits of page entry?
	int id       = regs.err_code & 0x10;       // Caused by an instruction fetch?

	// Output an error message.
	monitor_write( "Page fault! ( ", false, false);
	if ( present )  { monitor_write( "present ", false, false); }
	if ( rw )       { monitor_write( "read-only ", false, false); }
	if ( user )     { monitor_write( "user-mode ", false, false); }
	if ( reserved ) { monitor_write( "reserved ", false, false); }
	if ( id )       { monitor_write( "instruction fetch ", false, false); }
	monitor_write( ") at ", false, false);
	monitor_write_hex( faulting_address );
	monitor_writeln();
	
	PANIC( "Page fault" );
}
