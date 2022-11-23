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


API Reference
=============

Types
-----

### hash_set_t

A `struct` that represents a hash set instance. Instances can be allocated and de-allocated via the [hash_set_create()](#hashsetcreate) and [hash_set_destroy()](#hashsetdestroy) functions, respectively.

***Note:*** Application code shall treat this `struct` as opaque!

```C
typedef struct _hash_set hash_set_t;
```

Functions
---------

### hash_set_create()

Allocates a new hash set instance. The new hash set instance is empty initially.

```C
hash_set_t *hash_set_create(
	const size_t initial_capacity,
	const double load_factor,
	const uint16_t options
);
```

#### Parameters

* `initial_capacity`  
  The initial capacity of the hash set (number of items). The given value will be rounded to the next power of two. If set to *zero*, the default initial capacity is used.

* `load_factor`  
  The load factor to be used for the hash set. The given value will be clipped to the **0.1** to **1.0** range. If less than or equal to *zero*, the recommended default load factor is used.

* `options`  
  Bit mask that can contain zero or more of the following values OR'ed together:
  
  - `HASHSET_OPT_FAILFAST`  
    If an operation attempts to grow the hash set, but the hash set fails to grow (e.g. because of a low-memory situation), causes the operation to fail immediately.  
    By default, if the set can *not* be grown as desired, the operation may complete successfully anyways. As a result, the performance of the hash set may degrade!

#### Return value

On success, this function returns a pointer to a new hash set instance. On error, a `NULL` pointer is returned.

***Note:*** To avoid a memory leak, the returned pointer must be de-allocated using the [hash_set_destroy()](#hashsetdestroy) function, as soon as the instance is *not* needed anymore!

### hash_set_destroy()

De-allocates an existing hash set instance. All items in the hash set are discarded, and all memory occupied by the hash set is released.

```C
void hash_set_destroy(
	hash_set_t *instance
);
```

#### Parameters

* `instance`  
  A pointer to the existing hash set instance that is to be destroyed, as returned by the [hash_set_create()](#hashsetcreate) function.  
  ***Note:*** The given pointer is *invalidated* by this function, and it **must not** be used afterwards!

### hash_set_insert()

Tries to insert the given value into the hash set. The operation fails, if the set already contains the given value.

***Note:*** If the value is actually inserted, then is *may* cause the hash set to grow.

```C
errno_t hash_set_insert(
	hash_set_t *const instance,
	const uint64_t value
);
```
#### Parameters

* `instance`  
  A pointer to the hash set instance to be used by this operation, as returned by the [hash_set_create()](#hashsetcreate) function.

* `value`  
  The value (key) to be inserted into the hash set. It can be *any* value in the `0U` to `UINT64_MAX` range.

#### Return value

On success, this function returns *zero*. On error, the appropriate error code is returned. Possible error codes include:

* `EINVAL`  
  An invalid argument was given, e.g. `instance` was set to `NULL`.

* `EEXIST`  
  The given value (key) was *not* inserted into the hash set, because the hash set already contained that value.

* `ENOMEM`  
  The value could *not* be inserted, because the required amount of memory could *not* be allocated.

* `EFAULT`  
  Something else went wrong. This usually indicates an internal error (inconsistency) and is *not* supposed to happen.

### hash_set_remove()

```C
errno_t hash_set_remove(
	hash_set_t *const instance,
	const uint64_t value
);
```

### hash_set_clear()

```C
errno_t hash_set_clear(
	hash_set_t *const instance
);
```

### hash_set_shrink()

```C
errno_t hash_set_shrink(
	hash_set_t *const instance
);
```

### hash_set_contains()

```C
errno_t hash_set_contains(
	const hash_set_t *const instance,
	const uint64_t value
);
```

### hash_set_iterate()

```C
errno_t hash_set_iterate(
	const hash_set_t *const instance,
	size_t *const offset,
	uint64_t *const value
);
```

### hash_set_size()

```C
size_t hash_set_size(
	const hash_set_t *const instance
);
```

### hash_set_info()

```C
errno_t hash_set_info(
	const hash_set_t *const instance,
	size_t *const capacity,
	size_t *const valid,
	size_t *const deleted,
	size_t *const limit
);
```


License
=======

This work has been released under the **CC0 1.0 Universal** license.

For details, please refer to:  
<https://creativecommons.org/publicdomain/zero/1.0/legalcode>


&marker;
