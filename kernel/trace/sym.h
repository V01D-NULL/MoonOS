#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <base/base-types.h>

struct$(SymbolTable, {
	uint64_t addr;
	string_view name;
});

#if defined(__x86_64__)
void backtrace_symbol(uint64_t address);
int64_t find_symbol_by_name(string name);
SymbolTable sym_lookup(uint64_t address);
#endif // __x86_64__

inline void backtrace_symbol(uint64_t address) {}
inline SymbolTable sym_lookup(uint64_t address) {}

#endif // SYMBOLS_H