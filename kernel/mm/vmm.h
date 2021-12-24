#ifndef VMM_H
#define VMM_H

#include <amd64/moon.h>
#include <devices/serial/serial.h>
#include <mm/cpu/CR.h>
#include <stdint.h>
#include <stdbool.h>
#include <stivale2.h>
#include <util/range.h>

// Flags
enum
{
    PG_PR   = 1,           // Present
    PG_RW   = 2 | PG_PR,   // Read-write
    PG_PRIV = 4 | PG_RW,   // User/supervisor
    PG_WT   = 8,           // Write-through
    PG_CD   = 16,          // Cache-disable
};

#define TLB_FLUSH(param_addr) __asm__ volatile("invlpg (%[addr])" ::[addr] "r"(param_addr));
#define PAGE_LOAD_CR3(pml4)   __asm__ volatile("mov %0, %%cr3\n" ::"r"(pml4) : "memory");

void vmm_init(bool has_5_level_paging, struct stivale2_struct_tag_memmap *mmap);
void vmm_map(size_t vaddr, size_t paddr, int flags);
void vmm_unmap(size_t vaddr);
void vmm_remap(size_t vaddr_old, size_t vaddr_new, int flags);
void vmm_map_range(range_t range, size_t offset, int flags);

uint64_t *vmm_get_kernel_pagemap(void);
void vmm_guess_and_map(uint64_t cr2, int error_code);

#endif // VMM_H
