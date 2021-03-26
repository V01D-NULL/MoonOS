#ifndef BOOTLOADER_STIVALE2_INIT_H
#define BOOTLOADER_STIVALE2_INIT_H

#include <stdint.h>
#include <stddef.h>
#include "../stivale2.h"

typedef struct boot_cpu {
    uint64_t  processor_count;
    uint32_t  acpi_processor_uid;
    uint32_t  lapic_id;
    uint32_t  bootstrap_processor_lapic_id;
} boot_cpu_t;

typedef struct boot_mmap {
    uint64_t entries;
    uint64_t base;
    uint64_t length;
    uint32_t type;
} boot_mmap_t;

typedef struct boot_vesa {
    uint64_t fb_addr;
    uint16_t fb_width;
    uint16_t fb_height;
    uint16_t fb_pitch;
    uint16_t fb_bpp;
} boot_vesa_t;

typedef struct boot_info {
    boot_cpu_t  cpu;
    boot_vesa_t vesa;
    boot_mmap_t mmap;
} boot_info_t;


#endif // BOOTLOADER_STIVALE2_INIT_H
