#ifndef CREATE_ES_H
#define CREATE_ES_H

#include <moon-extra/result.h>
#include "es.h"

typedef Result(ExecutionSpace, Nullish) EsCreateResult;

EsCreateResult create_execution_space(const uint8_t *elf_pointer, int pid);

#endif  // CREATE_ES_H