#include <platform.h>
#include "time/sleep.h"

void usleep(uint64_t usec)
{
	platform_usleep(usec);
}