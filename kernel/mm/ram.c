#include "ram.h"
#include <stdint.h>

ram_usage_t ram;

void ram_manager_init(boot_info_t *ram_info)
{
    ram.free = ram_info->mmap.free_ram;
    ram.total = ram_info->mmap.total_ram;
    ram.used = ram_info->mmap.used_ram;
}

//Add n kb of ram to the ram usage struct
size_t ram_manager_add(uint64_t ammount)
{
    ram.used += ammount;
    ram.free -= ammount;
    return ram.used;
}

//Remove n kb of ram from the ram usage struct
size_t ram_manager_free(uint64_t ammount)
{
    ram.used -= ammount;
    ram.free += ammount;
    return ram.used;
}

uint64_t ram_manager_get_free() {
    return ram.free;
}

uint64_t ram_manager_get_total() {
    return ram.total;
}

uint64_t ram_manager_get_used() {
    return ram.used;
}