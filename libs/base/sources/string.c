#include "base/string.h"
#include "base/assert.h"

StringView make_string_view(const char *str)
{
#if defined(HEAVY_ASSERTS)
    assert(str != NULL && "Bad string pointer");
    assert(strlen(str) > 0 && "Bad length");
#else
    if (str == NULL)
        return (StringView){0};
#endif

    return (StringView){
        .length = strlen(str),
        .data   = str,
    };
}

StringView make_string_view_from_string(const String *str)
{
    return (StringView){
        .length = str->length,
        .data   = str->data,
    };
}

StringView make_string_view_from_array(const char arr[], size_t len)
{
#if defined(HEAVY_ASSERTS)
    assert(arr != NULL && "Bad string pointer");
    assert(len > 0 && "Bad length");
#else
    if (arr == NULL)
        return (StringView){0};
#endif

    return (StringView){
        .length = len,
        .data   = arr,
    };
}