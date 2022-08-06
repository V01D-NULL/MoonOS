#include <mm/phys.h>
#include "pmm.h"

void *pmm_alloc(void)
{
	return arch_alloc_page();
}

void pmm_free(void *page)
{
	arch_free_page(page);
}

Range pmm_alloc_range(size_t pages)
{
	return arch_alloc_page_range(pages);
}