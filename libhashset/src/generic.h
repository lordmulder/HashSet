/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#ifndef _LIBHASHSET_GENERIC_INCLUDED
#define _LIBHASHSET_GENERIC_INCLUDED

#ifndef NAME_SUFFIX
#error NAME_SUFFIX must be defined!
#endif

#define MAKE_NAME(X) NAME_GLUE(X,NAME_SUFFIX)

#include "common.h"

/* ------------------------------------------------- */
/* Data types                                        */
/* ------------------------------------------------- */

#define DATA_STRUCT MAKE_NAME(_hash_data)

struct DATA_STRUCT
{
	value_t *values;
	uint8_t *used, *deleted;
	size_t capacity;
};

struct MAKE_NAME(_hash_set)
{
	double load_factor;
	size_t valid, deleted, limit;
	struct DATA_STRUCT data;
};

/* ------------------------------------------------- */
/* Hash function                                     */
/* ------------------------------------------------- */

static FORCE_INLINE void hash_update(uint64_t *const hash, uint64_t value)
{
	do
	{
		*hash ^= value & 0xFF;
		*hash *= UINT64_C(1099511628211);
	}
	while (value >>= CHAR_BIT);
}

static INLINE uint64_t hash_compute(const uint64_t i, const value_t value)
{
	uint64_t hash = UINT64_C(14695981039346656037);
	hash_update(&hash, i);
	hash_update(&hash, value);
	return hash;
}

/* ------------------------------------------------- */
/* Allocation                                        */
/* ------------------------------------------------- */

static INLINE void zero_memory(void *const addr, const size_t count, const size_t size)
{
	memset(addr, 0, safe_mult(count, size));
}

static INLINE bool_t alloc_data(struct DATA_STRUCT *const data, const size_t capacity)
{
	zero_memory(data, 1U, sizeof(struct DATA_STRUCT));

	data->values = (value_t*) calloc(capacity, sizeof(value_t));
	if (!data->values)
	{
		return FALSE;
	}

	data->used = (uint8_t*) calloc(div_ceil(capacity, 8U), sizeof(uint8_t));
	if (!data->used)
	{
		SAFE_FREE(data->values);
		return FALSE;
	}

	data->deleted = (uint8_t*) calloc(div_ceil(capacity, 8U), sizeof(uint8_t));
	if (!data->deleted)
	{
		SAFE_FREE(data->used);
		SAFE_FREE(data->values);
		return FALSE;
	}

	data->capacity = capacity;
	return TRUE;
}

static INLINE void free_data(struct DATA_STRUCT *const data)
{
	if (data)
	{
		SAFE_FREE(data->values);
		SAFE_FREE(data->used);
		SAFE_FREE(data->deleted);
		data->capacity = 0U;
	}
}

/* ------------------------------------------------- */
/* Set functions                                     */
/* ------------------------------------------------- */

#define INDEX(X) ((size_t)((X) % data->capacity))

