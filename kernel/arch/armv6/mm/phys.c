#include "phys.h"
#include <base/align.h>
#include <base/assert.h>
#include <base/base-types.h>
#include <base/mem.h>
#include <panic.h>
#include <printk.h>

void init_phys_allocator(void)
{
}

void *arch_alloc_page_sz(UNUSED int sz)
{
    return NULL;
}

void arch_free_page(UNUSED void *page)
{
}