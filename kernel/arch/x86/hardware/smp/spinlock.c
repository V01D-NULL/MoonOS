#include "spinlock.h"
#include <moon.h>
#include <base/base-types.h>

void acquire_lock(lock_t *lock)
{
    while(__atomic_test_and_set(lock, __ATOMIC_ACQUIRE));
}

void release_lock(lock_t *lock)
{
    __atomic_clear(lock, __ATOMIC_RELEASE);
}