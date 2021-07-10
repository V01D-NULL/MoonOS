#include "vmm.h"
#include "libk/kassert.h"
#include "../amd64/validity.h"
#include "../drivers/gfx/gfx.h"

static struct page_directory *vmm_page_directory;
//Level4 page table aka the contents of CR3
static uint64_t lv4_pg;

void vmm_invalidate_page_table()
{
    //invlpg
}

void vmm_init()
{
    debug("Reserving memory for the pagetable\n");
    assert(((vmm_page_directory = pmm_alloc()) != NULL)); //There is no need to free this
    debug("Pagetable resides at 0x%x\n", vmm_page_directory);

    debug("Clearing page for vmm_page_directory\n");
    memset(&vmm_page_directory, 0x0, PAGE_SIZE);
    debug("Cleared page for vmm_page_directory\n");

    lv4_pg = cr_read(CR3);
    printk("vmm", "Initialised vmm\n");
}

void vmm_map_page(uint64_t vaddr, int rw)
{
    page_info_t vaddr_dissected = vmm_dissect_vaddr(vaddr);
    pte_t pte_entry = vmm_page_directory->page_tables[vaddr_dissected.lv4];
    if (!pte_entry.present)
    {
        pte_entry.present = 1;
        
        //Sanity check whether rw is 1 or 0, if vaue is bogus we assume a rw page
        pte_entry.readwrite = (rw >= 0 && rw <= 1) ? rw : 1;
        pte_entry.supervisor = 1;
        pte_entry.address = vaddr_dissected.page_offset;
        vmm_set_new_pte(pte_entry, vaddr_dissected.lv4);
    }
}

void vmm_unmap_page(uint64_t vaddr)
{
    page_info_t vaddr_dissected = vmm_dissect_vaddr(vaddr);
}

page_info_t vmm_dissect_vaddr(uint64_t virt_addr)
{
    page_info_t pg_info;
    const int bitmask = 0x1FF;
    
    pg_info.page_offset = virt_addr & 0xfff;
    virt_addr >>= 12;
    
    pg_info.lv1 = virt_addr & bitmask;
    virt_addr >>= 9;
    
    pg_info.lv2 = virt_addr & bitmask;
    virt_addr >>= 9;

    pg_info.lv3 = virt_addr & bitmask;
    virt_addr >>= 9;

    pg_info.lv4 = virt_addr & bitmask;
    
    return pg_info;
}

//Return the address of the level4 page table
static uint64_t vmm_get_lv4()
{
    return lv4_pg;
}

static inline void vmm_set_new_pte(
    pte_t new_pte_entry, 
    uintptr_t level
    )
{
    vmm_page_directory->page_tables[level] = new_pte_entry;
}
