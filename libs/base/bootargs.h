#ifndef CMDLINE_PARSER_H
#define CMDLINE_PARSER_H

#include <base/base-types.h>

bool bootarg_find(string_view tag, string_view cmdline);

#endif // CMDLINE_PARSER_H