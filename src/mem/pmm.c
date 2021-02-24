#include "pmm.h"

extern u32int end;
u32int paddr = (u32int)&end;

static u32int bitmap[BITMAP_SZ];

/*
    1. Mark the whole memory area as used/reserved (1)
    2. Ask the BIOS / GRUB which memory area's are available and mark those as free
    3. The BIOS does not know about the kernel or where it resides in memory so we must mark this memory area as used
*/
void pmm_init(struct multiboot *ptr)
{
    struct multiboot_mmap_entry *mmap = ptr->mmap_addr;
    struct multiboot_mmap_entry *mmap_end = (void*) ((u32int*)ptr->mmap_addr + ptr->mmap_length);
    
}