#ifndef VMM_H
#define VMM_H

#include <amd64/moon.h>
#include <devices/serial/serial.h>
#include <mm/cpu/CR.h>
#include <stdint.h>
#include <stdbool.h>
#include <stivale2.h>

#define FLAGS_PR   1   // Present
#define FLAGS_RW   2   // Read-write
#define FLAGS_PRIV 4   // User/supervisor
#define FLAGS_WT   8   // Write-through
#define FLAGS_CD   16  // Cache-disable

#define PAGE_NOT_PRESENT 0x0

#define TLB_FLUSH(param_addr) __asm__ volatile("invlpg (%[addr])" ::[addr] "r"(param_addr));
#define PAGE_LOAD_CR3(pml4)   __asm__ volatile("mov %0, %%cr3\n" ::"r"(pml4) : "memory");

/* Represents the indexes of entries in the page tables */
typedef struct
{
    uintptr_t lv1;
    uintptr_t lv2;
    uintptr_t lv3;
    uintptr_t lv4;
    uintptr_t lv5;
} page_info_t;

void vmm_init(bool has_5_level_paging, struct stivale2_struct_tag_memmap *mmap);
void vmm_map(size_t vaddr, size_t paddr, int flags);
void vmm_unmap(size_t vaddr);
void vmm_remap(size_t vaddr_old, size_t vaddr_new, int flags);
void vmm_adjust_lower_half_mapping(bool unmap, int flags);

uint64_t *vmm_get_root(void);
page_info_t vmm_dissect_vaddr(uint64_t virt_addr);
void vmm_guess_and_map(uint64_t cr2, int error_code);

#endif // VMM_H
