#include "platform.h"

void platform_init(BootHandover *handover)
{
	acpi_init(handover->rsdp);
	hpet_init();
}

void platform_usleep(uint64_t usec)
{
	hpet_usleep(usec);
}

NORETURN void platform_shutdown(void)
{
	arch_halt_cpu();
	__builtin_unreachable();
}
