#ifndef KERNEL_H
#define KERNEL_H

#include <boot/boot.h>

void kmain(BootContext *bootvars, struct stivale2_struct_tag_modules *mods);

#endif // KERNEL_H
