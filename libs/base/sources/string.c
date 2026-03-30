#include "base/string.h"
#include "base/assert.h"

String make_string(const char *str)
{
#if defined(HEAVY_ASSERTS)
    assert(str != NULL && "Bad string pointer");
#else
    if (str == NULL)
        return (String){0};
#endif

    size_t len = strlen(str);
    return (String){
        .length = len,
        .data   = str,
    };
}

StringView make_string_view(const char *str, size_t len)
{
#if defined(HEAVY_ASSERTS)
    assert(str != NULL && "Bad string pointer");
    assert(len > 0 && "Bad length");
    assert(strlen(str) <= len &&
           "Buffer overflow: cstring length is greater than provided length");
#else
    if (str == NULL)
        return (StringView){0};
#endif

    return (StringView){
        .length = len,
        .data   = str,
    };
}
