Introduction
============

**LibHashSet** is a simple *hash set* implementation for C99. It uses open addressing and double hashing.

At this time, the *only* type of elements (keys) supported is `uint64_t`.

This hash set implementation has been tested to *efficiently* handle several billions of items 😏


Getting Started
===============

Here is a simple example of how to use LibHashSet in your application:

```C
#include <hash_set.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	size_t offset = 0U;
	uint64_t value;

	/* create new hash set instance */
	hash_set_t *const hash_set = hash_set_create(0U, -1.0, 0U);
	if (!hash_set)
	{
		fputs("Allocation has failed!\n", stderr);
		return EXIT_FAILURE;
	}

	/* add a number of items to the hash set, the set will grow as needed */
	while (have_more_items())
	{
		const errno_t error = hash_set_insert(hash_set, get_next_item());
		if (error)
		{
			fprintf(stderr, "Insert operation has failed! (error: %d)\n", error);
			return EXIT_FAILURE;
		}
	}

	/* test whether hash set contains a specific item */
	if (hash_set_contains(hash_set, 42U) == 0)
	{
		puts("Set contains item!");

		/* remove the existing item from the hash set */
		if (hash_set_remove(hash_set, 42U) == 0)
		{
			puts("Item has been removed!");
		}
	}

	/* print total number of items in the hash set*/
	printf("Total number of items: %zu\n", hash_set_size(hash_set));

	/* print all items in the set */
	while (hash_set_iterate(hash_set, &offset, &value) == 0)
	{
		printf("Item: %016llX\n", value);
	}

	/* destroy the hash set, when it is no longer needed! */
	hash_set_destroy(hash_set);
	return EXIT_SUCCESS;
}
```


License
=======

This work has been released under the **CC0 1.0 Universal** license.

For details, please refer to:  
<https://creativecommons.org/publicdomain/zero/1.0/legalcode>


&marker;
