// #ifndef ORDERED_ARRAY_H
// #define ORDERED_ARRAY_H


// #include "common.h"
// #include "panic.h"
// // This array is insertion sorted - it always remains in a sorted state (between calls).
// // It can store anything that can be cast to a void* -- so a u32int, or any pointer.
// typedef void* type_t;

// // A predicate should return nonzero if the first argument is less than the second.
// // Else it should return zero.
// typedef s8int ( *lessthan_predicate_t ) ( type_t, type_t );

// typedef struct
// {
// 	type_t               *array;
// 	u32int               size;
// 	u32int               max_size;
// 	lessthan_predicate_t less_than;
// }
// ordered_array_t;

// // A standard less than predicate
// s8int standard_lessthan_predicate ( type_t a, type_t b );

// // Create an ordered array (uses kmalloc)
// ordered_array_t orderedArray_create ( u32int max_size, lessthan_predicate_t less_than );

// // Create an ordered array (uses given start location)
// ordered_array_t orderedArray_place ( void *addr, u32int max_size, lessthan_predicate_t less_than );

// // Destroy an ordered array
// void orderedArray_destroy ( ordered_array_t *array );

// // Add an item into the array
// void orderedArray_insert ( type_t item, ordered_array_t *array );

// // Lookup the item at index i
// type_t orderedArray_lookup ( u32int i, ordered_array_t *array );

// // Deletes the item at location i from the array
// void orderedArray_remove ( u32int i, ordered_array_t *array );


// #endif // ORDERED_ARRAY_H
