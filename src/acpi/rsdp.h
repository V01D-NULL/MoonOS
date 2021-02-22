#ifndef ROOT_SYSTEM_DESCRIPTION_POINTER_H
#define ROOT_SYSTEM_DESCRIPTION_POINTER_H

#include "../common.h"

//ACPI Version 1.x specific (since this is a 32 bit os Validity must use ACPI 1.x)
struct rsdp_desc
{
    char    sig[8];
    u8int   checksum;
    char    OEMID[6];
    u8int   revision;
    u32int  rsdp_addr;
}__attribute__((packed));



#endif // ROOT_SYSTEM_DESCRIPTION_POINTER_H