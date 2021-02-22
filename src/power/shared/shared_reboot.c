#include "shared_reboot.h"

void fast_reboot()
{
    u8int good = 0x02;
    while (good & 0x02)
        good = inb(0x64);
    outw(0x64, 0xFE);
    for (;;);
}