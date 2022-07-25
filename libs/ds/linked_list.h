#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdint.h>
#include <amd64/moon.h>

#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
#pragma GCC diagnostic push

struct dlist
{
    struct dlist *prev, *next;
};

struct slist
{
    struct slist *next;
};

// Taken from linux
#define container_of(ptr, type, member) ({                         \
    const typeof(((type *)0)->member) *__mptr = (ptr);             \
    type *__t = (type *)((char *)__mptr - offsetof(type, member)); \
    __mptr == NULL ? NULL : __t;                                   \
})

// Retrieve the next element in the list
#define list_next(input, member) ({                                  \
    typeof(input) *_next_ = (input)->member.next;                    \
    !_next_ ? NULL : container_of(_next_, typeof(*(input)), member); \
})

// Iterate over a singly or doubly linked list.
// Parameters:
// [] out:    The input->next data will be written to 'out'.
// [] member: Name of the list member in the struct 'input'.
// [] input:  An instance of a struct.
#define list_foreach(out, member, input) \
    for (typeof(input) out = input; out != NULL; out = list_next(out, member))

#define list_append(node, member, input) ({           \
    __list_append(&(input)->member, &(node)->member); \
})

STATIC_INLINE void __list_append(struct slist *head, struct slist *node)
{
    struct slist *prev = head;
    while (head)
        prev = head, head = head->next;

    prev->next = node;
}

#define list_set_next(node, member, input) ({                              \
    typeof(node) node_to_append = node;                                    \
    struct slist *head = &(input)->member;                                 \
    struct slist *next = !node_to_append ? NULL : &node_to_append->member; \
    head->next = next;                                                     \
})

#pragma GCC diagnostic pop

#endif // LINKED_LIST_H