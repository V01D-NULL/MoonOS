#ifndef SDT_H
#define SDT_H

#include <stdint.h>

struct sdt
{
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_string[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creatorID;
    uint32_t creator_revision;
};

#endif // SDT_H