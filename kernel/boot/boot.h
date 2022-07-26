#ifndef BOOT_H
#define BOOT_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stivale2.h>

/**
 * Note: BootContext* does not follow the traditional naming convention.
 *       This is an attempt to completely isolate it from the kernel since all it does is provide information from the bootloader.
 *       In other words, I have almost nothing to do with getting this information. As such it should be clearly distinct from the kernel.
 * 
 */
typedef struct {
    uint64_t  processor_count;
    uint32_t  acpi_processor_uid;
    uint32_t  lapic_id;
    uint32_t  bootstrap_processor_lapic_id;
    struct stivale2_smp_info *smp_info;
} BootContextSmp;

typedef struct {
    uint64_t rsdp_address;
} BootContextRsdp;

typedef struct  {
    uint64_t fb_addr;
    uint16_t fb_width;
    uint16_t fb_height;
    uint16_t fb_pitch;
    uint16_t fb_bpp;
} BootContextFramebuffer;

typedef struct
{
    BootContextSmp cpu;
    BootContextRsdp rsdp;
    BootContextFramebuffer fb;
    struct stivale2_struct_tag_memmap *mmap;
    bool is_uefi;
    uint64_t rbp;
} BootContext;

BootContext BootContextGet(void);

#endif // BOOT_H
