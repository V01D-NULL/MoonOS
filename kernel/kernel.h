#ifndef KERNEL_H
#define KERNEL_H

#include <boot/boot.h>

void kern_main(BootContext *bootvars, struct stivale2_struct_tag_modules *mods);

#endif // KERNEL_H