static INLINE bool_t find_slot(const struct DATA_STRUCT *const data, const value_t value, size_t *const index_out, bool_t *const reused_out)
{
	uint64_t loop = 0U;
	bool_t is_saved = FALSE;
	size_t index;

	for (index = INDEX(hash_compute(loop, value)); get_flag(data->used, index); index = INDEX(hash_compute(++loop, value)))
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
			if (data->values[index] == value)
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

static INLINE void put_value(struct DATA_STRUCT *const data, const size_t index, const value_t value, const bool_t reusing)
{
	data->values[index] = value;
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

static size_t INLINE compute_limit(const size_t capacity, const double load_factor)
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
	struct DATA_STRUCT temp;
	size_t index, k;

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
			const value_t value = instance->data.values[k];
			if (find_slot(&temp, value, &index, NULL))
			{
				free_data(&temp);
				return EFAULT; /*this should never happen!*/
			}
			put_value(&temp, index, value, FALSE);
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

hash_set_t* MAKE_NAME(hash_set_create)(const size_t initial_capacity, const double load_factor)
{
	hash_set_t* instance = (hash_set_t*) calloc(1U, sizeof(hash_set_t));
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
	instance->limit = compute_limit(instance->data.capacity, instance->load_factor);

	return instance;
}

void MAKE_NAME(hash_set_destroy)(hash_set_t *instance)
{
	if (instance)
	{
		free_data(&instance->data);
		zero_memory(instance, 1U, sizeof(hash_set_t));
		SAFE_FREE(instance);
	}
}

errno_t MAKE_NAME(hash_set_insert)(hash_set_t *const instance, const value_t value)
{
	size_t index;
	bool_t slot_reused;

	if ((!instance) || (!instance->data.values))
	{
		return EINVAL;
	}

	if (find_slot(&instance->data, value, &index, &slot_reused))
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
		if (find_slot(&instance->data, value, &index, &slot_reused))
		{
			return EFAULT;
		}
	}

	put_value(&instance->data, index, value, slot_reused);

	instance->valid = safe_incr(instance->valid);
	if (slot_reused)
	{
		instance->deleted = safe_decr(instance->deleted);
	}

	return 0;
}

errno_t MAKE_NAME(hash_set_contains)(const hash_set_t *const instance, const value_t value)
{
	if ((!instance) || (!instance->data.values))
	{
		return EINVAL;
	}

	return (instance->valid && find_slot(&instance->data, value, NULL, NULL)) ? 0 : ENOENT;
}

errno_t MAKE_NAME(hash_set_remove)(hash_set_t *const instance, const value_t value)
{
	size_t index;

	if ((!instance) || (!instance->data.values))
	{
		return EINVAL;
	}

	if ((!instance->valid) || (!find_slot(&instance->data, value, &index, NULL)))
	{
		return ENOENT;
	}

	set_flag(instance->data.deleted, index);
	instance->deleted = safe_incr(instance->deleted);
	instance->valid = safe_decr(instance->valid);

	if (!instance->valid)
	{
		return MAKE_NAME(hash_set_clear)(instance);
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

errno_t MAKE_NAME(hash_set_clear)(hash_set_t *const instance)
{
	if ((!instance) || (!instance->data.values))
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

errno_t MAKE_NAME(hash_set_iterate)(const hash_set_t *const instance, uintptr_t *const cursor, value_t *const value)
{
	size_t index;

	if ((!instance) || (!cursor) || (*cursor >= SIZE_MAX) || (!instance->data.values))
	{
		return EINVAL;
	}

	for (index = (size_t)(*cursor); index < instance->data.capacity; ++index)
	{
		if (IS_VALID(instance->data, index))
		{
			if (value)
			{
				*value = instance->data.values[index];
			}
			*cursor = (uintptr_t)(index + 1U);
			return 0;
		}
	}

	*cursor = (uintptr_t)SIZE_MAX;
	return ENOENT;
}

size_t MAKE_NAME(hash_set_size)(const hash_set_t *const instance)
{
	return instance ? instance->valid : 0U;
}

errno_t MAKE_NAME(hash_set_info)(const hash_set_t *const instance, size_t *const capacity, size_t *const valid, size_t *const deleted, size_t *const limit)
{
	if ((!instance) || (!instance->data.values))
	{
		return EINVAL;
	}

	if (capacity)
	{
		*capacity = instance->data.capacity;
	}
	if (valid)
	{
		*valid = instance->valid;
	}
	if (deleted)
	{
		*deleted = instance->deleted;
	}
	if (limit)
	{
		*limit = instance->limit;
	}

	return 0;
}

HASHSET_API errno_t MAKE_NAME(hash_set_dump)(const hash_set_t *const instance, int (*const callback)(const size_t index, const int status, const value_t value))
{
	size_t index;

	if ((!instance) || (!instance->data.values))
	{
		return EINVAL;
	}

	for (index = 0U; index < instance->data.capacity; ++index)
	{
		if (!callback(index, get_flag(instance->data.used, index) ? (get_flag(instance->data.deleted, index) ? 2 : 1) : 0, instance->data.values[index]))
		{
			return ECANCELED;
		}
	}

	return 0;
}

#endif /* _LIBHASHSET_GENERIC_INCLUDED */
