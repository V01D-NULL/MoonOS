#ifndef ADDR_H
#define ADDR_H

#include <base/base-types.h>
#include <moon.h>

#define KiB(x) 1024 * x
#define MiB(x) (1024 * 1024) * x
#define GiB(x) (1024LL * 1024LL * 1024LL) * x

typedef uint64_t virt_t, phys_t;

static const size_t $identity_vma = 0x0;  /* Regular, physical memory offset   */
static const size_t $high_vma = 0x0;	  /* Kernel data structures offset     */
static const size_t $high_vma_heap = 0x0; /* Kernel heap area offset           */
static const size_t $high_vma_code = 0x0; /* Kernel code offset                */

USED static phys_t *va(phys_t paddr)
{
	return (phys_t *)(paddr + ($high_vma));
}

USED static virt_t *pa(virt_t vaddr)
{
	return (virt_t *)(vaddr - ($high_vma));
}

#endif // ADDR_H