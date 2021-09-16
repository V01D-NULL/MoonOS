#include "vmm.h"
#include "memdefs.h"
#include <libk/kassert.h>
#include <amd64/validity.h>
#include <drivers/vbe/vbe.h>
#include <int/gdt.h>
#include <int/idt.h>
#include <util/ptr.h>
#include <stdbool.h>

#define GB 0x40000000UL

static uint64_t *rootptr;

void vmm_init(bool has_5_level_paging)
{
    la57_enabled = has_5_level_paging;

    rootptr = (uint64_t *)pmm_alloc();
    if (la57_enabled)
    {
        debug(true, "Using 5 level paging\n");
        printk("vmm", "pml5 resides at 0x%llx\n", rootptr);
    }
    else
    {
        debug(true, "Using 4 level paging\n");
        printk("vmm", "pml4 resides at 0x%llx\n", rootptr);
    }

    // First 4 GB in phys mem
    for (size_t n = 0; n < 4 * GB; n += PAGE_SIZE)
    {
        vmm_map(n, n, FLAGS_PR | FLAGS_RW);
    }

    // Map 0xff00... or 0xffff80... through 4GB
    for (size_t n = 0; n < 4 * GB; n += PAGE_SIZE)
    {
        vmm_map(to_virt(n), n, FLAGS_PR | FLAGS_RW);
    }

    // Map PMR's
    for (size_t n = 0; n < 0x80000000; n += PAGE_SIZE)
    {
        vmm_map(to_phys(n), n, FLAGS_PR | FLAGS_RW);
    }

    // Map framebuffer
    struct memtag_range framebuffer = pmm_find_tag(STIVALE2_MMAP_FRAMEBUFFER, 1);
    for (size_t n = 0; n < framebuffer.size; n += PAGE_SIZE)
        vmm_map(n, n, FLAGS_PR | FLAGS_RW);

    debug(true, "Old PML4: %llx\n", cr_read(CR3)); // Bootloader pml{n}
    PAGE_LOAD_CR3(GENERIC_CAST(uint64_t, rootptr));
    debug(true, "New PML4: %llx\n", cr_read(CR3)); // Kernel pml{n}

    init_gdt();
    init_idt();

    printk("vmm", "Initialised vmm\n");
}

static uint64_t *vmm_get_pml_or_alloc(uint64_t *entry, size_t level, int flags)
{
    if (entry[level] & 1)
        goto no_alloc;

    entry[level] = VOID_PTR_TO_64(pmm_alloc());
    entry[level] |= flags;

no_alloc:
    return GENERIC_CAST(uint64_t *, (entry[level] & ~(511)));
}

static uint64_t *vmm_get_pml(uint64_t *entry, size_t level)
{
    return GENERIC_CAST(uint64_t *, (entry[level] & ~(511)));
}

void vmm_map(size_t vaddr, size_t paddr, int flags)
{
    //TODO:
    //Performance tweak: Use an algorithm to save an address
    //which has been mapped already and check if the address is already mapped here
    page_info_t info = vmm_dissect_vaddr(vaddr);
    if (la57_enabled)
    {
        uint64_t *pml4, *pml3, *pml2, *pml1 = NULL;
        pml4 = vmm_get_pml_or_alloc(rootptr, info.lv5, flags);
        pml3 = vmm_get_pml_or_alloc(pml4, info.lv4, flags);
        pml2 = vmm_get_pml_or_alloc(pml3, info.lv3, flags);
        pml1 = vmm_get_pml_or_alloc(pml2, info.lv2, flags);
        pml1[info.lv1] = (paddr | flags);
    }

    else
    {
        uint64_t *pml3, *pml2, *pml1 = NULL;
        pml3 = vmm_get_pml_or_alloc(rootptr, info.lv4, flags);
        pml2 = vmm_get_pml_or_alloc(pml3, info.lv3, flags);
        pml1 = vmm_get_pml_or_alloc(pml2, info.lv2, flags);
        pml1[info.lv1] = (paddr | flags);
    }
}

