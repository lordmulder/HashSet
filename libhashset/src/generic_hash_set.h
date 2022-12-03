/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#ifndef _LIBHASHSET_GENERIC_SET_INCLUDED
#define _LIBHASHSET_GENERIC_SET_INCLUDED

#include "common.h"

#ifndef NAME_SUFFIX
#error NAME_SUFFIX must be defined!
#endif

#define DECLARE(X) CONCAT(X,NAME_SUFFIX)

/* ------------------------------------------------- */
/* Data types                                        */
/* ------------------------------------------------- */

typedef struct DECLARE(_hash_set_data)
{
	value_t *items;
	uint8_t *used, *deleted;
	size_t capacity;
}
hash_data_t;

struct DECLARE(_hash_set)
{
	double load_factor;
	size_t valid, deleted, limit;
	uint64_t basis;
	hash_data_t data;
};

/* ------------------------------------------------- */
/* Allocation                                        */
/* ------------------------------------------------- */

static INLINE bool_t alloc_data(hash_data_t *const data, const size_t capacity)
{
	zero_memory(data, 1U, sizeof(hash_data_t));

	data->items = (value_t*) calloc(capacity, sizeof(value_t));
	if (!data->items)
	{
		return FALSE;
	}

	data->used = (uint8_t*) calloc(div_ceil(capacity, 8U), sizeof(uint8_t));
	if (!data->used)
	{
		SAFE_FREE(data->items);
		return FALSE;
	}

	data->deleted = (uint8_t*) calloc(div_ceil(capacity, 8U), sizeof(uint8_t));
	if (!data->deleted)
	{
		SAFE_FREE(data->used);
		SAFE_FREE(data->items);
		return FALSE;
	}

	data->capacity = capacity;
	return TRUE;
}

static INLINE void free_data(hash_data_t *const data)
{
	if (data)
	{
		SAFE_FREE(data->items);
		SAFE_FREE(data->used);
		SAFE_FREE(data->deleted);
		data->capacity = 0U;
	}
}

/* ------------------------------------------------- */
/* Set functions                                     */
/* ------------------------------------------------- */

#define INDEX(X) ((size_t)((X) % data->capacity))

static INLINE bool_t find_slot(const hash_data_t *const data, const uint64_t basis, const value_t item, size_t *const index_out, bool_t *const reused_out)
{
	size_t index;
	bool_t is_saved = FALSE;
	uint64_t loop = 0U;

	for (index = INDEX(hash_compute(basis, loop, item)); get_flag(data->used, index); index = INDEX(hash_compute(basis, ++loop, item)))
	{
		if (get_flag(data->deleted, index))
		{
			if (!is_saved)
			{
				SAFE_SET(index_out, index);
				SAFE_SET(reused_out, TRUE);
				is_saved = TRUE;
			}
		}
		else
		{
			if (data->items[index] == item)
			{
				SAFE_SET(index_out, index);
				SAFE_SET(reused_out, FALSE);
				return TRUE;
			}
		}
	}

	if (!is_saved)
	{
		SAFE_SET(index_out, index);
		SAFE_SET(reused_out, FALSE);
	}

	return FALSE;
}

static INLINE void put_item(hash_data_t *const data, const size_t index, const value_t item, const bool_t reusing)
{
	data->items[index] = item;

	if (reusing)
	{
		assert(get_flag(data->used, index));
		clear_flag(data->deleted, index);
	}
	else
	{
		assert(!get_flag(data->deleted, index));
		set_flag(data->used, index);
	}
}

static INLINE size_t compute_limit(const size_t capacity, const double load_factor)
{
	size_t limit = round_sz(capacity * load_factor);

	while (capacity && (limit >= capacity))
	{
		limit = safe_decr(limit);
	}

	return limit;
}

static INLINE errno_t rebuild_set(hash_set_t *const instance, const size_t new_capacity)
{
	size_t k, index = SIZE_MAX;
	hash_data_t temp;

	if (new_capacity < instance->valid)
	{
		return EINVAL;
	}

	if (!alloc_data(&temp, new_capacity))
	{
		return ENOMEM;
	}

	for (k = 0U; k < instance->data.capacity; ++k)
	{
		if (IS_VALID(instance->data, k))
		{
			const value_t item = instance->data.items[k];
			if (find_slot(&temp, instance->basis, item, &index, NULL))
			{
				free_data(&temp);
				return EFAULT; /*this should never happen!*/
			}
			put_item(&temp, index, item, FALSE);
		}
	}

	free_data(&instance->data);
	instance->data = temp;
	instance->limit = compute_limit(instance->data.capacity, instance->load_factor);
	instance->deleted = 0U;

	return 0;
}

/* ========================================================================= */
/* PUBLIC FUNCTIONS                                                          */
/* ========================================================================= */

hash_set_t *DECLARE(hash_set_create)(const size_t initial_capacity, const double load_factor, const uint64_t seed)
{
	hash_set_t *instance = (hash_set_t*) calloc(1U, sizeof(hash_set_t));
	if (!instance)
	{
		return NULL;
	}

	if (!alloc_data(&instance->data, (initial_capacity > 0U) ? next_pow2(initial_capacity) : DEFAULT_CAPACITY))
	{
		SAFE_FREE(instance);
		return NULL;
	}

	instance->load_factor = (load_factor > DBL_EPSILON) ? BOUND(0.125, load_factor, 1.0) : DEFAULT_LOADFCTR;
	instance->basis = hash_initialize(seed);
	instance->limit = compute_limit(instance->data.capacity, instance->load_factor);

	return instance;
}

