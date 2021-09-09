#ifndef KERNEL_STACK_TRACE_H
#define KERNEL_STACK_TRACE_H

#include "../amd64/validity.h"

void strace();
void walk_frames(int frame_count);

#endif // KERNEL_STACK_TRACE_H