void vmm_unmap(size_t vaddr)
{
    page_info_t info = vmm_dissect_vaddr(vaddr);

    if (la57_enabled)
    {
        uint64_t *pml4, *pml3, *pml2, *pml1 = NULL;
        pml4 = vmm_get_pml(rootptr, info.lv5);
        pml3 = vmm_get_pml(pml4, info.lv4);
        pml2 = vmm_get_pml(pml3, info.lv3);
        pml1 = vmm_get_pml(pml2, info.lv2);
        debug(true, "pml[%d] = %llx\n", info.lv1, pml1[info.lv1] & ~(511));
        pml1[info.lv1] = 0;
    }
    else
    {
        uint64_t *pml3, *pml2, *pml1 = NULL;
        pml3 = vmm_get_pml(rootptr, info.lv4);
        pml2 = vmm_get_pml(pml3, info.lv3);
        pml1 = vmm_get_pml(pml2, info.lv2);
        pml1[info.lv1] = 0;
    }
    TLB_FLUSH(vaddr);
}

void vmm_remap(size_t vaddr_old, size_t vaddr_new, int flags)
{
    page_info_t info = vmm_dissect_vaddr(vaddr_old);
    size_t paddr = 0;

    /* Save the physical address `vaddr_old` was mapped to, purge that entry in the TLB
       and remap the page to a new virtual address (physical address stays the same) 
    */
    if (la57_enabled)
    {
        uint64_t *pml4, *pml3, *pml2, *pml1 = NULL;
        pml4 = vmm_get_pml_or_alloc(rootptr, info.lv5, flags);
        pml3 = vmm_get_pml_or_alloc(pml4, info.lv4, flags);
        pml2 = vmm_get_pml_or_alloc(pml3, info.lv3, flags);
        pml1 = vmm_get_pml_or_alloc(pml2, info.lv2, flags);
        paddr = pml1[info.lv1] & ~(511);
        pml1[info.lv1] = 0;
        TLB_FLUSH(vaddr_old);
    }

    else
    {
        uint64_t *pml3, *pml2, *pml1 = NULL;
        pml3 = vmm_get_pml_or_alloc(rootptr, info.lv4, flags);
        pml2 = vmm_get_pml_or_alloc(pml3, info.lv3, flags);
        pml1 = vmm_get_pml_or_alloc(pml2, info.lv2, flags);
        paddr = pml1[info.lv1] & ~(511);
        pml1[info.lv1] = 0;
        TLB_FLUSH(vaddr_old);
    }
    
    vmm_map(vaddr_new, paddr, flags);
}

//Pagefault handler
void vmm_guess_and_map(uint64_t cr2, int error_code)
{
    /* Non present page */
    if (error_code & 1)
        vmm_map(cr2, cr2, (error_code & 2) ? (FLAGS_PR | FLAGS_RW) : FLAGS_PR);

    /* CPL = 3 */
    else if (error_code & 4)
        ;

    /* Page fault generated by ab Instruction fetch, only when NX is enabled (Always will be because of limine) */
    else if (error_code & 16)
        ;

    else
        vmm_map(cr2, cr2, FLAGS_PR | FLAGS_RW);
}

page_info_t vmm_dissect_vaddr(uint64_t virt_addr)
{
    page_info_t pg_info;
    const int bitmask = 0x1FF;

    virt_addr >>= 12;

    pg_info.lv1 = virt_addr & bitmask;
    virt_addr >>= 9;

    pg_info.lv2 = virt_addr & bitmask;
    virt_addr >>= 9;

    pg_info.lv3 = virt_addr & bitmask;
    virt_addr >>= 9;

    pg_info.lv4 = virt_addr & bitmask;
    virt_addr >>= 9;

    pg_info.lv5 = virt_addr & bitmask;

    return pg_info;
}
