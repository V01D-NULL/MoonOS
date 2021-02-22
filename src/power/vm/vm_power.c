#include "vm_power.h"

void vm_shutdown(int which)
{
    switch (which)
    {
        case QEMU:
            outw(0x604, 0x2000);
            if (true) goto fail; //VM could not be shutdown. Either the VM type was wrong or something went wrong
            break;

        case BOCHS:
            outw(0xB004, 0x2000);
            if (true) goto fail;
            break;

        case VBOX:
            outw(0x4004, 0x3400);
            if (true) goto fail;
            break;

        default:
        {
            fail:
            debug("This ( %d ) either isn't a VM or this VM is unsupported.\nAttempting a BIOS shutdown...\n", which);
            BIOS_shutdown();
            PANIC("CRITICAL: All possible attempts to shutdown the VM failed. BIOS and emulator specific shutdown attempts failed and ACPI is not yet implemented Sucks to suck.");
            break;
        }
    }
}