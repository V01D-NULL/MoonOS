#include "vmm.h"
#include "../amd64/validity.h"

size_t *page_directory;

void vmm_invalidate_page_table()
{
    for (int i = 0; i < PAGE_TABLE_LIMIT; ++i)
    {
        page_directory[i] = 0x00000002;
    }
}

static inline void vmm_adjust_page_entry_flags(short entry, long flags)
{
    page_directory[entry] = flags;
}

page_info_t vmm_vaddr_to_page_info_struct(uint64_t virt_addr)
{
    page_info_t pg_info;
    const int bitmask = 0x1FF;
    
    pg_info.page_offset = virt_addr;
    virt_addr >>= 12;
    
    pg_info.lv1 = virt_addr & bitmask;
    virt_addr >>= 9;
    
    pg_info.lv2 = virt_addr & bitmask;
    virt_addr >>= 9;

    pg_info.lv3 = virt_addr & bitmask;
    virt_addr >>= 9;

    pg_info.lv4 = virt_addr & bitmask;
    
    // pg_info.lv2 = virt_addr >>= (9 & bitmask);
    // pg_info.lv3= virt_addr >>= (9 & bitmask);
    // pg_info.lv4 = virt_addr >>= (9 & bitmask);

    return pg_info;
}

void vmm_init()
{
    debug("Reserving memory for the pagetable\n");
    page_directory = pmm_alloc(); //There is no need to free this
    debug("Pagetable resides at 0x%x\n", page_directory);
}