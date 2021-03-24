#ifndef BOOTLOADER_STIVALE2_INIT_H
#define BOOTLOADER_STIVALE2_INIT_H

#include <stdint.h>
#include <stddef.h>
#include "../stivale2.h"
#include "../drivers/vga/vga.h"

//Init everything (yes, everything)
void bootloader_stivale2_init_all();

//Init framebuffer
void bootloader_stivale2_init_FB();

//Inint MemoryMap
void bootloader_stivale2_init_mmap(struct stivale2_struct *stivale2_struct);

//Init SMP information
void bootloader_stivale2_init_smp(struct stivale2_struct *stivale2_struct);

#endif // BOOTLOADER_STIVALE2_INIT_H