#ifndef DAEMON_LOADER_H
#define DAEMON_LOADER_H

#include <base/base-types.h>

void load_daemon(const uint8_t* elf, string_view name);

#endif // DAEMON_LOADER_H