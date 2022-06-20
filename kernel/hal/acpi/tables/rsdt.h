#ifndef RSDT_H
#define RSDT_H

#include "sdt.h"

struct RSDT
{
    struct SDT header;
    uint32_t next[];
} gnu_pack_bytes;

struct XSDT
{
    struct SDT header;
    uint64_t next[];
} gnu_pack_bytes;

#endif // RSDT_H