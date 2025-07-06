#include "tar.h"
#include <abi/syscalls.h>
#include <string.h>

uint32_t tar_get_content_size(const char *in)
{
    uint32_t size = 0;
    uint32_t j;
    uint32_t count = 1;

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

        uint32_t size = tar_get_content_size(header->size);
        headers[i]    = header;

        address += ((size / 512) + 1) * 512;

        if (size % 512)
            address += 512;
    }

    return i;
}
