#ifndef LIB_KERNEL_STDLIB_H
#define LIB_KERNEL_STDLIB_H

#include <stdint.h>

//Ascii to int
uint64_t atoi(const char* string);

//Int to binary
char *itob(uint64_t n);

//Int to hex
char *itoh(uint64_t n);

char *itoa(uint64_t n);

//Int To X (where X depends on the value of `base`, 2, 8, 10, 16 - b, o, d/i, h)
// Note that itoX is specific to the Validity kernel, the function is not posix or unix compliant
char *itoX(uint64_t num, uint64_t base);

#endif // LIB_KERNEL_STDLIB_H