#ifndef PLATFORM_H
#define PLATFORM_H

#include <cpu.h>
#include <drivers/i2c.h>
#include <moon-sys/handover.h>
#include <moon.h>

void platform_init(UNUSED BootHandover handover);
void platform_usleep(UNUSED uint64_t usec);

NORETURN void platform_shutdown(void);

#endif  // PLATFORM_H