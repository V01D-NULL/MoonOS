#include "pfa.h"
#include "../ram.h"
#include "../../drivers/io/serial.h"

extern liballoc_bitmap_t bitmap_manager;

//Mark a single page as free
int pfa_mark_page_as_free(void *address)
{   
    uint64_t idx = ((uint64_t)address / PAGE_SIZE);

    /* Is this block already reserved? */
    if (bitmap_manager.get(bitmap_manager.pool, idx) == BITMAP_USED)
        return 1;

    debug("pfa_free_ok for address 0x%llx w/ bit index %ld\n", (uintptr_t)address, idx);
    
    /* Set bit in bitmap aka reserve a page */
    bitmap_manager.clear(bitmap_manager.pool, idx);
    
    /* Update RAM stats */
    ram_manager_free(PAGE_SIZE);

    return 0;
}

//Mark a single page as used
int pfa_mark_page_as_used(void *address)
{
    uint64_t idx = ((uint64_t)address / PAGE_SIZE);
    int addr = (idx * PAGE_SIZE * 8);
    
    debug("addr: %d, %x | address passed: %x\n", addr, addr, (uint64_t)address);

    /* Is this block already reserved? */
    if (bitmap_manager.get(bitmap_manager.pool, idx) == BITMAP_USED)
        return 1;

    debug("pfa_reserve_ok for address 0x%llx w/ bit index %ld\n", (uintptr_t)address, idx);
    
    /* Set bit in bitmap aka reserve a page */
    bitmap_manager.set(bitmap_manager.pool, idx);
    
    /* Update RAM stats */
    ram_manager_add(PAGE_SIZE);

    return 0;
}
