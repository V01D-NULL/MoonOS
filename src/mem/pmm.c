#include "pmm.h"

//Start and end addresses of the kernel specified by the linker and needed to mark that memory as reserved / in use
extern const void kernel_start;
extern const void kernel_end;

static u32int bitmap[BITMAP_SZ];

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
    mmap_start = ptr->mmap_addr;
    mmap_end = (void*) (ptr->mmap_addr + ptr->mmap_length);
    
    memset((u8int*)bitmap, 0, sizeof(bitmap));
    debug("pmm_init: mmap_start => %x -- mmap_end => %x\n", mmap_start, mmap_end);
    
    while (mmap_start < mmap_end)
    {
        if (mmap_start->type == PMM_FREE)
        {
            //Here we have the start and end address of the memory map provided by GRUB
            u32int addr = mmap_start->base;
            u32int addr_end = addr + mmap_start->length;
            debug("pmm_init: addr => %x -- addr_end => %x\n", addr, addr_end);
            while (addr < addr_end) {
                pmm_free((void*)addr); //Set's a bit as free (1)
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
    
}

//A relatively safe implementation of the mark_as_used bitmap function.
static void pmm_mark_as_used(void *page)
{
    // u32int in_case_of_kernel_panic = 0;

    // debug("pmm_mark_as_used: page => %x\n", (u32int*)page);
    // debug("pmm_mark_as_used: idx => %x\n", PMM_IDX(page));
    u32int idx = (u32int)page/4096;

    //Mark the "is memory block used" bit in the bitmap array (the memory container if you will) as used.
    bitmap[idx/32] &= ~(1 << (idx % 32));
    // if (in_case_of_kernel_panic != PMM_USED)
    // {
    //     int __idx__ = bitmap[PMM_IDX(page)/32];
    //     debug("<<Before kernel panic>>: bitmap[idx/32] => %x\n<<Before kernel panic>>: idx => %x\n<<Before kernel panic>>: The result of the bit operation should be %d, but it is %d\n", __idx__, PMM_IDX(page), PMM_USED, in_case_of_kernel_panic);
    //     PANIC("bitmap[idx/32] &= ~(1 << (idx % 32)) FAILED! Value MUST be 0, but it isnt!");
    // }    
}

void pmm_free(void *page)
{
    // debug("pmm_free: page => %x\n", (u32int*)page);
    // debug("pmm_free: idx => %x\n", PMM_IDX(page));
    u32int idx = (u32int)page/4096;
    bitmap[idx/32] |= (1 << (idx % 32));
}