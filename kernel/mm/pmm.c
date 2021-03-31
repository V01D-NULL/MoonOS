#include "pmm.h"
#include <stdint.h>

static uint8_t  *bitmap; //Bitmap buffer / bitmap arena
pmm_bitmap_t pmm_bitmap;

void init_pmm(struct stivale2_struct_tag_memmap *mmap)
{

    debug("Bitmap size: %lld\n", BITMAP_SIZE);
    pmm_bitmap.size = BITMAP_SIZE;

    //Setup bitmap arena
    for (int i = 0; i < mmap->entries; i++)
    {
        struct stivale2_mmap_entry internal_mmap = mmap->memmap[i];
        
        if (internal_mmap.type == STIVALE2_MMAP_USABLE)
        {
            // bitmap[i] = PMM_FREE; //1 - triplefaults
        }
        else {
            bitmap[i] = PMM_USED; //0
        }
    }
    
}

