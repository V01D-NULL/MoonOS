#include "virt.h"
#include "phys.h"
#include "page.h"
#include <base/assert.h>
#include <panic.h>

static struct Pml *kernel_pagemap;

void arch_init_paging(void) {}

inline uint64_t index_of(uint64_t vaddr, int offset)
{
	return 0;
}

// Get the next PML in the page table hierarchy
static struct Pml *vmm_pml_advance(struct Pml *entry, size_t level, int flags)
{
	return NULL;
}

void arch_map_page(struct Pml *pml4, virt_t vaddr, phys_t paddr, int flags) {}

void arch_quickmap_page(struct Pml *pml4, virt_t vaddr, phys_t paddr, int flags) {}

void arch_unmap_page(struct Pml *pml4, virt_t vaddr) {}

void arch_map_range(struct Pml *pml4, Range range, int flags, uint64_t offset)
{
	uint64_t base = range.base, limit = range.limit;
	assert(base % 4096 == 0 && limit % 4096 == 0);

	for (; base != limit; base += PAGE_SIZE)
	{
		arch_map_page(pml4, base + offset, base, flags);
	}
}

void arch_quickmap_range(struct Pml *pml4, Range range, int flags, uint64_t offset)
{
	uint64_t base = range.base, limit = range.limit;
	assert(base % 4096 == 0 && limit % 4096 == 0);

	for (; base != limit; base += PAGE_SIZE)
	{
		arch_quickmap_page(pml4, base + offset, base, flags);
	}
}

void arch_switch_to_kernel_pagemap(void) {}

struct Pml *arch_create_new_pagemap(void)
{
	return (struct Pml *)arch_alloc_page();
}

void arch_copy_kernel_mappings(Task task) {}

void arch_switch_pagemap(Task task)
{
	if (!task.pagemap)
	{
		panic_if(
			task.task_type == TASK_DAEMON,
			"The kernel daemon task '%s' attempted to switch to a NULL pagemap!", task.descriptor);

		return; // Todo: Should probably return a status code or something
	}
}

void arch_pagefault_handler(uint64_t cr2, int error_code) {}