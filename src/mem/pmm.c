#include "pmm.h"

//Start and end addresses of the kernel specified by the linker and needed to mark that memory as reserved / in use
extern volatile const u32int kernel_start;
extern volatile const u32int kernel_end;

static u32int bitmap[BITMAP_SZ];
static u32int max_phys_blocks = BITMAP_SZ * 32;

/*
    1. Mark the whole memory area as used/reserved (1)
    2. Ask the BIOS / GRUB which memory area's are available and mark those as free
    3. The BIOS does not know about the kernel or where it resides in memory so we must mark this memory area as used
*/

static struct multiboot_mmap *mmap_start;
static struct multiboot_mmap *mmap_end;

void pmm_init(struct multiboot *ptr)
{
    debug("pmm_init: Initialising pmm...\n");
    mmap_start = (struct multiboot_mmap*)&ptr->mmap_addr;
    mmap_end = (void*) ((struct multiboot_mmap*)&ptr->mmap_addr + ptr->mmap_length);

    while (mmap_start < mmap_end)
    {
        if (mmap_start->type == PMM_FREE)
        {
            //Here we have the start and end address of the memory map provided by GRUB
            u32int *addr = mmap_start->base;
            u32int *addr_end = addr + mmap_start->length;

            while (addr < addr_end) {
                pmm_free(addr); //Set's a bit as free (1)
                addr += PMM_4KB; //Page align
            }
        }
        mmap_start++;
    }
    debug("pmm_init: Marking kernel memory as reserved ( %x -> %x )\n", (u32int)&kernel_start, (u32int)&kernel_end);
    u32int* kern_addr = (u32int*)&kernel_start;
    while (kern_addr < (u32int*)&kernel_end)
    {
        pmm_mark_as_used(kern_addr);
        kern_addr += PMM_4KB;
    }
    debug("pmm_init: Max physical block size => %d\n", 524288);
}

//Need to set a bit or something. Read some more wiki entries and look at other projects for help if needed
static void pmm_mark_as_used(u32int bit)
{
    debug("pmm_mark_used: Marking the bit %d as used\npmm_mark_used: The sizeof the paramter in bytes is %d\n", bit, sizeof(bit));
    
    //Mark the "is memory block used" bit in the bitmap array (the memory container if you will) as used.
    bitmap[SET_FLAG(PMM_USED)] = SET_BIT(bit, 5, PMM_USED);
}

void pmm_free(u32int memory)
{
    
}