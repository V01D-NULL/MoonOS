#ifndef VMM_H
#define VMM_H

#include "../drivers/io/serial.h"
#include "paging/pfa.h"
#include "paging/CR.h"
#include <stdint.h>

#define PAGE_TABLE_LIMIT 1024

struct page_directory
{
    uintptr_t page_tables[512];
};

/* A 4 level paging struct which holds info about the levels and page_offset */
typedef struct
{
    uintptr_t page_offset;
    uintptr_t lv1;
    uintptr_t lv2;
    uintptr_t lv3;
    uintptr_t lv4;
} page_info_t;


void vmm_init();
page_info_t vmm_vaddr_to_page_info_struct(uint64_t virt_addr);

#endif // VMM_H