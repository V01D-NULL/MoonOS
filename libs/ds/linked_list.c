#include "linked_list.h"
#include <stddef.h>

void ds_default_next(struct ds_default_list **restrict __list)
{
    (*__list) = (*__list)->next;
}
