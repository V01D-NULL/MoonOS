#ifndef RSDT_H
#define RSDT_H

#include "sdt.h"

struct RSDT
{
    struct SDT header;
    uint32_t next[];
} PACKED;

struct XSDT
{
    struct SDT header;
    uint64_t next[];
} PACKED;

#endif // RSDT_H