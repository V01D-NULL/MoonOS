#ifndef KERNEL_H
#define KERNEL_H

#include <moon-sys/handover.h>

void kern_main(BootHandover *handover, struct stivale2_struct_tag_modules *mods);

#endif // KERNEL_H
