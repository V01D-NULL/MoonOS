# Useful links
* https://rwth-os.github.io/eduOS/
* https://github.com/kAworu/JamesM
* https://github.com/cirosantilli/jamesmolloy-kernel-development-tutorials
* https://github.com/berkus/jamesm-tutorials/
* https://github.com/Usemox/Usemox/

# QEMU

No display
* exit: `ctrl-a + x`

Curses as display
* exit:
	* `alt + 2` to switch to qemu command line
	* Then type `q` and press `enter`


# GDB

Debugging
* `break`*
	* set breakpoints. Pause execution when reach a point in the code.
	* examples:
		* `break fxName`
		* `break *(0xFFFF)`
		* `break *(fxName+offset)`
		* `break fileName:lineNo`
* `watch`
	* set watchpoints. Pause execution when a value changes.
	* examples:
		* `watch varName`
		* ...
* `info`
	* list breakpoints: `info break`
	* list watchpoints: `info watch`
* `delete #`
	* delete a break or watchpoint
* `continue`
	* resume program
* `ctrl-c`
	* interrupt running program
* `step`
	* execute next line of code
* `stepi`
	* execute next line of assembly code
* `next`
	* like `step` but steps over function calls (instead of entering them)
* `nexti`
	* ditto
* `print`
	* print variables
	* examples:
		* `p varName`
		* `p *varName` (print pointee)
		* ...
* `x`
	* print memory
	* examples:
		* ...
* `backtrace`
	* show call stack
* `up`, `down`
	* navigate call stack
* `set disassembly-flavor _choice_`
	* choose disassembly syntax: `intel` or `att` (default)
* `quit`

Floating point
* Examine FPU stack

	* `p $st0`
	* `p $st1`
	* `p { $st0, $st1, $st2, $st3, $st4, $st5, $st6, $st7 }`
	* `info float`
* Examine float variable
	* `p {double} &varName`
		* double precision (DQ)
	* `p {float} &varName`
		* single precision (DW)

TUI
* enter or leave TUI mode: `ctrl-x + a`
* change focused window: `ctrl-x + o`
* show three windows: `ctrl-x + 2`
* change window content: `ctrl-x + 2` subsequent times
* previous command: `ctrl-p`

Trace commands
* Commands and outputs are written to gdb.txt
	* `set trace-commands on`
	* `set logging on`
* On another console/terminal, type `tail -f gdb.txt` to view the file in realtime

More
* Official manual: [old][1], [new][2]


# Objdump

Disassembly
* `-D`
	* disassemble all sections
* `-d`
	* disassemble .text
* `-M intel`
	* use intel syntax in disassembly
* `-D -b binary -m i386`
	* disassemble unknown binary using i386 ISA

Hex dump
* `-s`
	* hex dump all
* `-s -j .rodata`
	* hex dump .rodata


# Readelf

* `-a`
	* all


# Hexdump

* `-C`
	* hex+ASCII (more useful than default)
* `-s`
	* offset
* `-n`
	* number of bytes


# Compiling

Compiling C code
* Vanilla
	* `gcc inputFile.c -o outputFile.o`
* `-g`
	* generate debugging information
* `-mfpmath=387`
	* force x87 instead of SSE floating point

Compiling assembly code

* gcc (at&t syntax)
	* Vanilla
		* `as inputFile.asm -o outputFile.o`
	* With debug symbols
		* `as -g inputFile.asm -o outputFile.o`
	* Skip need for linker
		* `gcc -g -m64 -nostdlib inputFile.s -o outputFile`

* nasm (nasm/intel syntax)
	* Vanilla
		* `nasm -f elf64 inputFile.asm -o outputFile.o`
	* With debug symbols
		* `nasm -f elf64 -F dwarf -g inputFile.asm -o outputFile.o`

* linker (make executable)
	* `ld -m elf_x86_64 -o outputFile outputFile.o`











[1]: https://ftp.gnu.org/old-gnu/Manuals/gdb/html_node/gdb_toc.html
[2]: https://sourceware.org/gdb/onlinedocs/gdb/index.html
