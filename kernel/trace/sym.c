#include "sym.h"
#include <util/ptr.h>
#include <printk.h>
#include <stddef.h>
#include <libk/kstring.h>
#include <devices/serial/serial.h>

extern sym_table_t symbol_table[];

sym_table_t sym_lookup(uint64_t address)
{
    uint64_t corrected_address = 0;
    uint64_t index_new = 0;

    for (int i = 0;; i++)
    {
        uint64_t sym_addr = symbol_table[i].addr;

        if (corrected_address >= address)
            continue;

        else if (sym_addr < address)
        {
            if (sym_addr > corrected_address)
            {
                corrected_address = sym_addr;
                index_new = i;
            }
        }

        if (sym_addr == 0xFFFFFFFF)
        {
            return (sym_table_t){
                .addr = corrected_address,
                .name = symbol_table[index_new].name};
        }
    }
}

int64_t find_symbol_by_name(char *name)
{
    for (size_t i = 0;; i++)
    {
        char *symname = GENERIC_CAST(char *, symbol_table[i].name);

        if (strcmp(name, symname) == 0)
        {
            return symbol_table[i].addr;
        }

        if (symbol_table[i].addr == 0xFFFFFFFF)
        {
            return -1;
        }
    }
}

void backtrace_symbol(uint64_t address)
{
    sym_table_t sym = sym_lookup(address);
    printk("backtrace", "\033[0;37m%llx - %s\n", sym.addr, sym.name);
    debug(false, "%llx - %s\n", sym.addr, sym.name);
}