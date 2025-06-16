#include "tar.h"
#include <abi/syscalls.h>
#include <string.h>

unsigned int getsize(const char *in)
{
    unsigned int size = 0;
    unsigned int j;
    unsigned int count = 1;

    for (j = 11; j > 0; j--, count *= 8)
        size += ((in[j - 1] - '0') * count);

    return size;
}

int tar_parse_headers(uint64_t address, struct TarHeader **headers)
{
    int i;

    for (i = 0;; i++)
    {
        struct TarHeader *header = (struct TarHeader *)address;

        if (header->filename[i] == '\0')
            break;

        unsigned int size = getsize(header->size);

        headers[i] = header;

        address += ((size / 512) + 1) * 512;

        if (size % 512)
            address += 512;
    }

    return i;
}
