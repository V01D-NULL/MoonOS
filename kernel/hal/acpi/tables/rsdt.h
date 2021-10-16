#ifndef RSDT_H
#define RSDT_H

#include "sdt.h"

struct RSDT
{
    struct SDT header;
    uint32_t next[];
} __pack_bytes;

#endif // RSDT_H