#ifndef RESULT_H
#define RESULT_H

#include <base/base-types.h>
#include <moon.h>
#include <panic.h>

typedef void *Nullish;

#define Result(Ok, Err) \
    struct              \
    {                   \
        bool is_ok;     \
        union           \
        {               \
            Ok  ok;     \
            Err err;    \
        };              \
    }

#define Error(T, V)              \
    (T)                          \
    {                            \
        .is_ok = false, .err = V \
    }

#define Okay(T, V)             \
    (T)                        \
    {                          \
        .is_ok = true, .ok = V \
    }

#endif  // RESULT_H