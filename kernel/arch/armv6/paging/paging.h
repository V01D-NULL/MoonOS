#ifndef PAGING_H
#define PAGING_H

#include <moon.h>
#include <base/base-types.h>

struct pte
{
} PACKED;

struct Pml
{
	struct pte page_tables[512 /*CHANGE ME*/];
} PACKED;

struct Pagefault
{
};

// CHANGE ME
enum
{
	PG_PRESENT = (1 << 0), // Present
	PG_RW = (1 << 1),	   // Read-write
	PG_SUPER = (1 << 2)	   // Supervisor
};

// CHANGE ME
enum
{
	MAP_READONLY = PG_PRESENT,					 // Present/readonly, kernel only
	MAP_KERN = PG_PRESENT | PG_RW,				 // Read-write kernel only
	MAP_USER_RW = PG_PRESENT | PG_RW | PG_SUPER, // User accessible page (ReadWrite)
	MAP_USER_RO = PG_PRESENT | PG_SUPER			 // User accesible page (ReadOnly)
};

struct pte paging_create_entry(uint64_t paddr, int flags);
struct pte paging_purge_entry(void);
struct Pagefault paging_get_pagefault_flags(int error_code, bool do_panic);

#endif // PAGING_H