#include <base/base-types.h>
#include <base/assert.h>
#include <base/align.h>
#include <base/mem.h>
#include <printk.h>
#include <panic.h>
#include "phys.h"

void init_phys_allocator(void) {}

void *arch_alloc_page(void)
{
	return NULL;
}

Range arch_alloc_page_range(UNUSED size_t pages)
{
	return (Range){};
}

void arch_free_page(UNUSED void *page) {}