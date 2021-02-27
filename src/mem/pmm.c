#include "pmm.h"

//Start and end addresses of the kernel specified by the linker and needed to mark that memory as reserved / in use
extern const void kernel_start;
extern const void kernel_end;

static u32int bitmap[BITMAP_SZ] = {0};
static u32int in_case_of_kernel_panic = 0;
static u64int total_memory = 0;

/*
    1. Mark the whole memory area as free/reserved (1)
    2. Ask the BIOS / GRUB which memory area's are available and mark those as free
    3. The BIOS does not know about the kernel or where it resides in memory so we must mark this memory area as used
*/

void pmm_init(struct multiboot *ptr)
{
    monitor_write("Initialising physical memory manager\n", true, false);
    debug("pmm_init: Initialising pmm...\n");
    struct multiboot_mmap *mmap_start = ptr->mmap_addr;
    struct multiboot_mmap *mmap_end = ptr->mmap_addr + ptr->mmap_length;

    /* Step 1. Set the entire memory layout (also called arena in this case) to used/reserved */
    memset((u8int*)bitmap, PMM_USED, sizeof(bitmap));
    debug("pmm_init: mmap_start => %x -- mmap_end => %x\n", mmap_start, mmap_end);
    
    /* Step 2. Find the memory areas (technically I should say pages since Validity aligns them to a 4KB boundary thus setting a bit in each page) which have been marked as free (by GRUB/the BIOS) and tell Validity to free that page */
    while (mmap_start < mmap_end)
    {
        if (mmap_start->type == PMM_FREE)
        {
            //Here we have the start and end address of the memory map provided by GRUB
            u32int addr = mmap_start->base;
            u32int addr_end = addr + mmap_start->length;
            debug("pmm_init: addr => %x -- addr_end => %x\n", addr, addr_end);
            while (addr < addr_end) {
                pmm_free((void*)&addr); //Set's a bit as free (1) - I spent hours trying to figure out why pmm_free won't free stuff. Found out that you must pass addr as a reference, duh... Maybe this helps you out if you are failing with bitmaps like I was :)
                addr += PMM_4KB; //Page align
            }
        }
        mmap_start++;
    }

    /* Step 3. Mark the kernel memory as used/reserved since GRUB won't mark this memory range as used */
    debug("pmm_init: Marking kernel memory as reserved ( %x -> %x )\n", (u32int)&kernel_start, (u32int)&kernel_end);
    u32int *kern_addr = (u32int*)&kernel_start;
    while (kern_addr < (u32int*)&kernel_end)
    {
        pmm_mark_as_used((void*)&kern_addr);
        kern_addr += PMM_4KB;
    }

    /* Step 3.1. Now just mark the modules as used/reserved aswell. */

    monitor_write("Initialised physical memory manager\n", false, true);
}

//A relatively safe implementation of the mark_as_used bitmap function.
static void pmm_mark_as_used(void *page)
{
    in_case_of_kernel_panic = PMM_FREE; //Assume something went wrong by default. The world is bad, lol :D                                  (I'm kidding karen)

    debug("pmm_mark_as_used: page => %x\n", (u32int*)page);
    debug("pmm_mark_as_used: idx => %x\n", PMM_IDX(page));
    u32int idx = PMM_IDX(page);
    
    //Mark the "is memory block used" bit in the bitmap array (the memory container if you will) as used.
    in_case_of_kernel_panic = (bitmap[idx/32] &= ~(1 << (idx % 32)));
    
    if (in_case_of_kernel_panic != PMM_USED)
    {
        int __idx__ = bitmap[PMM_IDX(page)/32];
        debug("<<Before kernel panic>>: bitmap[idx/32] => %x\n<<Before kernel panic>>: idx => %x\n<<Before kernel panic>>: The result of the bit operation should be %d, but it is %d\n", __idx__, PMM_IDX(page), PMM_USED, in_case_of_kernel_panic);
        PANIC("bitmap[idx/32] &= ~(1 << (idx % 32)) FAILED! Value MUST be 0, but it isnt!");
    }
}

void pmm_free(void *page)
{
    //All hail the preprocessor gods.
    //This will make pmm_free really slow upon initializing large blocks of memory (such as free calls in pmm_init)
    #if defined DEBUG
        debug("pmm_free: page => %x\n", (u32int*)page);
        debug("pmm_free: idx => %x\n", PMM_IDX(page));
    #endif

    u32int idx = PMM_IDX(page);
    bitmap[idx/32] |= (1 << (idx % 32));
    
    #if defined DEBUG
        int __idx__ = bitmap[idx/32];
        debug("pmm_free: bitmap[idx/32] => %x\n", __idx__);
    #endif
}

//Count the total ammount of ram (on one stick only)
u64int total_ram(struct multiboot *ptr)
{
    struct multiboot_mmap *mmap_start = ptr->mmap_addr;
    struct multiboot_mmap *mmap_end = ptr->mmap_addr + ptr->mmap_length;
    
    while (mmap_start < mmap_end)
    {
        if (mmap_start->type == PMM_FREE)
        {
            u32int addr = mmap_start->base;
            u32int addr_end = addr + mmap_start->length;
            
            while (addr < addr_end) {
                addr += PMM_4KB; //Page align
                total_memory += addr;
            }
        }
        mmap_start++;
    }
    kprintf("%d %x\n", total_memory, total_memory);
    total_memory = total_memory / 4096 * 1024;
    kprintf("Ram memory total: %x\n", total_memory);
    return total_memory;
}