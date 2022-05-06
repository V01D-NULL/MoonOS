#ifndef BUDDY_UTILS
#define BUDDY_UTILS

#include <amd64/moon.h>

/*
	https://www.wfbsoftware.de/a-buddy-system-memory-allocator-explained/
	https://www.kernel.org/doc/gorman/html/understand/understand009.html
*/

STATIC_INLINE int get_left_child_node(int index)
{
	return (index * 2) + 1;
}

STATIC_INLINE int get_right_child_node(int index)
{
	return (index * 2) + 2;
}

// Get the parent node of the node at 'index'
STATIC_INLINE int get_parent_of(int index)
{
	return (index - 1) / 2;
}

// Get the sibling node of the node at 'index'
STATIC_INLINE int get_sibling_node(int index)
{
	return ((index - 1) ^ 1) + 1;
}

#endif // BUDDY_UTILS