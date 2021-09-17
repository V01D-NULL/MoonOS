#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <stdint.h>

typedef struct
{
    uint64_t addr;
    const char *name;
} sym_table_t;

void backtrace_symbol(uint64_t address);
uint64_t find_symbol_by_name(char *name);

#endif // SYMBOLS_H