#include "pmm.h"
#include <stdint.h>
#include <drivers/vbe/vbe.h>
#include <util/ptr.h>
#include <util/iterator.h>
#include <libk/kassert.h>
#include <panic.h>
#include <printk.h>
#include <liballoc/bitmap.h>
#include <drivers/io/serial.h>
#include "memdefs.h"

static mmap_t phys_mmap;
static struct stivale2_mmap_entry *map;
size_t highest_page;
static uint8_t PTR bitmap;

#define bset(bit, cmd)  (cmd == BIT_SET) ? (bitmap[bit / 8] |= (1 << (bit % 8))) : (bitmap[bit / 8] &= ~(1 << (bit % 8)))
#define btest(bit)      ((bitmap[bit / 8] & (1 << (bit % 8))))

static inline void *find_free_block_at(size_t offset);

// Note: This function assumes the address has not been converteted to an index yet
static inline bool pfa_alloc_allowed(void *addr)
{
    if (!in_range(addr))
        return false;

    return btest(GENERIC_CAST(uintptr_t, PAGE_2_BIT(addr))) == 0;
}

static inline void pfa_alloc(void *addr)
{
    uintptr_t index = GENERIC_CAST(uintptr_t, PAGE_2_BIT(addr));

    if (!pfa_alloc_allowed(addr))
        return;

    bset(index, BIT_SET);
}

static inline void pfa_free(void *addr)
{
    uintptr_t index = GENERIC_CAST(uintptr_t, PAGE_2_BIT(addr));
    bset(index, BIT_CLEAR);
}

static inline void pfa_free_multiple(size_t address_base, size_t n)
{
    size_t len = address_base + n;

    for (int i = address_base; i < len; i++)
    {
        bset(i, BIT_CLEAR);
    }
}

bool in_range(void *_address)
{
    uint64_t address = GENERIC_CAST(uint64_t, _address);

    for (int i = 0; i < phys_mmap.entries; i++)
    {
        // if (map[i].type != STIVALE2_MMAP_USABLE)
        // {
        //     continue;
        // }
        
        if (address >= map[i].base && address <= (map[i].base + map[i].length) - 1)
        {
            return true;
        }
    }

    return false;
}

const char *get_mmap_type(int entry);
void dump_mmap()
{
    debug(true, "Full mmap dump:\n");
    for (int i = 0; i < phys_mmap.entries; i++)
    {
        debug(true, "0x%llx - 0x%llx [%d - %s]\n",
              map[i].base,
              map[i].base + map[i].length - 1,
              map[i].type,
              get_mmap_type(
                  map[i].type));
    }
    debug(false, "\n");
}

const char *get_mmap_type(int entry)
{
    switch (entry)
    {
    case STIVALE2_MMAP_USABLE:
        return "Stivale2 mmap usable";

    case STIVALE2_MMAP_RESERVED:
        return "Stivale2 mmap reserved";

    case STIVALE2_MMAP_ACPI_RECLAIMABLE:
        return "Stivale2 mmap acpi reclaimable";
    
    case STIVALE2_MMAP_ACPI_NVS:
        return "Stivale2 mmap acpi nvs";

    case STIVALE2_MMAP_BAD_MEMORY:
        return "Stivale2 mmap bad memory";

    case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE:
        return "Stivale2 bootloader reclaimable";

    case STIVALE2_MMAP_KERNEL_AND_MODULES:
        return "Stivale2 mmap kernel and modules";

    case STIVALE2_MMAP_FRAMEBUFFER:
        return "Stivale2 mmap fb";

    default:
        return "???";
    }
}

