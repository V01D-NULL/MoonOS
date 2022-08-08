#include "spinlock.h"

void acquire_lock(Lock *lock)
{
	while (__atomic_test_and_set(lock, __ATOMIC_ACQUIRE))

// I'm not sure if there is an arm equivalent of the x86 pause instruction
#if defined(__x86_64__)
		asm volatile("pause");
#else
		;
#endif
}

void release_lock(Lock *lock)
{
	__atomic_clear(lock, __ATOMIC_RELEASE);
}
