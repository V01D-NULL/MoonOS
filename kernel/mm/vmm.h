#ifndef VMM_H
#define VMM_H

#include <amd64/paging/paging.h>
#include <proc/tasking/task.h>
#include <mm/cpu/CR.h>
#include <stdint.h>
#include <stdbool.h>
#include <stivale2.h>
#include <util/range.h>

#define GB 0x40000000UL

typedef struct task_struct task_t;
typedef struct
{
    range_t range;
    size_t address_offset; // Use one the the vma offsets (mm/mm.h)
    enum vmm_mapping_protection protection;
} VmmRange;

STATIC_INLINE VmmRange as_vmm_range(size_t base, size_t top, size_t offset)
{
    return (VmmRange){.range = (range_t){.base = base, .limit = top}, .address_offset = offset};
}

void v_init(struct stivale2_mmap_entry *mmap, int entries);
void v_map(struct Pml *pml4, size_t vaddr, size_t paddr, int flags);
void v_map_fast(struct Pml *pml4, size_t vaddr, size_t paddr, int flags);
void v_unmap(struct Pml *pml4, size_t vaddr);
void v_map_range(VmmRange range, int flags, struct Pml *pagemap);
void v_map_range_fast(VmmRange range, int flags, struct Pml *pagemap);

struct Pml *create_new_pagemap(void);
struct Pml *get_kernel_pagemap(void);
void switch_pagemap(task_t task);
void switch_to_kernel_pagemap(void);
void copy_kernel_mappings(task_t task);

void pagefault_handler(uint64_t cr2, int error_code);

#endif // VMM_H
