#include "pmm.h"
#include <stdint.h>
#include "../drivers/gfx/gfx.h"
#include "../util/ptr.h"
#include "../panic.h"
#include "memdefs.h"

pmm_t pmm;
size_t highest_page;
mmap_t phys_mmap;
struct usable_mem_range umem_range;

// Note: This function assumes the address has not been converteted to an index yet
static inline bool pfa_alloc_allowed(void *addr)
{
    //Todo: Check if address is within memory range marked as usable

    return (pmm.bitmap_manager.get(pmm.bitmap_manager.pool, GENERIC_CAST(uintptr_t, PAGE_2_BIT(addr)) / PAGE_SIZE) == BITMAP_USED);
}

static inline void pfa_alloc(void *addr)
{
    uintptr_t index = GENERIC_CAST(uintptr_t, PAGE_2_BIT(addr));

    if (!pfa_alloc_allowed(addr))
        return;

    pmm.bitmap_manager.set(pmm.bitmap, index);
}

static inline void pfa_free(void *addr)
{
    uintptr_t index = GENERIC_CAST(uintptr_t, PAGE_2_BIT(addr));

    if (!pfa_alloc_allowed(addr))
        return;

    pmm.bitmap_manager.clear(pmm.bitmap, index);
}

bool in_range(void *_address)
{
    uint64_t address = GENERIC_CAST(uint64_t, _address);

    if (address >= phys_mmap.abs_base && address <= phys_mmap.abs_top)
        return true;

    return false;
}

const char *get_mmap_type(int entry);
void dump_mmap()
{
    debug(true, "Full mmap dump:\n");
    for (int i = 0; i < phys_mmap.entries; i++)
    {
        debug(true, "0x%llx - 0x%llx [%d - %s]\n",
              phys_mmap.map[i].base,
              phys_mmap.map[i].base + phys_mmap.map[i].length - 1,
              phys_mmap.map[i].type,
              get_mmap_type(
                  phys_mmap.map[i].type));
    }
    debug(false, "\n");
}

const char *get_mmap_type(int entry)
{
    switch (entry)
    {
    case STIVALE2_MMAP_USABLE:
        return "Stivale2 mmap usable";

    case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE:
        return "Stivale2 bootloader reclaimable";

    case STIVALE2_MMAP_RESERVED:
        return "Stivale2 mmap reserved";

    case STIVALE2_MMAP_KERNEL_AND_MODULES:
        return "Stivale2 mmap kernel and modules";

    case STIVALE2_MMAP_ACPI_RECLAIMABLE:
        return "Stivale2 mmap acpi reclaimable";

    case STIVALE2_MMAP_BAD_MEMORY:
        return "Stivale2 mmap bad memory";

    case STIVALE2_MMAP_FRAMEBUFFER:
        return "Stivale2 mmap fb";

    default:
        return "???";
    }
}

void pmm_init(struct stivale2_mmap_entry *mmap, int entries)
{
    phys_mmap.entries = entries;
    phys_mmap.map = mmap;
    phys_mmap.abs_base = mmap[0].base;

    size_t top = 0;

    for (int i = 0; i < entries; i++)
    {
        if (mmap[i].type != STIVALE2_MMAP_USABLE)
            continue;

        top = mmap[i].base + mmap[i].length;
        if (top > highest_page)
            highest_page = top;
    }

    //Step 1. Calculate the size of the bitmap
    phys_mmap.abs_top = top;
    dump_mmap();

    //highest_page / PAGE_SIZE = amount of pages in total, and higest_page / PAGE_SIZE / 8 will get the amount of bytes the bitmap will occupy since 1 byte = 8 bits
    size_t bitmap_size_bytes = ALIGN_UP(highest_page / PAGE_SIZE / 8);
    pmm.size = bitmap_size_bytes;

    //Step 2. Find a big enough block to host the bitmap
    for (int i = 0; i < entries; i++)
    {
        if (mmap[i].type != STIVALE2_MMAP_USABLE)
            continue;

        //We found a large enough block of memory to host the bitmap!
        if (mmap[i].length >= bitmap_size_bytes)
        {
            debug(true, "Found a big enough block of memory to host the bitmap (size: %ld), size : %ldKiB\n", mmap[i].length, bitmap_size_bytes / 1024);

            pmm.bitmap = GENERIC_CAST(bitmap_size_type PTR, to_virt(mmap[i].base));
            pmm.bitmap_manager = bitmap_init(pmm.bitmap, bitmap_size_bytes);

            debug(true, "(phys) Bitmap stored at %lX-%lX\n", mmap[i].base, mmap[i].base + mmap[i].length - 1);
            memset(GENERIC_CAST(void PTR, pmm.bitmap_manager.pool), PMM_FULL, bitmap_size_bytes);
            
            pfa_free(VAR_TO_VOID_PTR(uintptr_t, to_virt(mmap[i].base + mmap[i].length)));
                
            break;
        }
    }

    //Sanity check, if this fails then there isn't enough memory to store the bitmap
    if (pmm.bitmap == GENERIC_CAST(bitmap_size_type PTR, PMM_FREE))
    {
        panic("Couldn't store bitmap\n");
    }

    //Step 3.
    for (int i = 0; i < entries; i++)
    {
        if (mmap[i].type != STIVALE2_MMAP_USABLE)
            continue;

        debug(true, "Freeing %lx\n", to_virt(mmap[i].base));
        pfa_free(VAR_TO_VOID_PTR(uintptr_t, to_virt(mmap[i].base)));
    }

    pfa_free(VAR_TO_VOID_PTR(uintptr_t, to_virt(0))); //Bit 0 is a nullptr
    bitmap_log_all_bits(pmm.bitmap_manager);
    printk("pmm", "Initialised pmm\n");
}

/**
 * @brief Alloc n number of pages & return a pointer to the allocated memory
 * 
 * @return void* Returns NULL on error and a pointer to the allocated memory on success.
 */
void *pmm_alloc()
{
    void PTR block = find_first_free_block();

    if (block == PMM_INVALID)
        return NULL;

    debug(true, "Attempting to alloc address: %llX\n", (uintptr_t)block);

    if (!in_range(block))
    {
        debug(true, "Physical address 0x%llx is out of bounds! Memory map spans from 0x%llx - 0x%llx\n",
              GENERIC_CAST(size_t, block),
              phys_mmap.abs_base,
              phys_mmap.abs_top);
        return NULL;
    }

    memset(block, 0, PAGE_SIZE);
    pfa_alloc(VAR_TO_VOID_PTR(uintptr_t, to_virt(GENERIC_CAST(uintptr_t, block))));
    return block;
}

/**
 * @brief Free a bit in the bitmap / free a page (using an address returned by pmm_alloc)
 * 
 * @param page 
 * @return int32_t Returns PMM_INVALID on error and 0 on success
 */
int32_t pmm_free(void *page)
{
    uintptr_t index = GENERIC_CAST(uintptr_t, PAGE_2_BIT(page));
    (void)index;
    pfa_free(VAR_TO_VOID_PTR(uintptr_t, page));

    return 0;
}

/**
 * @brief Find the first free bit in the bitmap and return it's index
 * 
 * @return uint32_t Returns PMM_INVALID on error and the address of the first free block in the bitmap on success
 */
void *find_first_free_block()
{
    for (int i = 0; i < PAGE_2_BIT(highest_page); i++)
    {
        if ((pmm.bitmap[i / 8] >> (i % 8) & 1))
        {
            return VAR_TO_VOID_PTR(size_t, BIT_2_PAGE(i));
        }
    }
    return PMM_INVALID;
}
