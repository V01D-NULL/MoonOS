/**
 * @file vga.cursor.h
 * @author Tim (V01D)
 * @brief Manage VGA text mode cursor
 * @version 0.1
 * @date 2021-04-16
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef VGA_CURSOR_H
#define VGA_CURSOR_H

#include <stdint.h>

/**
 * @brief Move the VGA cursor
 * 
 * The (x, y) coordinates are updated automatically by the kernel
 */
void vga_move_cursor();

/**
 * @brief Destroy / hide / disable the VGA cursor
 * 
 */
void vga_kill_cursor();

/**
 * @brief Modify the cursors appearance
 * 
 * @param cursor_start
 * @param cursor_end 
 */
void vga_enable_cursor(uint8_t cursor_start, uint8_t cursor_end);

#endif