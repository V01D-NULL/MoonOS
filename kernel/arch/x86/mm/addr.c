#include "addr.h"

static uint64_t hhdm_offset = 0xffff800000000000;

void set_hhdm_offset(uint64_t offset)
{
    hhdm_offset = offset;
}

phys_t *va(phys_t paddr)
{
    return (phys_t *)(paddr + hhdm_offset);
}

virt_t *pa(virt_t vaddr)
{
    return (virt_t *)(vaddr - hhdm_offset);
}
