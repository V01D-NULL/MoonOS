#ifndef LIB_KERNEL_CSTRING_H
#define LIB_KERNEL_CSTRING_H

#include <base/base-types.h>

int      strcmp(const char *str1, const char *str2);
int      strncmp(string_view str1, string_view str2, size_t n);
char    *strcpy(char *dest, string_view src);
char    *strcat(char *a, char *b);
uint64_t strlen(string_view s);
char    *strrev(char *src);
bool     isdigit(int c);

#endif  // LIB_KERNEL_CSTRING_H