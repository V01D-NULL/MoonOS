#ifndef VMM_H
#define VMM_H

#include "../amd64/validity.h"
#include "../drivers/io/serial.h"
#include "cpu/CR.h"
#include <stdint.h>

#define FLAGS_PR   1   // Present
#define FLAGS_RW   2   // Read-write
#define FLAGS_PRIV 4   // User/supervisor
#define FLAGS_WT   8   // Write-through
#define FLAGS_CD   16  // Cache-disable

#define PAGE_NOT_PRESENT 0x0

#define TLB_FLUSH(param_addr) __asm__ volatile("invlpg (%[addr])" ::[addr] "r"(param_addr));
#define PAGE_LOAD_CR3(pml4)   __asm__ volatile("mov %0, %%cr3\n" ::"r"(pml4) : "memory");

/* A 4 level paging struct which holds info about the levels and page offset */
typedef struct
{
    uintptr_t page_offset;
    uintptr_t lv1;
    uintptr_t lv2;
    uintptr_t lv3;
    uintptr_t lv4;
} page_info_t;

void vmm_init();
void vmm_map(uint64_t *pml4, size_t vaddr, size_t paddr, int flags);
page_info_t vmm_dissect_vaddr(uint64_t virt_addr);

uint64_t *vmm_get_lv4();

#endif // VMM_H
