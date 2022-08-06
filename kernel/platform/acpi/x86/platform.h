#ifndef PLATFORM_H
#define PLATFORM_H

#include "acpi.h"
#include "hpet/hpet.h"

#include <moon-sys/handover.h>

inline void platform_init(BootHandover *handover)
{
	acpi_init(handover->rsdp);
	hpet_init();
}

inline void platform_usleep(uint64_t usec)
{
	hpet_usleep(usec);
}

#endif // PLATFORM_H