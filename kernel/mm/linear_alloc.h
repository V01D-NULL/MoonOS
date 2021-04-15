/**
 * @file linear_alloc.h
 * @author Tim (V01D)
 * @brief Physical linear memory allocator
 * @version 0.1
 * @date 2021-04-16
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef MM_LINEAR_H
#define MM_LINEAR_H

#include <stdint.h>
#include <libk/kassert.h>
#include <libk/kstdlib.h>
#include "../drivers/io/serial.h"
#include "../boot/bootloader_stivale2.h"

/*
    Q&D linear memory allocator | Will be replaced by a more advanced memory managment algorithm eventually
*/

/**
 * @brief Holds information about the allocation status
 * 
 */
typedef struct linear_tracker {
    void *start;
    uint64_t size;
    uint8_t *first_free_addr;
    void *end;
    uint8_t *allocation_ptr;
}__attribute__((packed)) linear_tracker_t;

//
/**
 * @brief Setup a linear memory buffer
 * 
 * @param start  Start address of the linear memory buffer
 * @param size   Size of the linear memory buffer
 * @return int  Returns EXIT_FAILURE (1) on failure, and EXIT_SUCCESS (0) on success
 */
int linear_mm_init(void *start, uint64_t size);

/**
 * @brief Allocate a block of the linear buffer's memory
 * 
 * @param size Size of the memory block to allocate (in bytes)
 * @param byte_align_ammount  Align the allocated block of memory by 'byte_align_amount' (This is usually 16)
 * @return uint8_t* Returns EXIT_FAILURE (1) on failure, and a pointer to the allocated block on success
 */
uint8_t *linear_alloc(uint64_t size, int byte_align_ammount);

/**
 * @brief Release all memory in the linear buffer.
 * 
 */
void linear_mm_release_buffer();

#endif // MM_LINEAR_H