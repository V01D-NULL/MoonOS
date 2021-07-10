#ifndef VMM_H
#define VMM_H

#include "../amd64/validity.h"
#include "../drivers/io/serial.h"
#include "paging/pfa.h"
#include "paging/CR.h"
#include <stdint.h>

#define PAGE_TABLE_LIMIT 1024

struct pte
{
    uint8_t present: 1;
    uint8_t readwrite: 1;
    uint8_t supervisor: 1;
    uint8_t writethrough: 1;
    uint8_t cache_disabled: 1;
    uint8_t accessed: 1;
    uint8_t dirty: 1;
    uint8_t ignore: 1;
    uint8_t global: 1;
    uint8_t avail: 3;
    uint64_t address: 42;
}__pack_bytes; /* There's no need to use pack bytes but I tend to use that with bitfields */
typedef struct pte pte_t;

__page_align struct page_directory
{
    pte_t page_tables[512];
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
page_info_t vmm_dissect_vaddr(uint64_t virt_addr);
static uint64_t vmm_get_lv4();

static inline void vmm_set_new_pte(
    pte_t new_pte_entry,
    uintptr_t level
);

#endif // VMM_H