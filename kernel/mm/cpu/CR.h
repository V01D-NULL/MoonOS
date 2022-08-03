#ifndef CONTROL_REGISTER_X
#define CONTROL_REGISTER_X
#include <base/base-types.h>

/* Defines the cr number: cr(number) */
enum CR_NUMBER
{
    CR0 = 0x0,
    CR1,
    CR2,
    CR3,
    CR4
};

int64_t cr_read(enum CR_NUMBER cr);

#endif // CONTROL_REGISTER_X