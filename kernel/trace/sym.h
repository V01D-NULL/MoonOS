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

#else

// Stubs
inline void backtrace_symbol(uint64_t address) {}
inline SymbolTable sym_lookup(uint64_t address) { return (SymbolTable){.addr = 0, .name = ""}; }

#endif // __x86_64__
#endif // SYMBOLS_H