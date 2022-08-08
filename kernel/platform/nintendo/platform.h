#ifndef PLATFORM_H
#define PLATFORM_H

#include <moon-sys/handover.h>
#include <moon.h>

inline void platform_init(UNUSED BootHandover *handover) {}

inline void platform_usleep(UNUSED uint64_t usec) {}

#endif // PLATFORM_H