#ifndef CMDLINE_PARSER_H
#define CMDLINE_PARSER_H

#include <stdbool.h>
bool boot_cmdline_find_tag(const char *tag, const char *cmdline);

#endif // CMDLINE_PARSER_H