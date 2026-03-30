#ifndef LIB_KERNEL_STRING_H
#define LIB_KERNEL_STRING_H

#include <base/base-types.h>
#include <base/cstring.h>
#include <base/mem.h>

#define str(expression)                       \
    _Generic((expression),                    \
        char *: make_string_view,             \
        const char *: make_string_view,       \
        String: make_string_view_from_string, \
        default: make_string_view_from_array)(expression)

#define str_literal(literal)                           \
    (StringView)                                       \
    {                                                  \
        .length = sizeof(literal) - 1, .data = literal \
    }

typedef struct
{
    size_t length;
    char  *data;
} String;

typedef struct
{
    const size_t      length;
    const char *const data;
} StringView;

StringView make_string_view(const char *str);
StringView make_string_view_from_string(String str);
StringView make_string_view_from_array(const char arr[], size_t len);

#endif  // LIB_KERNEL_STRING_H