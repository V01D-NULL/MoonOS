#include <paging/paging.h>
#include <base/base-types.h>

UNUSED inline bool check_flag(int flags, int bit) { return (flags >> bit) & 1; }

struct pte paging_create_entry(uint64_t paddr, int flags)
{
	return (struct pte){};
}

struct pte paging_purge_entry(void)
{
	return paging_create_entry(0, 0);
}

struct Pagefault paging_get_pagefault_flags(int error_code, bool do_panic)
{
	return (struct Pagefault){};
}