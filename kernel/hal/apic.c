#include "apic.h"
#include <stddef.h>

int is_apic_available()
{
    uint32_t eax, unused, edx;
    __get_cpuid(1, &eax, &unused, &unused, &edx);
    return edx & 1 << 22;
}