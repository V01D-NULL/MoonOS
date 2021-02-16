#include "orderedArray.h"
#include "mem/heap.h"
#include "panic.h"

// A standard less than predicate
s8int standard_lessthan_predicate ( type_t a, type_t b )
{
	return ( a < b ) ? 1 : 0;
}

// Create an ordered array (uses kmalloc)
ordered_array_t orderedArray_create ( u32int max_size, lessthan_predicate_t less_than )
{
	ordered_array_t a;
	u32int address;

	address = kmalloc( max_size * sizeof( type_t ) );
	a.array = ( void * ) address;
	memset( ( u8int * ) address, 0, max_size * sizeof( type_t ) );  // fill with zeros

	a.size = 0;
	a.max_size = max_size;
	a.less_than = less_than;

	return a;
}

// Create an ordered array (uses given start location)
ordered_array_t orderedArray_place ( void *address, u32int max_size, lessthan_predicate_t less_than )
{
	ordered_array_t a;

	a.array = ( type_t * ) address;
	memset( ( u8int * ) address, 0, max_size * sizeof( type_t ) );  // fill with zeros

	a.size = 0;
	a.max_size = max_size;
	a.less_than = less_than;

	return a;
}

// Add an item into the array
void orderedArray_insert ( type_t item, ordered_array_t *array )
{
	u32int i;
	type_t tmp, tmp2;

	ASSERT( array -> less_than );

	// Iterate till find value greater or equal to item
	i = 0;
	while( i < array -> size &&
	       array -> less_than( array -> array[ i ], item ) )  // array[i] < item
	{
		i += 1;
	}

	// If reached end of array, just append the item
	if ( i == array -> size )
	{
		array -> array[ array -> size ] = item;

		array -> size += 1;
	}

	// Else, insert
	else
	{
		tmp = array -> array[ i ];

		array -> array[ i ] = item;

		while ( i < array -> size )
		{
			i += 1;

			tmp2 = array -> array[ i ];

			array -> array[ i ] = tmp;

			tmp = tmp2;
		}

		array -> size += 1;
	}
}

// Destroy an ordered array
void orderedArray_destroy ( ordered_array_t *array )
{
	// kfree( array -> array );
}

// Lookup the item at index i
type_t orderedArray_lookup ( u32int i, ordered_array_t *array )
{
	ASSERT( i < array -> size );

	return array -> array[ i ];
}

// Deletes the item at location i from the array
void orderedArray_remove ( u32int i, ordered_array_t *array )
{
	while ( i < array -> size )
	{
		array -> array[ i ] = array -> array[ i + 1 ];

		i += 1;
	}

	array -> size -= 1;
}
