#ifndef CMDLINE_PARSER_H
#define CMDLINE_PARSER_H

#include <ktypes.h>

bool boot_cmdline_find_tag(string_view tag, string_view cmdline);

#endif // CMDLINE_PARSER_H