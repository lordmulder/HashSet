Introduction
============

**LibHashSet** is a [*hash set*](https://en.wikipedia.org/wiki/Hash_table) and [*hash map*](https://en.wikipedia.org/wiki/Hash_table) implementation for C99. It uses open addressing and double hashing.

At this time, the *only* types of elements supported are `uint16_t`, `uint32_t` and `uint64_t`.

This hash set implementation has successfully been tested to *efficiently* handle several billions of items 😏


Getting Started
===============

Here is a simple example of how to use LibHashSet in your application:

```C
#include <hash_set.h>
#include <stdio.h>

int main(void)
{
	uint64_t item;
	uintptr_t cursor = 0U;

	/* create new hash set instance */
	hash_set64_t* const hash_set = hash_set_create64(0U, -1.0, 42U);
	if (!hash_set)
	{
		fputs("Allocation has failed!\n", stderr);
		return EXIT_FAILURE;
	}

	/* add a number of items to the hash set, the set will grow as needed */
	puts("Insertign items, please wait...");
	while (have_more_items())
	{
		const errno_t error = hash_set_insert64(hash_set, get_next_item());
		if (error)
		{
			fprintf(stderr, "Insert operation has failed! (error: %d)\n", error);
			return EXIT_FAILURE;
		}
	}
	puts("Done.\n");

	/* print total number of items in the hash set*/
	printf("Total number of items: %zu\n\n", hash_set_size64(hash_set));

	/* print all items in the set */
	while (hash_set_iterate64(hash_set, &cursor, &item) == 0)
	{
		printf("Item: %016llX\n", item);
	}

	/* destroy the hash set, when it is no longer needed! */
	hash_set_destroy64(hash_set);
	return EXIT_SUCCESS;
}
```


API Reference
=============

This section describes the LibHashSet programming interface. The functions for managing *hash sets* are declared in `<hash_set.h>`, whereas the functions for managing to *hash maps* are declared in `<hash_map.h>`.

LibHashSet supports sets and maps containing elements of different integral types. For each element type, separate functions are provided. The functions for `uint16_t`, `uint32_t` and `uint64_t` can be distinguished by the suffix `…16`, `…32` and `…64`, respectively. In the following, the functions are described in their "generic" form.

***Note:*** On Microsoft Windows (Visual C++), when using LibHashSet as a "shared" library (DLL file), the macro `HASHSET_DLL` must be defined *before* including the `<hash_set.h>` or `<hash_map.h>` header files!

Types
-----

### hash_set_t

A `struct` that represents a LibHashSet *hash set* instance. Hash set instances can be allocated and de-allocated via the [hash_set_create()](#hash_set_create) and [hash_set_destroy()](#hash_set_destroy) functions, respectively.

***Note:*** Application code shall treat this `struct` as opaque. The internals may change in future versions!

```C
typedef struct _hash_set hash_set_t;
```

### hash_map_t

A `struct` that represents a LibHashSet *hash map* instance. Hash map instances can be allocated and de-allocated via the [hash_map_create()](#hash_map_create) and [hash_map_destroy()](#hash_map_destroy) functions, respectively.

***Note:*** Application code shall treat this `struct` as opaque! The internals may change in future versions!

```C
typedef struct _hash_map hash_map_t;
```

Globals
-------

### Version information

The *major*, *minor* and *patch* version of the LibHashSet library:

```C
extern const uint16_t HASHSET_VERSION_MAJOR;
extern const uint16_t HASHSET_VERSION_MINOR;
extern const uint16_t HASHSET_VERSION_PATCH;
```

### Build information

The build *date* and *time* of the LibHashSet library:

```C
extern const char *const HASHSET_BUILD_DATE;
extern const char *const HASHSET_BUILD_TIME;
```

Set Functions
-------------

This section describes all functions for managing `hash_set_t` instances.

### hash_set_create()

Allocates a new hash set instance. The new hash set instance is empty initially.

```C
hash_set_t *hash_set_create(
	const size_t initial_capacity,
	const double load_factor,
	const uint32_t seed
);
```

#### Parameters

* `initial_capacity`  
  The initial capacity of the hash set (number of items). The given count will be rounded to the next power of two. If the number of items to be inserted into the hash set can be estimated beforehand, then the initial capacity should be adjusted accordingly to avoid unnecessary re-allocations. In any case, the hash set will be able to grow dynamically as needed. If this parameter is set to *zero*, the the *default* initial capacity (8192) is used.

* `load_factor`  
  The load factor to be applied to the hash set. The given load factor will be clipped to the **0.1** to **1.0** range. Generally, the default load factor (0.8) offers a good trade-off between performance and memory usage. Higher load factors decrease the memory overhead, but also may increase the time required for insert, lookup and remove operations. If this parameter is less than or equal to *zero*, the *default* load factor is used.

* `seed`  
  The "seed" value that is used to tweak the internal hash computation. The application should set this parameter to a value that is hard to predict and that is unlikely to repeat (e.g., a high-resolution timer is suitable here).

#### Return value

On success, this function returns a pointer to a new hash set instance. On error, a `NULL` pointer is returned.

***Note:*** To avoid a memory leak, the returned pointer must be de-allocated by the application using the [hash_set_destroy()](#hash_set_destroy) function, as soon as the instance is *not* needed anymore!

### hash_set_destroy()

De-allocates an existing hash set instance. All items in the hash set are discarded.

```C
void hash_set_destroy(
	hash_set_t *instance
);
```

#### Parameters

* `instance`  
  A pointer to the hash set instance that is to be destroyed, as returned by the [hash_set_create()](#hash_set_create) function.  
  ***Note:*** The given pointer is *invalidated* by this function, and it **must not** be used afterwards!

### hash_set_insert()

Tries to insert the given item into the hash set. The operation fails, if the set already contains the given item.

***Note:*** If the item is actually inserted, then the hash set *may* need to grow.

```C
errno_t hash_set_insert(
	hash_set_t *const instance,
	const value_t item
);
```

#### Parameters

* `instance`  
  A pointer to the hash set instance to be modified, as returned by the [hash_set_create()](#hash_set_create) function.

* `item`  
  The item to be inserted into the hash set.

#### Return value

On success, this function returns *zero*. On error, the appropriate error code is returned. Possible error codes include:

* `EINVAL`  
  An invalid argument was given, e.g. `instance` was set to `NULL`.

* `EEXIST`  
  The given item was *not* inserted into the hash set (again), because it was already present.

* `ENOMEM`  
  The item could *not* be inserted, because the required amount of memory could *not* be allocated.

* `EFAULT`  
  Something else went wrong. This usually indicates an internal error and is *not* supposed to happen.

### hash_set_remove()

Tries to remove the given item from the hash set. The operation fails, if the set does *not* contain the given item.

***Note:*** If the item is actually removed, then the hash set *may* shrink.

```C
errno_t hash_set_remove(
	hash_set_t *const instance,
	const value_t item
);
```

#### Parameters

* `instance`  
  A pointer to the hash set instance to be modified, as returned by the [hash_set_create()](#hash_set_create) function.

* `item`  
  The item to be removed from the hash set.

#### Return value

On success, this function returns *zero*. On error, the appropriate error code is returned. Possible error codes include:

* `EINVAL`  
  An invalid argument was given, e.g. `instance` was set to `NULL`.

* `ENOENT`  
  The given item could *not* be removed from the hash set, because *no* such item was present.

* `EFAULT`  
  Something else went wrong. This usually indicates an internal error and is *not* supposed to happen.

### hash_set_clear()

Discards *all* items from the hash set at once.

```C
errno_t hash_set_clear(
	hash_set_t *const instance
);
```
#### Parameters

* `instance`  
  A pointer to the hash set instance to be modified, as returned by the [hash_set_create()](#hash_set_create) function.

#### Return value

On success, this function returns *zero*. On error, the appropriate error code is returned. Possible error codes include:

* `EINVAL`  
  An invalid argument was given, e.g. `instance` was set to `NULL`.

* `EAGAIN`  
  The hash set was *not* cleared, because it already was empty. Please try again later!

* `EFAULT`  
  Something else went wrong. This usually indicates an internal error and is *not* supposed to happen.

### hash_set_contains()

Tests whether the hash set contains an item. The operation fails, if the set does *not* contain the given item.

```C
errno_t hash_set_contains(
	const hash_set_t *const instance,
	const value_t item
);
```

#### Parameters

* `instance`  
  A pointer to the hash set instance to be examined, as returned by the [hash_set_create()](#hash_set_create) function.

* `item`  
  The item to be searched in the hash set.

#### Return value

On success, this function returns *zero*. On error, the appropriate error code is returned. Possible error codes include:

* `EINVAL`  
  An invalid argument was given, e.g. `instance` was set to `NULL`.

* `ENOENT`  
  The hash set does *not* contain the specified item.

* `EFAULT`  
  Something else went wrong. This usually indicates an internal error and is *not* supposed to happen.

### hash_set_iterate()

Iterates through the items stored in the hash set. The elements are iterated in **no** particular order.

This function returns one item at a time. It should be called *repeatedly*, until the end of the set is encountered.

***Warning:*** The result is *undefined*, if the set is modified while the iteration is in progress!

```C
errno_t hash_set_iterate(
	const hash_set_t *const instance,
	size_t *const cursor,
	value_t *const item
);
```

#### Parameters

* `instance`  
  A pointer to the hash set instance to be examined, as returned by the [hash_set_create()](#hash_set_create) function.

* `cursor`  
  A pointer to a variable of type `size_t` where the current iterator state (position) is saved.  
  This variable **must** be initialized to the value `0U`, by the calling application, prior to the the *first* invocation!  
  Each invocation will update the value of `*cursor`. This value **shall not** be altered by the application.

* `item`  
  A pointer to a variable of type `value_t` where the next item in the set is stored on success.  
  The content of the variable should be considered *undefined*, if the invocation has failed.

#### Return value

On success, this function returns *zero*. On error, the appropriate error code is returned. Possible error codes include:

* `EINVAL`  
  An invalid argument was given, e.g. `instance` was set to `NULL`.

* `ENOENT`  
  No more items. The end of the set has been encountered.

* `EFAULT`  
  Something else went wrong. This usually indicates an internal error and is *not* supposed to happen.

### hash_set_size()

Returns the current number of (distinct) items in the hash set.

```C
size_t hash_set_size(
	const hash_set_t *const instance
);
```

#### Parameters

* `instance`  
  A pointer to the hash set instance to be examined, as returned by the [hash_set_create()](#hash_set_create) function.

#### Return value

This function returns the number of (distinct) items in the hash set.

### hash_set_info()

Returns technical information about the hash set.

```C
errno_t hash_set_info(
	const hash_set_t *const instance,
	size_t *const capacity,
	size_t *const valid,
	size_t *const deleted,
	size_t *const limit
);
```

#### Parameters

* `instance`  
  A pointer to the hash set instance to be examined, as returned by the [hash_set_create()](#hash_set_create) function.

* `capacity`  
  A pointer to a variable of type `size_t` where the current total *capacity* of the hash set is stored.  
  This value will always be greater than or equal to the sum of the *valid* and *deleted* entries.

* `valid`  
  A pointer to a variable of type `size_t` where the current number of *valid* entries in the hash set is stored.  
  This value is equivalent to the return value of the [hash_set_size()](#hash_set_size) function.

* `deleted`  
  A pointer to a variable of type `size_t` where the current number of *deleted* entries in the hash set is stored.  
  For technical reasons, entires are *not* removed from the set immediately, but are marked as "deleted".

* `limit`  
  A pointer to a variable of type `size_t` where the current "grow" *limit* of the hash set is stored.  
  The hash set is grown automatically, as soon as the sum of the *valid* and *deleted* entries exceeds this limit.

#### Return value

On success, this function returns *zero*. On error, the appropriate error code is returned. Possible error codes include:

* `EINVAL`  
  An invalid argument was given, e.g. `instance` was set to `NULL`.

* `EFAULT`  
  Something else went wrong. This usually indicates an internal error and is *not* supposed to happen.

### hash_set_dump()

Dump the current status and content of all "slots" of the hash set.

```C
errno_t hash_set_dump(
	const hash_set_t *const instance,
	const hash_map_callback_t callback
);
```

#### Parameters

* `instance`  
  A pointer to the hash set instance to be examined, as returned by the [hash_set_create()](#hash_set_create) function.

* `callback`  
  A pointer to the callback function that will be invoked once for every "slot" in the hash set.

  The callback function is defined as follows:
  ```C
  typedef int (*hash_map_callback_t)(
  	const size_t index,
  	const char status,
  	const value_t key,
  	const value_t value
  );
  ```

  ##### Parameters

  * `index`  
    The index of the current "slot" within the hash set.

  * `status`  
    Indicates the status of the current "slot":
    - `'u'` &ndash; the slot is *unused*
    - `'v'` &ndash; the slot is *valid*
    - `'d'` &ndash; the slot is *deleted*

  * `item`  
    The item that is stored at the current "slot" index.

  ##### Return value

  If the function returns a *non-zero* value, the iteration continues; otherwise it is cancelled.

#### Return value

On success, this function returns *zero*. On error, the appropriate error code is returned. Possible error codes include:

* `EINVAL`  
  An invalid argument was given, e.g. `instance` was set to `NULL`.

* `ECANCELED`  
  The operation was cancelled by the calling application.

* `EFAULT`  
  Something else went wrong. This usually indicates an internal error and is *not* supposed to happen.

Map Functions
-------------

This section describes all functions for managing `hash_map_t` instances.

### hash_map_create()

Allocates a new hash map instance. The new hash map instance is empty initially.

```C
hash_map_t *hash_map_create(
	const size_t initial_capacity,
	const double load_factor,
	const uint32_t seed
);
```

#### Parameters

* `initial_capacity`  
  The initial capacity of the hash map. The given count will be rounded to the next power of two. If the number of key-value pairs to be inserted into the hash map can be estimated beforehand, then the initial capacity should be adjusted accordingly to avoid unnecessary re-allocations. In any case, the hash map will be able to grow dynamically as needed. If this parameter is map to *zero*, the the *default* initial capacity (8192) is used.

* `load_factor`  
  The load factor to be applied to the hash map. The given load factor will be clipped to the **0.1** to **1.0** range. Generally, the default load factor (0.8) offers a good trade-off between performance and memory usage. Higher load factors decrease the memory overhead, but also may increase the time required for insert, lookup and remove operations. If this parameter is less than or equal to *zero*, the *default* load factor is used.

* `seed`  
  The "seed" value that is used to tweak the internal hash computation. The application should set this parameter to a value that is hard to predict and that is unlikely to repeat (e.g., a high-resolution timer is suitable here).

#### Return value

On success, this function returns a pointer to a new hash map instance. On error, a `NULL` pointer is returned.

***Note:*** To avoid a memory leak, the returned pointer must be de-allocated by the application using the [hash_map_destroy()](#hash_map_destroy) function, as soon as the instance is *not* needed anymore!

### hash_map_destroy()

De-allocates an existing hash map instance. All key-value pairs in the hash map are discarded.

```C
void hash_map_destroy(
	hash_map_t *instance
);
```

#### Parameters

* `instance`  
  A pointer to the hash map instance that is to be destroyed, as returned by the [hash_map_create()](#hash_map_create) function.  
  ***Note:*** The given pointer is *invalidated* by this function, and it **must not** be used afterwards!

### hash_map_insert()

Tries to insert the given key-value pair into the hash map.

If the map already contains the given key, then the value associated with the existing key is updated.

***Note:*** If the key is actually inserted, then the hash map *may* need to grow.

```C
errno_t hash_map_insert(
	hash_map_t *const instance,
	const value_t key,
	const value_t value
);
```

#### Parameters

* `instance`  
  A pointer to the hash map instance to be modified, as returned by the [hash_map_create()](#hash_map_create) function.

* `key`  
  The key to be inserted into the hash map.

* `value`  
  The value to be associated with the given key.

#### Return value

On success, this function returns *zero*. On error, the appropriate error code is returned. Possible error codes include:

* `EINVAL`  
  An invalid argument was given, e.g. `instance` was map to `NULL`.

* `EEXIST`  
  The given key was *not* inserted into the hash map (again), because it was already present.  
  Nonetheless, the value associated with the existing key has been updated!

* `ENOMEM`  
  The key could *not* be inserted, because the required amount of memory could *not* be allocated.

* `EFAULT`  
  Something else went wrong. This usually indicates an internal error and is *not* supposed to happen.

### hash_map_remove()

Tries to remove the given key from the hash map. The operation fails, if the map does *not* contain the given key.

The value associated with the removed key is discarded.

***Note:*** If the key is actually removed, then the hash map *may* shrink.

```C
errno_t hash_map_remove(
	hash_map_t *const instance,
	const value_t key
);
```

#### Parameters

* `instance`  
  A pointer to the hash map instance to be modified, as returned by the [hash_map_create()](#hash_map_create) function.

* `key`  
  The key to be removed from the hash map.

#### Return value

On success, this function returns *zero*. On error, the appropriate error code is returned. Possible error codes include:

* `EINVAL`  
  An invalid argument was given, e.g. `instance` was map to `NULL`.

* `ENOENT`  
  The given key could *not* be removed from the hash map, because *no* such key was present.

* `EFAULT`  
  Something else went wrong. This usually indicates an internal error and is *not* supposed to happen.

### hash_map_clear()

Discards *all* key-value pairs from the hash map at once.

```C
errno_t hash_map_clear(
	hash_map_t *const instance
);
```
#### Parameters

* `instance`  
  A pointer to the hash map instance to be modified, as returned by the [hash_map_create()](#hash_map_create) function.

#### Return value

On success, this function returns *zero*. On error, the appropriate error code is returned. Possible error codes include:

* `EINVAL`  
  An invalid argument was given, e.g. `instance` was map to `NULL`.

* `EAGAIN`  
  The hash map was *not* cleared, because it already was empty. Please try again later!

* `EFAULT`  
  Something else went wrong. This usually indicates an internal error and is *not* supposed to happen.

### hash_map_contains()

Tests whether the hash map contains a key. The operation fails, if the map does *not* contain the given key.

```C
errno_t hash_map_contains(
	const hash_map_t *const instance,
	const value_t key
);
```

#### Parameters

* `instance`  
  A pointer to the hash map instance to be examined, as returned by the [hash_map_create()](#hash_map_create) function.

* `key`  
  The key to be searched in the hash map.

#### Return value

On success, this function returns *zero*. On error, the appropriate error code is returned. Possible error codes include:

* `EINVAL`  
  An invalid argument was given, e.g. `instance` was map to `NULL`.

* `ENOENT`  
  The hash map does *not* contain the specified key.

* `EFAULT`  
  Something else went wrong. This usually indicates an internal error and is *not* supposed to happen.

### hash_map_get()

Retrieves the value that is associated with the given key. The operation fails, if the map does *not* contain the key.

```C
errno_t hash_map_get(
	const hash_map_t *const instance,
	const value_t key,
	value_t *const value
);
```

#### Parameters

* `instance`  
  A pointer to the hash map instance to be examined, as returned by the [hash_map_create()](#hash_map_create) function.

* `key`  
  The key to be searched in the hash map.

* `value`  
  A pointer to a variable of type `value_t` where the value associated with the key is stored on success.  
  The content of the variable should be considered *undefined*, if the invocation has failed.

#### Return value

On success, this function returns *zero*. On error, the appropriate error code is returned. Possible error codes include:

* `EINVAL`  
  An invalid argument was given, e.g. `instance` was map to `NULL`.

* `ENOENT`  
  The hash map does *not* contain the specified key.

* `EFAULT`  
  Something else went wrong. This usually indicates an internal error and is *not* supposed to happen.

### hash_map_iterate()

Iterates through the key-value pairs stored in the hash map. The entries are iterated in **no** particular order.

This function returns one key-value pair at a time. It should be called *repeatedly*, until the end of the map is encountered.

***Warning:*** The result is *undefined*, if the map is modified while the iteration is in progress!

```C
errno_t hash_map_iterate(
	const hash_map_t *const instance,
	size_t *const cursor,
	value_t *const key,
	value_t *const value
);
```

#### Parameters

* `instance`  
  A pointer to the hash map instance to be examined, as returned by the [hash_map_create()](#hash_map_create) function.

* `cursor`  
  A pointer to a variable of type `size_t` where the current iterator state (position) is saved.  
  This variable **must** be initialized to the value `0U`, by the calling application, prior to the the *first* invocation!  
  Each invocation will update the value of `*cursor`. This value **shall not** be altered by the application.

* `key`  
  A pointer to a variable of type `value_t` where the next key in the map is stored on success.  
  The content of the variable should be considered *undefined*, if the invocation has failed.

* `value`  
  A pointer to a variable of type `value_t` where the next value in the map is stored on success.  
  The content of the variable should be considered *undefined*, if the invocation has failed.

#### Return value

On success, this function returns *zero*. On error, the appropriate error code is returned. Possible error codes include:

* `EINVAL`  
  An invalid argument was given, e.g. `instance` was map to `NULL`.

* `ENOENT`  
  No more entries. The end of the map has been encountered.

* `EFAULT`  
  Something else went wrong. This usually indicates an internal error and is *not* supposed to happen.

### hash_map_size()

Returns the current number of (distinct) keys in the hash map.

```C
size_t hash_map_size(
	const hash_map_t *const instance
);
```

#### Parameters

* `instance`  
  A pointer to the hash map instance to be examined, as returned by the [hash_map_create()](#hash_map_create) function.

#### Return value

This function returns the number of (distinct) keys in the hash map.

### hash_map_info()

Returns technical information about the hash map.

```C
errno_t hash_map_info(
	const hash_map_t *const instance,
	size_t *const capacity,
	size_t *const valid,
	size_t *const deleted,
	size_t *const limit
);
```

#### Parameters

* `instance`  
  A pointer to the hash map instance to be examined, as returned by the [hash_map_create()](#hash_map_create) function.

* `capacity`  
  A pointer to a variable of type `size_t` where the current total *capacity* of the hash map is stored.  
  This value will always be greater than or equal to the sum of the *valid* and *deleted* entries.

* `valid`  
  A pointer to a variable of type `size_t` where the current number of *valid* entries in the hash map is stored.  
  This value is equivalent to the return value of the [hash_map_size()](#hash_map_size) function.

* `deleted`  
  A pointer to a variable of type `size_t` where the current number of *deleted* entries in the hash map is stored.  
  For technical reasons, entires are *not* removed from the map immediately, but are marked as "deleted".

* `limit`  
  A pointer to a variable of type `size_t` where the current "grow" *limit* of the hash map is stored.  
  The hash map is grown automatically, as soon as the sum of the *valid* and *deleted* entries exceeds this limit.

#### Return value

On success, this function returns *zero*. On error, the appropriate error code is returned. Possible error codes include:

* `EINVAL`  
  An invalid argument was given, e.g. `instance` was map to `NULL`.

* `EFAULT`  
  Something else went wrong. This usually indicates an internal error and is *not* supposed to happen.

### hash_map_dump()

Dump the current status and content of all "slots" of the hash map.

```C
errno_t hash_map_dump(
	const hash_map_t *const instance,
	const hash_map_callback_t callback
);
```

#### Parameters

* `instance`  
  A pointer to the hash map instance to be examined, as returned by the [hash_map_create()](#hash_map_create) function.

* `callback`  
  A pointer to the callback function that will be invoked once for every "slot" in the hash map.

  The callback function is defined as follows:
  ```C
  typedef int (*hash_map_callback_t)(
  	const size_t index,
  	const char status,
  	const value_t key,
  	const value_t value
  );
  ```

  ##### Parameters

  * `index`  
    The index of the current "slot" within the hash map.

  * `status`  
    Indicates the status of the current "slot":
    - `'u'` &ndash; the slot is *unused*
    - `'v'` &ndash; the slot is *valid*
    - `'d'` &ndash; the slot is *deleted*

  * `key`  
    The key that is stored at the current "slot" index.

  * `value`  
    The value that is stored at the current "slot" index.

  ##### Return value

  If the function returns a *non-zero* value, the iteration continues; otherwise it is cancelled.

#### Return value

On success, this function returns *zero*. On error, the appropriate error code is returned. Possible error codes include:

* `EINVAL`  
  An invalid argument was given, e.g. `instance` was map to `NULL`.

* `ECANCELED`  
  The operation was cancelled by the calling application.

* `EFAULT`  
  Something else went wrong. This usually indicates an internal error and is *not* supposed to happen.

Thread Safety
-------------

LibHashSet is ***thread-safe***, in the sense that all public functions operate *exclusively* on the given `hash_set_t` or `hash_map_t` instance; there is **no** implicit shared "global" state. This means that **no** synchronization is required in multi-threaded applications, provided that each instance is created and accessed only by a ***single*** thread.

However, LibHashSet does **nothing** to synchronize access to a particular `hash_set_t` or `hash_map_t` instance! Consequently, in situations where the *same* instance needs to be shared across *multiple* concurrent threads, the calling application is responsible for serializing all access to the "shared" instance, e.g. by using a [*mutex*](https://pubs.opengroup.org/onlinepubs/007908799/xsh/pthread_mutex_lock.html) lock!


License
=======

This work has been released under the **CC0 1.0 Universal** license.

For details, please refer to:  
<https://creativecommons.org/publicdomain/zero/1.0/legalcode>


&marker;
