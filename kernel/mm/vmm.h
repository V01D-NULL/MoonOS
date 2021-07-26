#ifndef VMM_H
#define VMM_H

#include "../amd64/validity.h"
#include "../drivers/io/serial.h"
#include "paging/pfa.h"
#include "paging/CR.h"
#include <stdint.h>

#define FLAGS_RW   0b00000010   // Read-write
#define FLAGS_PRIV 0b00000100   // User/supervisor
#define FLAGS_WT   0b00001000   // Write-through
#define FLAGS_CD   0b00010000   // Cache-disable

#define PAGE_NOT_PRESENT 0x0

#define TLB_FLUSH(param_addr) asm volatile("invlpg (%[addr])" ::[addr] "r"(param_addr));
#define PAGE_LOAD_CR3(pml4)   asm volatile("mov %0, %%cr3\n" ::"r"(pml4) : "memory");

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
} __pack_bytes;

__page_align struct page_directory
{
    struct pte page_tables[512] __page_align;
};

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
void vmm_map(struct page_directory *pml4, size_t vaddr, size_t paddr, int flags);
void vmm_remap(struct page_directory *pml4, size_t vaddr, size_t paddr, int flags);
void vmm_unmap(struct page_directory *pml4, size_t vaddr, int flags);

struct pte vmm_create_entry(uint64_t paddr, int flags);
struct pte vmm_purge_entry();

page_info_t vmm_dissect_vaddr(uint64_t virt_addr);

struct page_directory *get_pml4();

#endif // VMM_H
