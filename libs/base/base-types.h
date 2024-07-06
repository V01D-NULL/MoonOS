#ifndef BASE_TYPES_H
#define BASE_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define auto __auto_type

typedef char       *string;
typedef const char *string_view;

typedef signed long ssize_t;

#define struct$(tdef, ...) typedef struct __##tdef __VA_ARGS__ tdef
#define packed_struct$(tdef, ...) \
    typedef struct __##tdef __VA_ARGS__ PACKED tdef

// Forward declaration for $*_struct types
#define forward_declare_struct$(typename) typedef struct __##typename typename

#endif  // BASE_TYPES_H