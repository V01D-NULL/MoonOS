#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdint.h>

/* Default linked list */
struct ds_default_list
{
    union
    {
        struct ds_default_list *next;
        uint64_t value;
    };
};

void ds_default_next(struct ds_default_list **restrict __list);

#endif // LINKED_LIST_H