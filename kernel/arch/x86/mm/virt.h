#ifndef VIRTUAL_MEMORY_H
#define VIRTUAL_MEMORY_H

#include <moon-extra/range.h>
#include <paging/paging.h>
#include "addr.h"

void arch_map_page(struct Pml *pml4, virt_t vaddr, phys_t paddr, int flags);
void arch_map_range(struct Pml *pml4, Range range, int flags, uint64_t offset);

void arch_unmap_page(struct Pml *pml4, virt_t vaddr);

void        arch_switch_to_kernel_pagemap(void);
struct Pml *arch_create_new_pagemap(void);
void        arch_copy_kernel_mappings(struct Pml *pagemap);
void        arch_switch_pagemap(struct Pml *pagemap);
void        arch_pagefault_handler(uint64_t cr2, int error_code);
phys_t      arch_translate(struct Pml *pml4, virt_t vaddr);

#endif  // VIRTUAL_MEMORY_H