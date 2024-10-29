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

#define UNWRAP(fn)                                              \
    ({                                                          \
        auto result = fn;                                       \
        if (!result.is_ok)                                      \
        {                                                       \
            panic("Unwrapping error result! (%s)", result.err); \
        }                                                       \
        result.ok;                                              \
    })

#define TRY_UNWRAP(fn, T)                \
    ({                                   \
        auto result = fn;                \
        if (!result.is_ok)               \
        {                                \
            return Error(T, result.err); \
        }                                \
        result.ok;                       \
    })

#endif  // RESULT_H