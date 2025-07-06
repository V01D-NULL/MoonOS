#include "../public/string.h"

// TODO: Implement me in a different PR
string make_string(const char *from)
{
}

// TODO: Implement me in a different PR
string_view make_string_view(const char *from)
{
}

int strlen(const char *s)
{
    int len = 0;

    while (*s++ != '\0')
    {
        len++;
    }

    return len;
}

uint64_t strtoul(const char *str)
{
    unsigned long result = 0;
    int           base   = 10;
    const char   *ptr    = str;

    if (ptr[0] == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
    {
        base = 16;
        ptr += 2;  // Move past the "0x" prefix
    }

    while (*ptr)
    {
        char          c = *ptr;
        unsigned long value;

        if (c >= '0' && c <= '9')
        {
            value = c - '0';
        }
        else if (c >= 'a' && c <= 'f')
        {
            value = c - 'a' + 10;
        }
        else if (c >= 'A' && c <= 'F')
        {
            value = c - 'A' + 10;
        }
        else
        {
            break;
        }

        if (value >= base)
        {
            break;
        }

        result = result * base + value;
        ptr++;
    }

    return result;
}