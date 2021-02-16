#ifndef VIRTUAL_8086_H
#define VIRTUAL_8086_H

#include "../common.h"

// Defined in v8086_.s
extern void enter_v8086(u32int SS, u32int ESP, u32int CS, u32int EIP);

#endif //VIRTUAL_8086_H