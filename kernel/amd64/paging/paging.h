#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <amd64/moon.h>

#define PAGE_SHIFT 12
#define GET_PMLx(vaddr, pml, level) (struct Pml*) ((uintptr_t)pml->page_tables[index_of(vaddr, level)].address << PAGE_SHIFT);

#define invlpg(param_addr) asm volatile("invlpg (%[addr])" ::[addr] "r"(param_addr))
#define wrcr3(pml4) asm volatile("mov %0, %%cr3\n" ::"r"(pml4) : "memory")

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

enum
{
    PG_PRESENT = (1 << 0), // Present
    PG_RW = (1 << 1),      // Read-write
    PG_SUPER = (1 << 2)    // Supervisor
};

enum vmm_mapping_protection
{
    MAP_READONLY = PG_PRESENT,                   // Present/readonly, kernel only
    MAP_KERN = PG_PRESENT | PG_RW,               // Read-write kernel only
    MAP_USER_RW = PG_PRESENT | PG_RW | PG_SUPER, // User accessible page (ReadWrite)
    MAP_USER_RO = PG_PRESENT | PG_SUPER          // User accesible page (ReadOnly)
};

struct pte paging_create_entry(uint64_t paddr, int flags);
struct pte paging_purge_entry();

#endif // PAGING_H