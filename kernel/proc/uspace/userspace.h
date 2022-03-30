#ifndef USERSPACE_H
#define USERSPACE_H

#include <amd64/moon.h>

gnu_no_return void enter_ring3(void *addr, size_t stack);

#endif // USERSPACE_H