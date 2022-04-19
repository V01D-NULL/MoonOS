#ifndef KERNEL_H
#define KERNEL_H

#include <boot/boot.h>

/**
 * @brief This is where all the magic happens
 * 
 * @param[in] bootvars Essential information passed to kmain() from the bootloader
 */
void kmain(BootContext *bootvars, struct stivale2_struct_tag_modules *mods);

#endif // KERNEL_H
