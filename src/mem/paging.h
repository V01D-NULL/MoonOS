#ifndef PAGING_H
#define PAGING_H


#include "../common.h"
#include "../arch/x86/cpu/interrupts/interrupts.h"
#include "../panic.h"

#define TABLE_SIZE      1024
#define PAGE_SIZE       0x1000      // 4096
#define PAGE_ALIGN_MASK 0xFFFFF000  // anding with address checks... ??

typedef struct page
{
	u32int present    :  1;  // Page present in memory
	u32int rw         :  1;  // Read-only if clear, readwrite if set
	u32int user       :  1;  // Supervisor level only if clear
	u32int accessed   :  1;  // Has the page been accessed since last refresh?
	u32int dirty      :  1;  // Has the page been written to since last refresh?
	u32int unused     :  7;  // Amalgamation of unused and reserved bits
	u32int frame      : 20;  // Frame address (shifted right 12 bits)
}
page_t;

typedef struct page_table
{
	page_t pages [ TABLE_SIZE ];  // 2^10 pages per page_table
}
page_table_t;


typedef struct page_directory
{
	// Array of pointers to pagetables. *virtual* addresses
	page_table_t *tables [ TABLE_SIZE ];  // 2^10 page_tables per page_directory

	// Array of pointers to the pagetables above, but gives their *physical*
	// location, for loading into the CR3 register
	u32int tables_physical [ TABLE_SIZE ];

	// The physical address of tables_physical. This comes into play
	// when we get our kernel heap allocated and the directory
	// may be in a different location in virtual memory
	u32int physical_address;
}
page_directory_t;

// Sets up the environment, page directories etc and enables paging
void init_paging ();

// Causes the specified page directory to be loaded into the CR3 register
void switch_to_page_directory ( page_directory_t *dir );

// Returns a pointer to the page entry of a particular address
/* Can optionally be passed a make parameter. If 1 and the
   page table that the requested page entry should reside in hasn't been created,
   then it will be created.
*/
page_t *get_page ( u32int address, int make, page_directory_t *dir );

// Handler for page faults (prints out a nice error message)
void page_fault ( registers_t regs );


// Allocate a frame
void alloc_frame ( page_t *page, int is_kernel, int is_writeable );

// Deallocate a frame
void free_frame ( page_t *page );


#endif  // PAGING_H
