#include <devices/serial/serial.h>
#include <proc/smp/spinlock.h>
#include <libk/kassert.h>
#include <ds/bitmap.h>
#include <stdint.h>
#include <printk.h>
#include <ktypes.h>
#include <panic.h>
#include "pmm.h"
#include "mm.h"

create_lock("pmm", pmm_lock);

static void *find_first_free_block(void);
static const char *get_mmap_type(int entry);
static void dump_mmap(int entries, struct stivale2_mmap_entry *map);

static struct zone *init_zone(uint64_t base, uint64_t len, int nr)
{
    struct zone *zone = base;

    base = ALIGN_UP(base + sizeof(struct zone));
    len -= PAGE_SIZE;

    boot_term_write("zone: 0x%lX-0x%lX\n", base, base + len);
    zone->start = base;
    zone->len = len;
    zone->name = "Physical memory (Generic-RAM)";
    zone->page_count = len / PAGE_SIZE;
    zone->zone_nr = nr;

    return zone;
}

static struct zone *zone_list;

void pmm_init(struct stivale2_mmap_entry *mmap, int entries)
{
    dump_mmap(entries, mmap);

    // 1. Create zones
    struct zone *tail = NULL;
    for (int i = 0, zone_nr = 0; i < entries; i++)
    {
        if (mmap[i].type != STIVALE2_MMAP_USABLE)
            continue;

        auto zone = init_zone(mmap[i].base, mmap[i].length - 1, zone_nr++);

        if (!tail)
            zone_list = zone;
        else
            list_set_next(zone, list, tail);

        tail = zone;
    }
    list_set_next((struct zone *)NULL, list, tail);

    // 2. Initialize a bitmap for each zone
    list_foreach(zone, list, zone_list)
    {
        size_t bitmap_size_bytes = ALIGN_UP((zone->start + zone->len) / PAGE_SIZE / 8);

        zone->bitmap = (uint8_t *)zone->start;

        debug(true, "Zone#%d: Bitmap stored at %lX-%lX | size: %ldKiB\n", zone->zone_nr, zone->start, zone->start + zone->len - 1, bitmap_size_bytes / 1024);
        memset((void *)zone->bitmap, PMM_FREE, bitmap_size_bytes);

        zone->start += bitmap_size_bytes;
        zone->len -= bitmap_size_bytes;
    }
}

void *pmm_alloc(void)
{
    acquire_lock(&pmm_lock);
    void *block = find_first_free_block();

    if (!block)
        return NULL;

    memset(block, 0, PAGE_SIZE);

    release_lock(&pmm_lock);
    return block;
}

range_t pmm_alloc_range(size_t pages)
{
    return (range_t){};
}

void pmm_free(void *page)
{
    // Don't try to acquire a spin lock if the page is NULL
    if (!page)
        return;

    acquire_lock(&pmm_lock);

    list_foreach(zone, list, zone_list)
    {
        auto zone_sz = zone->start + zone->len;
        auto addr = (uintptr_t) page;

        if (addr > zone_sz)
            continue;

        addr -= zone->start;
        auto bit = (addr / 4096);
        __clear_bit(zone->bitmap, bit);
    }

    release_lock(&pmm_lock);
}

static void *find_first_free_block(void)
{
    list_foreach(zone, list, zone_list)
    {
        for (size_t i = 0; i < zone->page_count; i++)
        {
            if (!__check_bit(zone->bitmap, i))
            {
                __set_bit(zone->bitmap, i);
                // debug(true, "bit: %d\n", i);
                // debug(true, "Zone#%d returning: 0x%lX\n", zone->zone_nr, zone->start + (i * PAGE_SIZE));
                return (void *)(zone->start + (i * PAGE_SIZE));
            }
        }
    }

    return NULL;
}

static const char *get_mmap_type(int entry)
{
    switch (entry)
    {
    case STIVALE2_MMAP_USABLE:
        return "RAM";

    case STIVALE2_MMAP_RESERVED:
        return "Reserved";

    case STIVALE2_MMAP_ACPI_RECLAIMABLE:
        return "ACPI Reclaimable";

    case STIVALE2_MMAP_ACPI_NVS:
        return "ACPI NVS";

    case STIVALE2_MMAP_BAD_MEMORY:
        return "Bad Memory";

    case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE:
        return "Bootloader Reclaimable";

    case STIVALE2_MMAP_KERNEL_AND_MODULES:
        return "Kernel And Modules";

    case STIVALE2_MMAP_FRAMEBUFFER:
        return "Framebuffer";

    default:
        return "???";
    }
}

static void dump_mmap(int entries, struct stivale2_mmap_entry *map)
{
    debug(true, "Full mmap dump:\n");
    for (int i = 0; i < entries; i++)
    {
        boot_term_write("0x%llx - 0x%llx [%d - %s]\n",
                        map[i].base,
                        map[i].base + map[i].length - 1,
                        map[i].type,
                        get_mmap_type(
                            map[i].type));
    }
    boot_term_write("\n");
    debug(false, "\n");
}