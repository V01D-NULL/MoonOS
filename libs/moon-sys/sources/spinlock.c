#include "spinlock.h"

void acquire_lock(Lock *lock)
{
	while (__atomic_test_and_set(lock, __ATOMIC_ACQUIRE))
		asm volatile("pause");
}

void release_lock(Lock *lock)
{
	__atomic_clear(lock, __ATOMIC_RELEASE);
}
