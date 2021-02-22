#ifndef SHARED_REBOOT_H
#define SHARED_REBOOT_H
/*
    This file is in a shared/ folder, meaning both emulators and baremetal support this type of activity
*/

#include "../../common.h"

void fast_reboot();

#endif // SHARED_REBOOT_H