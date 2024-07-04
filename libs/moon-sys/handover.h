#ifndef BOOT_INFO_HANDOVER_H
#define BOOT_INFO_HANDOVER_H

#include <base/base-types.h>

// clang-format off
#define HANDOVER_MEMMAP_USABLE                 0
#define HANDOVER_MEMMAP_RESERVED               1
#define HANDOVER_MEMMAP_ACPI_RECLAIMABLE       2
#define HANDOVER_MEMMAP_ACPI_NVS               3
#define HANDOVER_MEMMAP_BAD_MEMORY             4
#define HANDOVER_MEMMAP_BOOTLOADER_RECLAIMABLE 5
#define HANDOVER_MEMMAP_KERNEL_AND_MODULES     6
#define HANDOVER_MEMMAP_FRAMEBUFFER            7
// clang-format on

struct$(HandoverModuleEntry, {
    void       *address;
    uint64_t    size;
    string_view cmdline;
});

struct$(HandoverModules, {
    uint64_t            count;
    HandoverModuleEntry modules[16];
});

struct$(MemoryMapEntry, {
    uint64_t base;
    uint64_t length;
    uint64_t type;
});

struct$(HandoverMemoryMap, {
    uint64_t         entry_count;
    MemoryMapEntry **entries;
});

struct$(HandoverFramebuffer, {
    void    *address;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint16_t bpp;
    uint8_t  memory_model;
    uint8_t  red_mask_size;
    uint8_t  red_mask_shift;
    uint8_t  green_mask_size;
    uint8_t  green_mask_shift;
    uint8_t  blue_mask_size;
    uint8_t  blue_mask_shift;
    uint8_t  unused[7];
    uint64_t edid_size;
    void    *edid;
});

struct$(BootHandover, {
#if defined(__x86_64__) || defined(__aarch64__)
    uint64_t rsdp;
#endif
    uint64_t            hhdm_offset;
    uint64_t            kernel_virt_start;
    uint64_t            kernel_phys_start;
    uint64_t            kernel_size;
    HandoverMemoryMap   memory_map;
    HandoverFramebuffer framebuffer;
    HandoverModules     modules;
});

#endif  // BOOT_INFO_HANDOVER_H