#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <ktypes.h>

$struct(SymbolTable, {
	uint64_t addr;
	string_view name;
});

void backtrace_symbol(uint64_t address);
int64_t find_symbol_by_name(string name);
SymbolTable sym_lookup(uint64_t address);

#endif // SYMBOLS_H