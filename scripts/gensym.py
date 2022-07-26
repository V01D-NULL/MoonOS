from os import popen, system
import sys

PARSED =  "parsed.sym"
KERNEL =  sys.argv[1]
writer = open(PARSED, "w+")

symbol_table_naked = """
#include <sym.h>    // kernel/trace/sym.h
sym_table_t symbol_table[] = {
"""

def init_writer():
    writer.seek(0)
    writer.truncate()
    writer.write(symbol_table_naked)

# return an entry in the symbol array: {0xdeadbeef, "function"},
def symbol_template(address, function_name):
    return '\t{0x' + address + ', ' + '\"' + function_name + '\"' + '},\n'

def write_data(address, function_name):
    if function_name == "":
        return
    writer.write(symbol_template(address=address, function_name=function_name))


def destroy_writer():
    writer.write("{0xFFFFFFFF, \"\"}")
    writer.write("};")
    writer.close()

# Input: A single line from objdump, returns true if it's a function, otherwise it returns false since we don't need variables or function names
def is_function_symbol(objdump_line):
    if not ".text" in objdump_line:
        return False
    return True

def function_symbol_extract(objdump_line):
    objdump_line = str(objdump_line).split()
    return (str(objdump_line[0]), str(objdump_line[5:]).replace("]", "").replace("'","").replace("[", "").replace(",,", ","))

def parse_symbol_tables(unparsed_sym_table):
    unparsed_sym_table = unparsed_sym_table.splitlines()

    for x in range(len(unparsed_sym_table)-1):
        if not is_function_symbol(unparsed_sym_table[x]):
            continue
        else:
            addr, name = function_symbol_extract(unparsed_sym_table[x])
            write_data(str(addr), name)
        

def compile_sym_table():
    system(f"gcc -ffreestanding -x c -c {PARSED} -m64 -I trace/ -I . -o trace/symtable.o")
    print("[*] Compiled symbol table")

if __name__ == '__main__':
    init_writer()
    parse_symbol_tables(popen(f"objdump -C -t {KERNEL}").read())
    destroy_writer()
    print("[*] Wrote symbol table")

    compile_sym_table()