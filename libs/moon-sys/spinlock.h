#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <base/base-types.h>

#define create_lock(name, lock) static Lock lock = {0, name};

struct$(Lock, {
	int locked;
	string name;
});

void acquire_lock(Lock *lock);
void release_lock(Lock *lock);

#endif // SPINLOCK_H