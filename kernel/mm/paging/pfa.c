#include "pfa.h"
#include "../ram.h"
#include "../../drivers/io/serial.h"

extern pmm_t pmm;

//Mark a single page as free
int pfa_mark_page_as_free(void *address)
{   
    uint64_t idx = ((uint64_t)address / PAGE_SIZE);

    /* Is this block already reserved? */
    if (pmm.bitmap_manager.get(pmm.bitmap_manager.pool, idx) == BITMAP_USED)
        return 1;

    debug("pfa_free_ok for address 0x%llx w/ bit index %ld\n", (uintptr_t)address, idx);
    
    /* Set bit in bitmap aka reserve a page */
    pmm.bitmap_manager.clear(pmm.bitmap_manager.pool, idx);
    
    /* Update RAM stats */
    ram_manager_free(PAGE_SIZE);

    return 0;
}

//Mark a single page as used
int pfa_mark_page_as_used(void *address)
{
    uint64_t idx = ((uint64_t)address / PAGE_SIZE);
    int addr = (idx * PAGE_SIZE * 8);

    /* Is this block already reserved? */
    if (pmm.bitmap_manager.get(pmm.bitmap_manager.pool, idx) == BITMAP_USED)
        return 1;

    debug("pfa_reserve_ok for address 0x%llx w/ bit index %ld\n", (uintptr_t)address, idx);
    
    /* Set bit in bitmap aka reserve a page */
    pmm.bitmap_manager.set(pmm.bitmap_manager.pool, idx);
    
    /* Update RAM stats */
    ram_manager_add(PAGE_SIZE);

    return 0;
}
