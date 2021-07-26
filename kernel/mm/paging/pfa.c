#include "pfa.h"
#include "../ram.h"
#include "../../drivers/io/serial.h"
#include "../../amd64/validity.h"

__export pmm_t pmm;

//Mark a single page as free
int pfa_mark_page_as_free(void *address, bool calculate_absolute_address)
{

    if (calculate_absolute_address)
    {
        uint64_t idx = ADDRESS_TO_BIT(address);
        
        /* Is this page already reserved? */
        if (pmm.bitmap_manager.get(pmm.bitmap_manager.pool, idx) == BITMAP_USED)
            return PFA_ERR;

        size_t addr = BIT_TO_ADDRESS(idx);
        debug("pfa_free_ok for address 0x%llx w/ bit index %ld\n", (uintptr_t)addr, idx);
        
        /* Clear bit in bitmap aka free a page */
        pmm.bitmap_manager.clear(pmm.bitmap_manager.pool, idx);
    }
    else
    {
        uint64_t idx = (uint64_t)address;

        /* Is this page already reserved? */
        if (pmm.bitmap_manager.get(pmm.bitmap_manager.pool, idx) == BITMAP_FREE)
            return PFA_ERR;

        debug("pfa_free_ok for bit index %ld\n", idx);

        /* Clear bit in bitmap aka free a page */
        pmm.bitmap_manager.clear(pmm.bitmap_manager.pool, idx);
    }

    /* Update RAM stats */
    ram_manager_free(PAGE_SIZE);

    return PFA_OK;
}

//Mark a single page as used
//calculate_absolute_address will determine whether or not print the bitmap index or the address of the page, this way the pfa functions are more flexibel meaning they can be used with a bit index or an absolute addresss
int pfa_mark_page_as_used(void *address, bool calculate_absolute_address)
{
    if (calculate_absolute_address)
    {
        uint64_t idx = ADDRESS_TO_BIT(address);
        
        /* Is this page already reserved? */
        if (pmm.bitmap_manager.get(pmm.bitmap_manager.pool, idx) == BITMAP_USED)
            return PFA_ERR;

        size_t addr = BIT_TO_ADDRESS(idx);
        // debug("pfa_reserve_ok for address 0x%llx w/ bit index %ld\n", addr, idx);
        
        /* Set bit in bitmap aka reserve a page */
        pmm.bitmap_manager.set(pmm.bitmap_manager.pool, idx);
    }
    else
    {
        uint64_t idx = (uint64_t)address;

        /* Is this page already reserved? */
        if (pmm.bitmap_manager.get(pmm.bitmap_manager.pool, idx) == BITMAP_USED)
            return PFA_ERR;

        // debug("pfa_reserve_ok for bit index %ld\n", idx);

        /* Set bit in bitmap aka reserve a page */
        pmm.bitmap_manager.set(pmm.bitmap_manager.pool, idx);
    }
    
    /* Update RAM stats */
    ram_manager_add(PAGE_SIZE);

    return PFA_OK;
}
