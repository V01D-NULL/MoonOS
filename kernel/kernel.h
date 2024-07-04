#ifndef KERNEL_H
#define KERNEL_H

#include <moon-sys/handover.h>

#if defined(__x86_64__)
void kern_main(HandoverModules mods);
#else
void kern_main(void);
#endif

#endif  // KERNEL_H
