/**
 * @file bootloader_stivale2.h
 * @author Tim (V01D)
 * @brief Hanldes information from the bootloader
 * @version 0.1
 * @date 2021-04-15
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef BOOTLOADER_STIVALE2_INIT_H
#define BOOTLOADER_STIVALE2_INIT_H

#include <stdint.h>
#include <stddef.h>
#include "../stivale2.h"

/**
 * @brief CPU information provided by the bootloader
 * 
 */
typedef struct boot_cpu {
    uint64_t  processor_count;
    uint32_t  acpi_processor_uid;
    uint32_t  lapic_id;
    uint32_t  bootstrap_processor_lapic_id;
} boot_cpu_t;

/**
 * @brief Memorymap information provided by the bootloader
 * 
 */
typedef struct boot_mmap {
    uint64_t entries;
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint64_t free_ram;
    uint64_t total_ram;
    uint64_t used_ram;
    struct stivale2_mmap_entry *memmap;
} boot_mmap_t;

/**
 * @brief VESA information provided by the bootloader
 * 
 */
typedef struct boot_vesa {
    uint64_t fb_addr;
    uint16_t fb_width;
    uint16_t fb_height;
    uint16_t fb_pitch;
    uint16_t fb_bpp;
} boot_vesa_t;

/**
 * @brief A single structure that combines all bootloader-provided information structs
 * 
 */
typedef struct boot_info {
    boot_cpu_t  cpu;
    boot_vesa_t vesa;
    boot_mmap_t mmap;
} boot_info_t;


#endif // BOOTLOADER_STIVALE2_INIT_H
