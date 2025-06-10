#ifndef USERSPACE_H
#define USERSPACE_H

#include <moon.h>

extern NORETURN void arch_enter_userspace(void *addr, size_t stack, int argc,
                                          uint64_t *argv);

#endif  // USERSPACE_H