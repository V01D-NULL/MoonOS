#include <hal/acpi/tables/hpet/hpet.h>

// TODO: Add more timers

void sleep_init(void)
{
    hpet_init();
}

void usleep(uint64_t usec)
{
    hpet_usleep(usec);
}