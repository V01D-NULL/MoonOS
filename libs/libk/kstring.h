/**
 * @file kstring.h
 * @author Tim (V01D)
 * @brief Kernel string function library
 * @version 0.1
 * @date 2021-04-16
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef LIB_KERNEL_STRING_H
#define LIB_KERNEL_STRING_H

#include <stdint.h>
#include <stddef.h>

/*
    TODO: Add strncmp
*/

/**
 * @brief Copy *n* bytes from src to dest
 * 
 * @param dest A pointer to the source bytes
 * @param src  A pointer to the destination bytes
 * @param len  The amount of bytes to copy
 */
void memcpy(uint8_t *dest, const uint8_t *src, uint64_t len);
void memcpy32(uint32_t *dest, const uint32_t *src, uint64_t len);

/**
 * @brief Set *n* bytes equal to val
 * 
 * @param dest A pointer to the destination bytes
 * @param val  The value to set the bytes to
 * @param len  The amount of bytes to modify
 */
void *memset (void *dest, const uint8_t val, size_t len);
// void memset(void *dest, uint8_t val, uint64_t len);

/**
 * @brief Compare two bytes
 * 
 * @param src   A pointer to the source bytes
 * @param dst   A pointer to the destination bytes
 * @param n     The number of bytes to compare
 * @return uint64_t 
 */
uint64_t memcmp(const void *src, void *dst, uint64_t n);

/**
 * @brief Move the bytes in dst to src
 * 
 * @param src  Source bytes
 * @param dst  Destination bytes
 * @param n    Amount of bytes to copy
 * @return void* 
 */
void *memmove(void *dst, const void *src, size_t n);

/**
 * @brief Compare two NULL-terminated strings
 * 
 * @param str1 First string
 * @param str2 Second string
 * @return int -1 if strings are not equal in length, 0 if string are the same, 1 if strings are not equal to eachother
 */
int strcmp(char *str1, char *str2);

/**
 * @brief Copy the NULL-terminated string 'src' into 'dest'
 * 
 * @param dest 
 * @param src 
 * @return char* The copied string 'dest'
 */
char* strcpy(char *dest, const char *src);

/**
 * @brief Concatenate two NULL-terminated strings
 * 
 * @param a First string
 * @param b Second string
 * @return char* The concatenated string
 */
char* strcat(char* a, char* b);

/**
 * @brief Count the length of a string
 * 
 * @param s String to count
 * @return uint64_t Length of the string
 */
uint64_t strlen(const char *s);

/**
 * @brief Reverse a string
 * 
 * @param src String to reverse
 * @return char* Reversed string
 */
char *strrev(char *src);

//Look for the first newline and return it's index
int parse_string_until_newline(char *str);

#endif // LIB_KERNEL_STRING_H