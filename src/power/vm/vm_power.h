#ifndef VM_POWER_H
#define VM_POWER_H

#include "../../common.h"
#include "../../arch/x86/bios32/bios_calls.h"
#include "../../panic.h"

enum VM_TYPE
{
    QEMU = 0,
    BOCHS,
    VBOX
};

void vm_shutdown(int which);

#endif // VM_POWER_H