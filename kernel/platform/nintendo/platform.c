#include "platform.h"

void platform_init(UNUSED BootHandover *handover) {}
void platform_usleep(UNUSED uint64_t usec) {}

NORETURN void platform_shutdown(void)
{
	i2c_write(3, 0x20, 1 << 0);
	arch_halt_cpu();
	__builtin_unreachable();
}