#ifndef LIB_KERNEL_STDLIB_H
#define LIB_KERNEL_STDLIB_H

#include <stdint.h>

//Ascii to int
uint64_t atoi(const char* string);

//Int to binary
char *itob(uint64_t n);

//Int to hex
char *itoh(uint64_t n);

//Int to ascii - The base must be specified
char *itoa(int n, char *str, int base);

#endif // LIB_KERNEL_STDLIB_H