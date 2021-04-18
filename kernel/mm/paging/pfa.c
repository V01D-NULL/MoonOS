#include "pfa.h"
#include "../ram.h"

extern const size_t kernel_end[];

//Mark a single page as free
int pfa_mark_page_as_free(uint8_t *bitmap, void *address)
{   
    uint64_t idx = (uint64_t) (uintptr_t) address / PAGE_SIZE;
    
    if (bitmap[idx] == PMM_USED)
    {
        pmm_clear(idx);
        ram_manager_free(PAGE_SIZE);
        return 0;
    }
    return 1;
}

//Mark a single page as used
int pfa_mark_page_as_used(uint8_t *bitmap, void *address)
{
    uint64_t idx = (uint64_t)address / PAGE_SIZE;
    
    debug("pfa_ok for address 0x%llx and bit index %ld\n", (uintptr_t)address, idx);
    pmm_set(idx);
    ram_manager_add(PAGE_SIZE);
    return 0;
}