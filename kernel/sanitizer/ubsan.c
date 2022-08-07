/*
    BSD 2-Clause License

    Copyright (c) 2021, Abb1x
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdint.h>
#include <moon-io/serial.h>
#include <base/base-types.h>

struct tu_source_location
{
    string_view file;
    uint32_t line;
    uint32_t column;
};

struct tu_type_descriptor
{
    uint16_t kind;
    uint16_t info;
    char name[];
};

struct tu_overflow_data
{
    struct tu_source_location location;
    struct tu_type_descriptor *type;
};

struct tu_shift_out_of_bounds_data
{
    struct tu_source_location location;
    struct tu_type_descriptor *left_type;
    struct tu_type_descriptor *right_type;
};

struct tu_invalid_value_data
{
    struct tu_source_location location;
    struct tu_type_descriptor *type;
};

struct tu_array_out_of_bounds_data
{
    struct tu_source_location location;
    struct tu_type_descriptor *array_type;
    struct tu_type_descriptor *index_type;
};

struct tu_type_mismatch_v1_data
{
    struct tu_source_location location;
    struct tu_type_descriptor *type;
    unsigned char log_alignment;
    unsigned char type_check_kind;
};

struct tu_negative_vla_data
{
    struct tu_source_location location;
    struct tu_type_descriptor *type;
};

struct tu_nonnull_return_data
{
    struct tu_source_location location;
};

struct tu_nonnull_arg_data
{
    struct tu_source_location location;
};

struct tu_unreachable_data
{
    struct tu_source_location location;
};

struct tu_invalid_builtin_data
{
    struct tu_source_location location;
    unsigned char kind;
};

static void tu_print_location(string message, struct tu_source_location loc)
{
    debug(0, "tinyubsan: %s at file %s, line %d, column %d\n", message, loc.file, loc.line, loc.column);
}

void __ubsan_handle_add_overflow(struct tu_overflow_data *data)
{
    tu_print_location("addition overflow", data->location);
}

void __ubsan_handle_sub_overflow(struct tu_overflow_data *data)
{
    tu_print_location("subtraction overflow", data->location);
}

void __ubsan_handle_mul_overflow(struct tu_overflow_data *data)
{
    tu_print_location("multiplication overflow", data->location);
}

void __ubsan_handle_divrem_overflow(struct tu_overflow_data *data)
{
    tu_print_location("division overflow", data->location);
}

void __ubsan_handle_negate_overflow(struct tu_overflow_data *data)
{
    tu_print_location("negation overflow", data->location);
}

void __ubsan_handle_pointer_overflow(struct tu_overflow_data *data)
{
    tu_print_location("pointer overflow", data->location);
}

void __ubsan_handle_shift_out_of_bounds(struct tu_shift_out_of_bounds_data *data)
{
    tu_print_location("shift out of bounds", data->location);
}

void __ubsan_handle_load_invalid_value(struct tu_invalid_value_data *data)
{
    tu_print_location("invalid load value", data->location);
}

void __ubsan_handle_out_of_bounds(struct tu_array_out_of_bounds_data *data)
{
    tu_print_location("array out of bounds", data->location);
}

void __ubsan_handle_type_mismatch_v1(struct tu_type_mismatch_v1_data *data, uintptr_t ptr)
{
    if (!ptr)
    {
        tu_print_location("use of NULL pointer", data->location);
    }

    else if (ptr & ((1 << data->log_alignment) - 1))
    {
        tu_print_location("use of misaligned pointer", data->location);
    }
    else
    {
        tu_print_location("no space for object", data->location);
    }
}

void __ubsan_handle_vla_bound_not_positive(struct tu_negative_vla_data *data)
{
    tu_print_location("variable-length argument is negative", data->location);
}

void __ubsan_handle_nonnull_return(struct tu_nonnull_return_data *data)
{
    tu_print_location("non-null return is null", data->location);
}

void __ubsan_handle_nonnull_arg(struct tu_nonnull_arg_data *data)
{
    tu_print_location("non-null argument is null", data->location);
}

void __ubsan_handle_builtin_unreachable(struct tu_unreachable_data *data)
{

    tu_print_location("unreachable code reached", data->location);
}

void __ubsan_handle_invalid_builtin(struct tu_invalid_builtin_data *data)
{

    tu_print_location("invalid builtin", data->location);
}