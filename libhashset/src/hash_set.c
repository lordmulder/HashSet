/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#include "hash_set.h"

/* CRT */
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <float.h>

typedef int bool_t;
#define TRUE 1
#define FALSE 0

#if defined(__GNUC__)
#  define INLINE __inline__
#  define FORCE_INLINE __attribute__((always_inline)) __inline__
#elif defined(_MSC_VER)
#  define INLINE __inline
#  define FORCE_INLINE __forceinline
#else
#  define INLINE
#endif

struct _hash_set_data
{
	uint64_t *values;
	uint8_t *used, *deleted;
	size_t capacity;
};

struct _hash_set
{
	double load_factor;
	uint16_t options;
	size_t valid, deleted, limit;
	struct _hash_set_data data;
};

static const size_t MINIMUM_CAPACITY =   128U;
static const size_t DEFAULT_CAPACITY = 16384U;
static const double DEFAULT_LOADFCTR = 0.8125;

/* ========================================================================= */
/* PRIVATE FUNCTIONS                                                         */
/* ========================================================================= */

#define IS_VALID(X,Y) (get_flag((X).used, (Y)) && (!get_flag((X).deleted, (Y))))

#define BOUND(MIN,VAL,MAX) (((VAL) < (MIN)) ? (MIN) : (((VAL) > (MAX)) ? (MAX) : (VAL)))

#define SAFE_FREE(X) do { if ((X)) { free((X)); (X) = NULL; } } while(0)

/* ------------------------------------------------- */
/* Math                                              */
/* ------------------------------------------------- */

static FORCE_INLINE size_t safe_mult(const size_t a, const size_t b)
{
	const size_t result = a * b;
	return  ((a == 0U) || (result / a == b)) ? result : SIZE_MAX;
}

static FORCE_INLINE size_t div_ceil(const size_t value, const size_t divisor)
{
	return (value / divisor) + ((value % divisor != 0U) ? 1U : 0U);
}

static FORCE_INLINE size_t round_sz(double d)
{
	return (d >= 0.0) ? ((d + 0.5 >= ((double)SIZE_MAX)) ? SIZE_MAX : ((size_t)(d + 0.5))) : 0U;
}

static FORCE_INLINE size_t safe_add(const size_t a, const size_t b)
{
	return ((SIZE_MAX - a) > b) ? (a + b) : SIZE_MAX;
}

static FORCE_INLINE size_t safe_incr(const size_t value)
{
	return (value < SIZE_MAX) ? (value + 1U) : value;
}

static FORCE_INLINE size_t safe_decr(const size_t value)
{
	return (value > 0U) ? (value - 1U) : value;
}

static FORCE_INLINE size_t next_pow2(const size_t target)
{
	size_t result = MINIMUM_CAPACITY;

	while (result < target)
	{
		result = safe_mult(result, 2U);
	}

	return result;
}

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

static INLINE uint64_t hash_compute(const uint64_t i, const uint64_t value)
{
	uint64_t hash = UINT64_C(14695981039346656037);
	hash_update(&hash, i);
	hash_update(&hash, value);
	return hash;
}

/* ------------------------------------------------- */
/* Allocation                                        */
/* ------------------------------------------------- */

static INLINE void zero_memory(void* const addr, const size_t count, const size_t size)
{
	memset(addr, 0, safe_mult(count, size));
}

static INLINE bool_t alloc_data(struct _hash_set_data *const data, const size_t capacity)
{
	zero_memory(data, 1U, sizeof(struct _hash_set_data));
	
	data->values = (uint64_t*) calloc(capacity, sizeof(uint64_t));
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

	data->deleted = (uint8_t*) calloc(div_ceil(capacity,  8U), sizeof(uint8_t));
	if (!data->deleted)
	{
		SAFE_FREE(data->used);
		SAFE_FREE(data->values);
		return FALSE;
	}

	data->capacity = capacity;
	return TRUE;
}

static INLINE void free_data(struct _hash_set_data *const data)
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
/* Flags                                             */
/* ------------------------------------------------- */

static INLINE bool_t get_flag(const uint8_t* const flags, const size_t index)
{
	return (flags[index / 8U] >> (index % 8U)) & 1U;
}

static INLINE void set_flag(uint8_t* const flags, const size_t index)
{
	flags[index / 8U] |= UINT8_C(1) << (index % 8U);
}

static INLINE void clear_flag(uint8_t* const flags, const size_t index)
{
	flags[index / 8U] &= ~(UINT8_C(1) << (index % 8U));
}

/* ------------------------------------------------- */
/* Set functions                                     */
/* ------------------------------------------------- */

#define INDEX(X) ((size_t)((X) % data->capacity))

static INLINE bool_t find_slot(const struct _hash_set_data *const data, const uint64_t value, size_t *const index_out)
{
	uint64_t loop = 0U;
	size_t index;
	bool_t index_saved = FALSE;

	for (index = INDEX(hash_compute(loop, value)); get_flag(data->used, index); index = INDEX(hash_compute(++loop, value)))
	{
		if (!get_flag(data->deleted, index))
		{
			if (data->values[index] == value)
			{
				if (index_out)
				{
					*index_out = index;
				}
				return TRUE;
			}
		}
		else if ((!index_saved) && index_out)
		{
			*index_out = index;
			index_saved = TRUE;
		}
	}

	if ((!index_saved) && index_out)
	{
		*index_out = index;
	}

	return FALSE;
}

