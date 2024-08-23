#ifndef CONTAINERS_H
#define CONTAINERS_H

/**
 * This header file only exists to expose cc.h after defining the necessary
 * macros. This way CC_REALLOC and CC_FREE do not have to be manually defined in
 * every file using cc.h.
 * 
 * cc.h should and can not be directly be included in other files unless the ALLOW_IMPORT_FROM_CC_H macro is defined.
 * Always include this file instead.
 */

#include <mm/alloc.h>

#define ALLOW_IMPORT_FROM_CC_H
#define CC_REALLOC realloc
#define CC_FREE free
#include "private/cc.h"
#undef ALLOW_IMPORT_FROM_CC_H

#endif  // CONTAINERS_H