#ifndef SLEEP_H
#define SLEEP_H

#include <base/base-types.h>

void sleep_init(void);
void usleep(uint64_t usec);

#endif // SLEEP_H