#ifndef RSDT_H
#define RSDT_H

#include "sdt.h"

struct rsdt_header
{
    struct sdt rsdt;
    char children[];
} __pack_bytes;

#endif // RSDT_H