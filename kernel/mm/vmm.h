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

#define PAGE_SHIFT 12
#define GET_PMLx(vaddr, pml, level) (struct Pml*) ((uintptr_t)pml->page_tables[index_of(vaddr, level)].address << PAGE_SHIFT);

#define invlpg(param_addr) asm volatile("invlpg (%[addr])" ::[addr] "r"(param_addr))
#define wrcr3(pml4) asm volatile("mov %0, %%cr3\n" ::"r"(pml4) : "memory")

STATIC_INLINE VmmRange vmm_as_range(size_t base, size_t top, size_t offset)
{
    return (VmmRange){.range = (range_t){.base = base, .limit = top}, .address_offset = offset};
}

struct pte
{
    uint8_t present : 1;
    uint8_t readwrite : 1;
    uint8_t supervisor : 1;
    uint8_t writethrough : 1;
    uint8_t cache_disabled : 1;
    uint8_t accessed : 1;
    uint8_t dirty : 1;
    uint8_t ignore : 1;
    uint8_t global : 1;
    uint8_t avail : 3;
    uint64_t address : 52;
} gnu_pack_bytes;

struct Pml
{
    struct pte page_tables[512];
} gnu_pack_bytes;

void vmm_init(struct stivale2_mmap_entry *mmap, int entries);
void vmm_map(struct Pml *pml4, size_t vaddr, size_t paddr, int flags);
void vmm_remap(struct Pml *pml4, size_t vaddr, size_t paddr, int flags);
void vmm_unmap(struct Pml *pml4, size_t vaddr);

struct pte vmm_create_entry(uint64_t paddr, int flags);
struct pte vmm_purge_entry();

void vmm_switch_to_kernel_pagemap(void);
void vmm_map_range(VmmRange range, int flags, struct Pml *pagemap);

struct Pml *vmm_create_new_pagemap(void);
struct Pml *vmm_get_kernel_pagemap(void);
void vmm_switch_pagemap(task_t task);
void vmm_switch_to_kernel_pagemap(void);
void vmm_copy_kernel_mappings(task_t task);

void vmm_pagefault_handler(uint64_t cr2, int error_code);

#endif // VMM_H
