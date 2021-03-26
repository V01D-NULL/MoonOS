#ifndef MM_RAM_H
#define MM_RAM_H

#include <stdint.h>
#include "../boot/bootloader_stivale2.h"

//size_t go brr
typedef struct ram {
    size_t used;
    size_t total;
    size_t free;
} ram_usage_t;

//Store inital RAM information from the bootloader
void ram_manager_init(boot_info_t ram_info);

//Get the free, total, or used ram in kb
size_t ram_manager_get_free();
size_t ram_manager_get_total();
size_t ram_manager_get_used();

//Marks *ammount* number of kb as free and returns the used ram
size_t ram_manager_free(uint64_t ammount);

//Marks *ammount* number of kb as used and returns the used ram
size_t ram_manager_add(uint64_t ammount);

#endif // MM_RAM_H