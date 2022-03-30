#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <amd64/moon.h>

enum
{
    PG_PRESENT = (1 << 0), // Present
    PG_RW = (1 << 1),      // Read-write
    PG_SUPER = (1 << 2)    // Supervisor
};

enum vmm_mapping_protection
{
    MAP_READONLY = PG_PRESENT,                   // Present/readonly, kernel only
    MAP_KERN = PG_PRESENT | PG_RW,               // Read-write kernel only
    MAP_USER_RW = PG_PRESENT | PG_RW | PG_SUPER, // User accessible page (ReadWrite)
    MAP_USER_RO = PG_PRESENT | PG_SUPER          // User accesible page (ReadOnly)
};

#endif // PAGING_H