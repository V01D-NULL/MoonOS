#ifndef CREATE_EC_H
#define CREATE_EC_H

#include <base/base-types.h>
#include "ec.h"

ExecutionContext create_ec(uint64_t entry, uint64_t sp);

#endif  // CREATE_EC_H