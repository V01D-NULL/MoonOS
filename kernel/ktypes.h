#ifndef CTYPES_H
#define CTYPES_H

#include <stddef.h>
#include <stdint.h>

#define bool _Bool
#define true 1
#define false 0

#define auto __auto_type

typedef char *string;
typedef const char *string_view;

#define $struct(tdef, ...) typedef struct __##tdef __VA_ARGS__ tdef
#define $packed_struct(tdef, ...) typedef struct __##tdef __VA_ARGS__ PACKED tdef

// Forward declaration for $*_struct types
#define $fwd_decl_struct(typename) typedef struct __##typename typename

#endif // CTYPES_H