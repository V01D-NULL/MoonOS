#ifndef TAR_H
#define TAR_H

#include <stdint.h>

#define TAR_BLOCK_SIZE 512

struct TarHeader
{
    char filename[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag[1];
};

int tar_parse_headers(uint64_t address, struct TarHeader **headers);

#endif  // TAR_H