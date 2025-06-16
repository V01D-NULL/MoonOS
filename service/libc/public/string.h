#ifndef STRING_H
#define STRING_h

#include <stdint.h>

typedef struct
{
    char *value;
    int   len;
} string;

typedef struct
{
    const char *string_view;
    int         len;
} string_view;

string      make_string(const char *from);
string_view make_string_view(const char *from);

int      strlen(const char *s);
uint64_t strtoul(const char *str);

#endif  // STRING_H