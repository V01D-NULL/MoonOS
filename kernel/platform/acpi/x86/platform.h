#ifndef PLATFORM_H
#define PLATFORM_H

#include "acpi.h"
#include "hpet/hpet.h"

#include <cpu.h>
#include <moon-sys/handover.h>

void          platform_init(BootHandover handover);
void          platform_usleep(uint64_t usec);
NORETURN void platform_shutdown(void);

#endif  // PLATFORM_H