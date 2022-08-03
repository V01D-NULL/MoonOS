#ifndef TRY_CATCH_H
#define TRY_CATCH_H

#include "compiler.h"
#include <base/base-types.h>

#define TRY_THROW_ENABLE               \
	void const *error_handler = &&err; \
	struct UNIQUE_NAME(__##__FUNC__)   \
	{                                  \
		string_view reason;            \
	} throw_reason;

// Enables TRY, THROW but defines a custom
// goto label instead of the default 'err'
#define TRY_THROW_ENABLE_ALTERNATIVE(x) \
	void const *error_handler = &&x;    \
	struct UNIQUE_NAME(__##__FUNC__)    \
	{                                   \
		string_view reason;             \
	} throw_reason;

#define TRY(x) \
	if (!(x))  \
	goto *error_handler

#define THROW(x)             \
	throw_reason.reason = "Issued THROW in: " __FILE__ " (" x ")"; \
	goto *error_handler;

// Added above a function (decl, definition) to emphasize that it uses TRY/THROW
#define CAN_THROW

#endif // TRY_CATCH_H