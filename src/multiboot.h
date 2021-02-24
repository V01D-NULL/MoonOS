// Declares the multiboot info structure

#ifndef MULTIBOOT_H
#define MULTIBOOT_H


#include "common.h"

#define MULTIBOOT_FLAG_MEM      0x001
#define MULTIBOOT_FLAG_DEVICE   0x002
#define MULTIBOOT_FLAG_CMDLINE  0x004
#define MULTIBOOT_FLAG_MODS     0x008
#define MULTIBOOT_FLAG_AOUT     0x010
#define MULTIBOOT_FLAG_ELF      0x020
#define MULTIBOOT_FLAG_MMAP     0x040
#define MULTIBOOT_FLAG_CONFIG   0x080
#define MULTIBOOT_FLAG_LOADER   0x100
#define MULTIBOOT_FLAG_APM      0x200
#define MULTIBOOT_FLAG_VBE      0x400

typedef unsigned char           multiboot_uint8_t;
typedef unsigned short          multiboot_uint16_t;
typedef unsigned int            multiboot_uint32_t;
typedef unsigned long long      multiboot_uint64_t;

struct multiboot
{
    u32int flags;
    u32int mem_lower;
    u32int mem_upper;
    u32int boot_device;
    u32int cmdline;
    u32int mods_count;
    u32int mods_addr;
    u32int num;
    u32int size;
    u32int addr;
    u32int shndx;
    u32int mmap_length;
    u32int mmap_addr;
    u32int drives_length;
    u32int drives_addr;
    u32int config_table;
    u32int boot_loader_name;
    u32int apm_table;
    u32int vbe_control_info;
    u32int vbe_mode_info;
    u32int vbe_mode;
    u32int vbe_interface_seg;
    u32int vbe_interface_off;
    u32int vbe_interface_len;
}
__attribute__( ( packed ) );

typedef struct multiboot_header multiboot_header_t;

struct multiboot_mmap_entry
{
    multiboot_uint32_t size;
    multiboot_uint64_t addr;
    multiboot_uint64_t len;
    #define MULTIBOOT_MEMORY_AVAILABLE              1
    #define MULTIBOOT_MEMORY_RESERVED               2
    #define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
    #define MULTIBOOT_MEMORY_NVS                    4
    #define MULTIBOOT_MEMORY_BADRAM                 5
    multiboot_uint32_t type;
} __attribute__((packed));
typedef struct multiboot_mmap_entry multiboot_memory_map_t;

#endif  // MULTIBOOT_H
