/**
 * @file ram.h
 * @author Tim (V01D)
 * @brief Track RAM usage
 * @version 0.1
 * @date 2021-04-16
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef MM_RAM_H
#define MM_RAM_H

#include <stdint.h>
#include "../boot/bootloader_stivale2.h"

/*
    All random access memory statistics (whether it be used, total, or free) will be returned in kb.
*/

typedef struct ram {
    uint64_t used;
    uint64_t total;
    uint64_t free;
} ram_usage_t;

/// Store inital RAM information from the bootloader
void ram_manager_init(boot_info_t *ram_info);

//Get the free, total, or used ram in kb
uint64_t ram_manager_get_free();
uint64_t ram_manager_get_total();
uint64_t ram_manager_get_used();
uint64_t ram_manager_get_total_in_bytes();

/// Marks *ammount* number of kb as free and returns the used ram
size_t ram_manager_free(uint64_t ammount);

/// Marks *ammount* number of kb as used and returns the used ram
size_t ram_manager_add(uint64_t ammount);

#endif // MM_RAM_H