void DECLARE(hash_set_destroy)(hash_set_t *instance)
{
	if (instance)
	{
		free_data(&instance->data);
		zero_memory(instance, 1U, sizeof(hash_set_t));
		SAFE_FREE(instance);
	}
}

errno_t DECLARE(hash_set_insert)(hash_set_t *const instance, const value_t item)
{
	size_t index = SIZE_MAX;
	bool_t slot_reused;

	if ((!instance) || (!instance->data.items))
	{
		return EINVAL;
	}

	if (find_slot(&instance->data, instance->basis, item, &index, &slot_reused))
	{
		return EEXIST;
	}

	if ((!slot_reused) && (safe_add(instance->valid, instance->deleted) >= instance->limit))
	{
		const errno_t error = rebuild_set(instance, safe_times2(instance->data.capacity));
		if (error)
		{
			return error;
		}
		if (find_slot(&instance->data, instance->basis, item, &index, &slot_reused))
		{
			return EFAULT;
		}
	}

	put_item(&instance->data, index, item, slot_reused);

	instance->valid = safe_incr(instance->valid);
	if (slot_reused)
	{
		instance->deleted = safe_decr(instance->deleted);
	}

	return 0;
}

errno_t DECLARE(hash_set_contains)(const hash_set_t *const instance, const value_t item)
{
	if ((!instance) || (!instance->data.items))
	{
		return EINVAL;
	}

	return (instance->valid && find_slot(&instance->data, instance->basis, item, NULL, NULL)) ? 0 : ENOENT;
}

errno_t DECLARE(hash_set_remove)(hash_set_t *const instance, const value_t item)
{
	size_t index;

	if ((!instance) || (!instance->data.items))
	{
		return EINVAL;
	}

	if ((!instance->valid) || (!find_slot(&instance->data, instance->basis, item, &index, NULL)))
	{
		return ENOENT;
	}

	set_flag(instance->data.deleted, index);
	instance->deleted = safe_incr(instance->deleted);
	instance->valid = safe_decr(instance->valid);

	if (!instance->valid)
	{
		return DECLARE(hash_set_clear)(instance);
	}

	if (instance->deleted > (instance->limit / 2U))
	{
		const size_t min_capacity = next_pow2(round_sz(safe_incr(instance->valid) / instance->load_factor));
		const errno_t error = rebuild_set(instance, (instance->data.capacity > min_capacity) ? min_capacity : instance->data.capacity);
		if (error && (error != ENOMEM))
		{
			return error;
		}
	}

	return 0;
}

errno_t DECLARE(hash_set_clear)(hash_set_t *const instance)
{
	if ((!instance) || (!instance->data.items))
	{
		return EINVAL;
	}

	if (instance->valid || instance->deleted)
	{
		const size_t count = div_ceil(instance->data.capacity, 8U);
		instance->valid = instance->deleted = 0U;
		zero_memory(instance->data.used, count, sizeof(uint8_t));
		zero_memory(instance->data.deleted, count, sizeof(uint8_t));
	}
	else
	{
		return EAGAIN;
	}

	if (instance->data.capacity > MINIMUM_CAPACITY)
	{
		const errno_t error = rebuild_set(instance, MINIMUM_CAPACITY);
		if (error && (error != ENOMEM))
		{
			return error;
		}
	}

	return 0;
}

errno_t DECLARE(hash_set_iterate)(const hash_set_t *const instance, size_t *const cursor, value_t *const item)
{
	size_t index;

	if ((!instance) || (!cursor) || (*cursor >= SIZE_MAX) || (!instance->data.items))
	{
		return EINVAL;
	}

	for (index = *cursor; index < instance->data.capacity; ++index)
	{
		if (IS_VALID(instance->data, index))
		{
			SAFE_SET(item, instance->data.items[index]);
			*cursor = index + 1U;
			return 0;
		}
	}

	*cursor = SIZE_MAX;
	return ENOENT;
}

size_t DECLARE(hash_set_size)(const hash_set_t *const instance)
{
	return instance ? instance->valid : 0U;
}

errno_t DECLARE(hash_set_info)(const hash_set_t *const instance, size_t *const capacity, size_t *const valid, size_t *const deleted, size_t *const limit)
{
	if ((!instance) || (!instance->data.items))
	{
		return EINVAL;
	}

	SAFE_SET(capacity, instance->data.capacity);
	SAFE_SET(valid, instance->valid);
	SAFE_SET(deleted, instance->deleted);
	SAFE_SET(limit, instance->limit);

	return 0;
}

HASHSET_API errno_t DECLARE(hash_set_dump)(const hash_set_t *const instance, const hash_set_callback_t callback)
{
	size_t index;

	if ((!instance) || (!instance->data.items) || (!callback))
	{
		return EINVAL;
	}

	for (index = 0U; index < instance->data.capacity; ++index)
	{
		if (!callback(index, get_flag(instance->data.used, index) ? (get_flag(instance->data.deleted, index) ? 'd' : 'v') : 'u', instance->data.items[index]))
		{
			return ECANCELED;
		}
	}

	return 0;
}

#endif /*_LIBHASHSET_GENERIC_SET_INCLUDED*/
