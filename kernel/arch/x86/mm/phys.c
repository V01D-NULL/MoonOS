#define PR_MODULE "phys"
#include "phys.h"
#include <base/align.h>
#include <base/assert.h>
#include <base/base-types.h>
#include <base/mem.h>
#include <mm/buddy.h>
#include <moon-ds/bitmap.h>
#include <moon-io/serial.h>
#include <moon-sys/spinlock.h>
#include <panic.h>
#include <printk.h>
#define BUDDY_ALLOC_IMPLEMENTATION
#include "zone.h"

create_lock("pmm", pmm_lock);

static void       *find_first_free_block(int sz);
static string_view get_mmap_type(int entry);
static void        dump_mmap(HandoverMemoryMap mmap);

static struct zone *init_zone(uint64_t base, uint64_t len, int nr)
{
    struct zone *zone = (struct zone *)base;

    base = ALIGN_UP(base + sizeof(struct zone));
    len -= PAGE_SIZE;

    trace(TRACE_BOOT, "zone: 0x%lX-0x%lX\n", base, base + len);
    zone->start      = base;
    zone->len        = len;
    zone->name       = "Physical memory (Generic-RAM)";
    zone->page_count = len / PAGE_SIZE;
    zone->zone_nr    = nr;
    zone->buddy      = buddy_embed((void *)base, len);

    return zone;
}

static struct zone *zone_list;

void init_phys_allocator(HandoverMemoryMap mmap)
{
    dump_mmap(mmap);

    // Create zones and initialize buddy allocators arenas
    struct zone *tail = NULL;
    for (int i = 0, zone_nr = 0; i < mmap.entry_count; i++)
    {
        if (mmap.entries[i]->type != HANDOVER_MEMMAP_USABLE ||
            mmap.entries[i]->length ==
                PAGE_SIZE)  // The zone itself occupies one page (4kib), so we
                            // skip small memory regions. Future work: implement
                            // a way to merge small memory regions and/or ensure
                            // zones have a smaller memory overhead.
            continue;

        auto zone = init_zone(
            va(mmap.entries[i]->base), mmap.entries[i]->length - 1, zone_nr++);

        if (!tail)
            zone_list = zone;
        else
            list_set_next(zone, list, tail);

        tail = zone;
    }
    list_set_next((struct zone *)NULL, list, tail);
}

void *arch_alloc_page_sz(int sz)
{
    acquire_lock(&pmm_lock);
    void *block = find_first_free_block(sz);

    if (!block)
        return NULL;

    memset(block, 0, PAGE_SIZE);

    release_lock(&pmm_lock);
    return block;
}

void arch_free_page(void *page)
{
    if (!page)
        return;

    acquire_lock(&pmm_lock);

    list_foreach(zone, list, zone_list)
    {
        auto zone_end = zone->start + zone->len;
        auto addr     = (uintptr_t)page;

        if (addr >= zone->start && addr <= zone_end)
        {
            buddy_free(zone->buddy, page);
            break;
        }
    }

    release_lock(&pmm_lock);
}

static void *find_first_free_block(int sz)
{
    list_foreach(zone, list, zone_list)
    {
        if (zone->len < sz)
            continue;

        const void *ptr = buddy_malloc(zone->buddy, sz);
        if (unlikely(!ptr))
            continue;

        return ptr;
    }

    return NULL;
}

static string_view get_mmap_type(int entry)
{
    switch (entry)
    {
        case HANDOVER_MEMMAP_USABLE:
            return "RAM";

        case HANDOVER_MEMMAP_RESERVED:
            return "Reserved";

        case HANDOVER_MEMMAP_ACPI_RECLAIMABLE:
            return "ACPI Reclaimable";

        case HANDOVER_MEMMAP_ACPI_NVS:
            return "ACPI NVS";

        case HANDOVER_MEMMAP_BAD_MEMORY:
            return "Bad Memory";

        case HANDOVER_MEMMAP_BOOTLOADER_RECLAIMABLE:
            return "Bootloader Reclaimable";

        case HANDOVER_MEMMAP_KERNEL_AND_MODULES:
            return "Kernel And Modules";

        case HANDOVER_MEMMAP_FRAMEBUFFER:
            return "Framebuffer";

        default:
            return "???";
    }
}

static void dump_mmap(HandoverMemoryMap mmap)
{
    debug(true, "Full mmap dump:\n");
    for (int i = 0; i < mmap.entry_count; i++)
    {
        debug(false,
              "0x%llx - 0x%llx [%d - %s]\n",
              mmap.entries[i]->base,
              mmap.entries[i]->base + mmap.entries[i]->length - 1,
              mmap.entries[i]->type,
              get_mmap_type(mmap.entries[i]->type));
    }
    debug(false, "\n");
}