/**
 * @file gdt.h
 * @author Tim (V01D)
 * @brief Global Descriptor Table
 * @version 0.1
 * @date 2021-04-16
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef GDT_H
#define GDT_H

#include <stdint.h>
#include "../common.h"
#include "../amd64/validity.h"

/**
 * @brief Defines a memory segment
 * 
 */
struct memory_segment {
    uint16_t limit;
    uint16_t base_low;
    uint8_t  base_mid;
    uint8_t  access;
    uint8_t  gran;
    uint8_t  base_high;
} __pack_bytes;

/**
 * @brief The values used to load the GDT into memory using 'lgdt'
 * 
 */
struct gdtr {
    uint16_t limit;
    uint64_t offset;
} __pack_bytes;

struct gdt_table {
    struct memory_segment gdt_table_memory_segments[5];
    //tss
    struct gdtr gdtr;
} __pack_bytes;

/**
 * @brief Setup and load the GDT
 * 
 */
void init_gdt();

#endif // GDT_H