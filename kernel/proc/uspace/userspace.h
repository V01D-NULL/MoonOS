#ifndef USERSPACE_H
#define USERSPACE_H

#include <moon.h>

extern NORETURN void arch_enter_userspace(void *addr, size_t stack);

#endif // USERSPACE_H