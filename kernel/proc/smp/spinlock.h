#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <ktypes.h>

#define create_lock(name, lock) static lock_t lock = {0, name};

// clang-format off
$struct(lock_t,
{
	int locked;
	string name;
});
// clang-format on

void acquire_lock(lock_t *lock);
void release_lock(lock_t *lock);

#endif // SPINLOCK_H