void pmm_init(struct stivale2_mmap_entry *mmap, int entries)
{
    phys_mmap.entries = entries;
    map = mmap;
    phys_mmap.abs_base = mmap[0].base;

    size_t top = 0;

    for (int i = 0; i < entries; i++)
    {
        if (mmap[i].type != STIVALE2_MMAP_USABLE)
            continue;

        top = mmap[i].base + mmap[i].length - 1;
        if (top > highest_page)
            highest_page = top;
    }

    //Step 1. Calculate the size of the bitmap
    phys_mmap.abs_top = mmap[entries - 1].base + mmap[entries - 1].length - 1;
    dump_mmap();

    //highest_page / PAGE_SIZE = amount of pages in total, and higest_page / PAGE_SIZE / 8 will get the amount of bytes the bitmap will occupy since 1 byte = 8 bits
    size_t bitmap_size_bytes = ALIGN_UP(highest_page / PAGE_SIZE / 8);

    //Step 2. Find a big enough block to host the bitmap
    for (int i = 0; i < entries; i++)
    {
        if (mmap[i].type != STIVALE2_MMAP_USABLE)
            continue;

        //We found a large enough block of memory to host the bitmap!
        if (mmap[i].length >= bitmap_size_bytes)
        {
            debug(true, "Found a big enough block of memory to host the bitmap (size: %ld), size : %ldKiB\n", mmap[i].length, bitmap_size_bytes / 1024);

            bitmap = GENERIC_CAST(bitmap_size_type PTR, mmap[i].base);

            debug(true, "(phys) Bitmap stored at %lX-%lX\n", mmap[i].base, mmap[i].base + mmap[i].length - 1);
            memset(VAR_TO_VOID_PTR(uint8_t PTR, bitmap), PMM_FULL, bitmap_size_bytes);
            
            mmap[i].base += bitmap_size_bytes;
            mmap[i].length -= bitmap_size_bytes;

            break;
        }
    }

    //Sanity check, if this fails then there isn't enough memory to store the bitmap
    if (bitmap == GENERIC_CAST(bitmap_size_type PTR, PMM_FREE))
    {
        panic("Couldn't store bitmap");
    }

    //Step 3.
    size_t mem_lo = 0;
    size_t mem_hi = 0;
    for (int i = 0; i < entries; i++)
    {
        if (mmap[i].type != STIVALE2_MMAP_USABLE)
            continue;

        mem_lo = mmap[i].base;
        mem_hi = mmap[i].base + mmap[i].length - 1;
        
        debug(true, "pmm_init: Freeing %lx-%lx | Pages: %ld\n", mem_lo, mem_hi, get_page_count((void PTR) mem_lo, mem_hi));
        size_t len  = PAGE_2_BIT(mmap[i].length);
        size_t page = PAGE_2_BIT(mmap[i].base);
        pfa_free_multiple(page, len);
    }

    // printk("pmm", "Initialized pmm\n");
}

/**
 * @brief Alloc the first free page found & return a pointer to the beginning of it
 * 
 * @return void* Returns NULL on error and a pointer to the allocated memory on success.
 */
void *pmm_alloc()
{
    void PTR block = find_first_free_block();

    if (block == PMM_INVALID)
        return NULL;

    if (!in_range(block))
    {
        debug(true, "Physical address 0x%llx is out of bounds! Memory map spans from 0x%llx - 0x%llx\n",
              GENERIC_CAST(size_t, block),
              phys_mmap.abs_base,
              phys_mmap.abs_top);
        return NULL;
    }

    memset(block, 0, PAGE_SIZE);
    pfa_alloc(VAR_TO_VOID_PTR(uintptr_t, (GENERIC_CAST(uintptr_t, block))));
    // return block;
    return VAR_TO_VOID_PTR(uintptr_t, to_virt(GENERIC_CAST(uintptr_t, block)));
}

void *pmm_alloc_any(void *addr)
{
    void PTR block = find_free_block_at(PAGE_2_BIT(addr));
    if (block == NULL)
        return NULL;

    /* pfa_alloc() checks if the memory is marked as usable, for that reason it isn't used here */
    uintptr_t index = GENERIC_CAST(uintptr_t, PAGE_2_BIT(addr));
    bset(index, BIT_SET);

    return block;
}

range_t pmm_alloc_range(size_t pages)
{   
    uint64_t* base = (uint64_t*)pmm_alloc();
    uint64_t* top = (uint64_t*)0;

    for (size_t i = 0; i < pages; i++)
    {
        assert((top = (uint64_t*)pmm_alloc()) != NULL);
    }
    
    return (range_t) {(size_t)base, (size_t)top};
}

//Find a memory entry by it's tag up to `retries` times
struct memtag_range pmm_find_tag(size_t tag, int retries)
{
    struct memtag_range result;
    size_t counter = 0;
    
    for (size_t i = 0; i < phys_mmap.entries; i++)
    {
        if (map[i].type != tag)
            continue;
        
        if (counter == retries)
            break;

        result.base  = map[i].base;
        result.size = (result.base + map[i].length) - 1;
        counter++;    
    }

    return result;
}

/**
 * @brief Free a bit in the bitmap / free a page (using an address returned by pmm_alloc)
 * 
 * @param page 
 * @return int32_t Returns 1 on error and 0 on success
 */
void pmm_free(void *page)
{
    memset(page, 0, PAGE_SIZE);
    pfa_free(page);
}

/**
 * @brief Find the first free bit in the bitmap and return it's index
 * 
 * @return void* Returns PMM_INVALID on error and the address of the first free block in the bitmap on success
 */
void *find_first_free_block()
{
    for (size_t i = 0; i < PAGE_2_BIT(highest_page); i++)
    {   
        if (btest(i) == 0)
        {
            return VAR_TO_VOID_PTR(size_t, BIT_2_PAGE(i));
        }
    }
    
    return PMM_INVALID;
}

static inline void *find_free_block_at(size_t offset)
{
    return (btest(offset) == 0 ? VAR_TO_VOID_PTR(size_t, BIT_2_PAGE(offset)) : NULL);
}