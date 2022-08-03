#ifndef RESULT_H
#define RESULT_H

#include <panic.h>
#include <base/base-types.h>
#include <amd64/moon.h>

packed_struct$(Result, {
	bool success;
	uint64_t status;
});

inline uint64_t UNWRAP_RESULT(Result r)
{
	if (r.success)
		return r.status;

	panic("Failed to unwrap result");
}

#define Error() \
	(Result) { 0 }

#define Okay(x) \
	(Result) { .success = true, .status = x }

#endif // RESULT_H