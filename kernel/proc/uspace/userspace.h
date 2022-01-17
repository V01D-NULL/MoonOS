#ifndef USERSPACE_H
#define USERSPACE_H

#include <amd64/moon.h>

gnu_export void jump_to_user_address(void *addr);

#endif // USERSPACE_H