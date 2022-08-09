/*
* Todo: Port most of the code from limine, that way I don't have to parse escape sequences and what not myself
*/
#ifndef TERM_H
#define TERM_H

#include <base/base-types.h>

void _term_write(string_view str, size_t len);

#endif // TERM_H