static INLINE bool_t store_value(struct _hash_set_data *const data, const size_t index, const uint64_t value)
{
	data->values[index] = value;

	if (get_flag(data->used, index))
	{
		clear_flag(data->deleted, index);
		return FALSE;
	}
	else
	{
		set_flag(data->used, index);
		return TRUE;
	}
}

static INLINE errno_t rebuild_set(hash_set_t *const instance, const size_t new_capacity)
{
	struct _hash_set_data temp;
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
			const uint64_t value = instance->data.values[k];
			if (find_slot(&temp, value, &index))
			{
				free_data(&temp);
				return EFAULT; /*should never happen!*/
			}
			if (!store_value(&temp, index, value))
			{
				free_data(&temp);
				return EFAULT; /*should never happen!*/
			}
		}
	}

	free_data(&instance->data);
	instance->data = temp;
	instance->limit = round_sz(instance->data.capacity * instance->load_factor);
	instance->deleted = 0U;

	return 0;
}

/* ========================================================================= */
/* PUBLIC FUNCTIONS                                                          */
/* ========================================================================= */

hash_set_t *hash_set_create(const size_t initial_capacity, const double load_factor, const uint16_t options)
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
	instance->options = options;
	instance->limit = round_sz(instance->data.capacity * instance->load_factor);

	return instance;
}

void hash_set_destroy(hash_set_t *instance)
{
	if (instance)
	{
		free_data(&instance->data);
		zero_memory(instance, 1U, sizeof(hash_set_t));
		SAFE_FREE(instance);
	}
}

errno_t hash_set_insert(hash_set_t *const instance, const uint64_t value)
{
	size_t index;

	if ((!instance) || (!instance->data.values))
	{
		return EINVAL;
	}

	if (find_slot(&instance->data, value, &index))
	{
		return EEXIST;
	}

	if (!store_value(&instance->data, index, value))
	{
		instance->deleted = safe_decr(instance->deleted);
	}

	instance->valid = safe_incr(instance->valid);

	while ((safe_add(instance->valid, instance->deleted) > instance->limit) || (instance->valid >= instance->data.capacity))
	{
		if (instance->data.capacity == SIZE_MAX)
		{
			if ((instance->options & HASHSET_OPT_FAILFAST) || (instance->valid >= instance->data.capacity))
			{
				return ENOMEM; /*malloc has failed!*/
			}
		}
		else
		{
			const errno_t error = rebuild_set(instance, safe_mult(instance->data.capacity, 2U));
			if (error)
			{
				if (error == ENOMEM)
				{
					if ((instance->options & HASHSET_OPT_FAILFAST) || (instance->valid >= instance->data.capacity))
					{
						return ENOMEM; /*malloc has failed!*/
					}
					instance->limit = SIZE_MAX;
				}
				else
				{
					return error;
				}
			}
		}
	}

	return 0;
}

errno_t hash_set_contains(const hash_set_t *const instance, const uint64_t value)
{
	if ((!instance) || (!instance->data.values))
	{
		return EINVAL;
	}

	return find_slot(&instance->data, value, NULL) ? 0 : ENOENT;
}

errno_t hash_set_remove(hash_set_t *const instance, const uint64_t value)
{
	size_t index;

	if ((!instance) || (!instance->data.values))
	{
		return EINVAL;
	}

	if (!find_slot(&instance->data, value, &index))
	{
		return ENOENT;
	}

	set_flag(instance->data.deleted, index);
	instance->deleted = safe_incr(instance->deleted);
	instance->valid = safe_decr(instance->valid);

	if (instance->valid == 0U)
	{
		hash_set_clear(instance); /*optimization*/
	}

	return 0;
}

errno_t hash_set_clear(hash_set_t *const instance)
{
	if ((!instance) || (!instance->data.values))
	{
		return EINVAL;
	}

	if ((instance->valid == 0U) && (instance->deleted == 0U))
	{
		return EAGAIN;
	}

	zero_memory(instance->data.used,    div_ceil(instance->data.capacity, 8U), sizeof(uint8_t));
	zero_memory(instance->data.deleted, div_ceil(instance->data.capacity, 8U), sizeof(uint8_t));

	instance->valid = instance->deleted = 0U;
	return 0;
}

errno_t hash_set_shrink(hash_set_t *const instance)
{
	if ((!instance) || (!instance->data.values))
	{
		return EINVAL;
	}

	if (instance->data.capacity > MINIMUM_CAPACITY)
	{
		const size_t target_capacity = next_pow2(round_sz(safe_add(instance->valid, MINIMUM_CAPACITY) / instance->load_factor));
		if ((instance->data.capacity > target_capacity) || (instance->deleted > 0U))
		{
			return rebuild_set(instance, target_capacity);
		}
	}

	return EAGAIN;
}

errno_t hash_set_iterate(const hash_set_t *const instance, size_t *const offset, uint64_t *const value)
{
	size_t index;

	if ((!instance) || (!offset) || (!instance->data.values))
	{
		return EINVAL;
	}

	for (index = *offset; index < instance->data.capacity; ++index)
	{
		if (IS_VALID(instance->data, index))
		{
			if (value)
			{
				*value = instance->data.values[index];
			}
			*offset = index + 1U;
			return 0;
		}
	}

	*offset = SIZE_MAX;
	return ENOENT;
}

size_t hash_set_size(const hash_set_t *const instance)
{
	return instance ? instance->valid : 0U;
}

errno_t hash_set_info(const hash_set_t *const instance, size_t *const capacity, size_t *const valid, size_t *const deleted, size_t *const limit)
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
