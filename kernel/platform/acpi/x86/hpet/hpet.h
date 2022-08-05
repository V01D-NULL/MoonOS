#ifndef HPET_H
#define HPET_H

#include <moon.h>
#include <platform/acpi/x86/tables/sdt.h>

void hpet_init(void);
void hpet_usleep(uint64_t usec);

#endif // HPET_H