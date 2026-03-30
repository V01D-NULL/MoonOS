#ifndef LIB_KERNEL_STRING_H
#define LIB_KERNEL_STRING_H

#include <base/base-types.h>
#include <base/cstring.h>
#include <base/mem.h>

#define str(expression)                                                 \
    _Generic((expression),                                              \
        char *: make_string_view(expression, strlen(expression)),       \
        const char *: make_string_view(expression, strlen(expression)), \
        default: make_string_view(expression, sizeof(expression) - 1))

typedef struct
{
    size_t length;
    char  *data;
} String;

typedef struct
{
    const size_t length;
    const char  *data;
} StringView;

String     make_string(const char *str);
StringView make_string_view(const char *str, size_t len);

#endif  // LIB_KERNEL_STRING_H