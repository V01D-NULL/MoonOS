#ifndef VMM_H
#define VMM_H

#include <proc/tasking/task.h>
#include <amd64/moon.h>
#include <devices/serial/serial.h>
#include <mm/cpu/CR.h>
#include <stdint.h>
#include <stdbool.h>
#include <stivale2.h>
#include <util/range.h>
#include <panic.h>

#define GB 0x40000000UL

// Flags
enum vmm_mapping_protection
{
    MAP_READONLY = 1,        // Present/readonly, kernel only
    MAP_KERN = 3,            // Read-write kernel only
    MAP_USER = 7,            // User accessible page (R+W)
};

typedef struct task_struct task_t;
typedef struct
{
    range_t range;
    size_t address_offset; // Can be VMEM_LV{4,5}_BASE, VMEM_CODE_BASE or VMEM_DIRECT_MAPPING
    enum vmm_mapping_protection protection;
} vmm_range_t;

#define invlpg(param_addr) __asm__ volatile("invlpg (%[addr])" ::[addr] "r"(param_addr))
#define wrcr3(pml4) __asm__ volatile("mov %0, %%cr3\n" ::"r"(pml4) \
                                     : "memory")

void vmm_init(bool has_5_level_paging, struct stivale2_struct_tag_memmap *mmap);
void vmm_map(uint64_t *pagemap, size_t vaddr, size_t paddr, int flags);
void vmm_unmap(uint64_t *pagemap, size_t vaddr);
void vmm_remap(uint64_t *pagemap, size_t vaddr_old, size_t vaddr_new, int flags);
void vmm_map_range(vmm_range_t range, int flags, uint64_t *pagemap);

uint64_t *vmm_create_new_pagemap(void);
uint64_t *vmm_get_kernel_pagemap(void);
void vmm_switch_pagemap(task_t task);
void vmm_switch_to_kernel_pagemap(void);
void vmm_copy_kernel_mappings(task_t task);

void vmm_pagefault_handler(uint64_t cr2, int error_code);

#endif // VMM_H
