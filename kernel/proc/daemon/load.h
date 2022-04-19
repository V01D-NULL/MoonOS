#ifndef DAEMON_LOADER_H
#define DAEMON_LOADER_H
#include <stdint.h>

void load_daemon(const uint8_t* elf, const char *name);

#endif // DAEMON_LOADER_H