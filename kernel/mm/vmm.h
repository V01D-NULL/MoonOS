#ifndef VMM_H
#define VMM_H

#include <paging/paging.h>
#include <sched/task.h>
#include <base/base-types.h>
#include <boot/stivale2.h>
#include <moon-extra/range.h>

forward_declare_struct$(Task);

struct$(VmmRange, {
	Range range;
	size_t address_offset; // Use one the the vma offsets (mm/mm.h)
	enum vmm_mapping_protection protection;
});

inline VmmRange as_vm_range(size_t base, size_t top, size_t offset)
{
	return (VmmRange){.range = (Range){.base = base, .limit = top}, .address_offset = offset};
}

void v_init(void);
void v_map(struct Pml *pml4, size_t vaddr, size_t paddr, int flags);
void v_map_fast(struct Pml *pml4, size_t vaddr, size_t paddr, int flags);
void v_unmap(struct Pml *pml4, size_t vaddr);
void v_map_range(VmmRange range, int flags, struct Pml *pagemap);
void v_map_range_fast(VmmRange range, int flags, struct Pml *pagemap);

struct Pml *create_new_pagemap(void);
struct Pml *get_kernel_pagemap(void);
void switch_pagemap(Task task);
void switch_to_kernel_pagemap(void);
void copy_kernel_mappings(Task task);

void pagefault_handler(uint64_t cr2, int error_code);

#endif // VMM_H
