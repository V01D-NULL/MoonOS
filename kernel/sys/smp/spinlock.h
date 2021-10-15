#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdbool.h>

#define create_lock(name, lock) static lock_t lock = {0, name};

typedef struct lock {
    int locked;
    char *name;
} lock_t;

void acquire_lock(lock_t *lock);
void release_lock(lock_t *lock);

#endif // SPINLOCK_H