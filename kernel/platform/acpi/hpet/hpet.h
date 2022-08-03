#ifndef HPET_H
#define HPET_H

#include <amd64/moon.h>
#include <platform/acpi/tables/sdt.h>

void hpet_init(void);
void hpet_usleep(uint64_t usec);

#endif // HPET_H