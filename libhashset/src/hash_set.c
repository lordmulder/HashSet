/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#include "hash_set.h"

/* CRT */
#include <string.h>
#include <errno.h>

typedef int bool_t;
#define TRUE 1
#define FALSE 0

#if defined(__GNUC__)
#  define INLINE __inline__
#elif defined(_MSC_VER)
#  define INLINE __inline
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
	size_t total, valid, limit;
	struct _hash_set_data data;
};

#define IS_VALID(X,Y) (get_flag((X).used, (Y)) && (!get_flag((X).deleted, (Y))))

#define BOUND(MIN,VAL,MAX) (((VAL) < (MIN)) ? (MIN) : (((VAL) > (MAX)) ? (MAX) : (VAL)))

#define SAFE_FREE(X) do { if ((X)) { free((X)); (X) = NULL; } } while(0)

/* ========================================================================= */
/* PRIVATE FUNCTIONS                                                         */
/* ========================================================================= */

static INLINE size_t hash(const uint64_t value, const size_t capacity)
{
	return (size_t) (((UINT64_C(14695981039346656037) + value) * UINT64_C(1099511628211)) % capacity);
}

static INLINE size_t safe_mult2(const size_t value)
{
	return (value < (SIZE_MAX >> 1)) ? (value << 1) : SIZE_MAX;
}

static INLINE size_t round_sz(double d)
{
	return (d >= 0.0) ? ((size_t)(d + 0.5)) : ((size_t)(d - ((double)((size_t)(d - 1))) + 0.5)) + ((size_t)(d - 1));
}

static INLINE size_t increment(const size_t value, const size_t bound)
{
	const size_t result = value + 1U;
	return (result >= bound) ? 0U : result;
}

static INLINE size_t next_pow2(const size_t minimum)
{
	size_t result = 2U;

	while (result < minimum)
	{
		result = safe_mult2(result);
	}

	return result;
}

static INLINE bool_t alloc_data(struct _hash_set_data *const data, const size_t capacity)
{
	memset(data, 0, sizeof(struct _hash_set_data));
	
	data->values = (uint64_t*) calloc(capacity, sizeof(uint64_t));
	if (!data->values)
	{
		return FALSE;
	}

	data->used = (uint8_t*) calloc((capacity / 8U) + ((capacity % 8U != 0U) ? 1U : 0U), sizeof(uint8_t));
	if (!data->used)
	{
		SAFE_FREE(data->values);
		return FALSE;
	}

	data->deleted = (uint8_t*) calloc((capacity / 8U) + ((capacity % 8U != 0U) ? 1U : 0U), sizeof(uint8_t));
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

static INLINE bool_t get_flag(const uint8_t *const flags, const size_t index)
{
	return (flags[index / 8U] >> (index % 8U)) & 1U;
}

static INLINE void set_flag(uint8_t *const flags, const size_t index)
{
	flags[index / 8U] |= UINT8_C(1) << (index % 8U);
}

static INLINE void clear_flag(uint8_t *const flags, const size_t index)
{
	flags[index / 8U] &= ~(UINT8_C(1) << (index % 8U));
}

static INLINE bool_t find_slot(const struct _hash_set_data *const data, const uint64_t value, size_t *const index_out)
{
	size_t index;
	bool_t index_saved = FALSE;

	for (index = hash(value, data->capacity); get_flag(data->used, index); index = increment(index, data->capacity))
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

	set_flag(data->used, index);
	return TRUE;
}

static INLINE errno_t grow_set(hash_set_t *const instance, const size_t new_capacity)
{
	struct _hash_set_data temp;
	size_t index, k;

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
				return EFAULT;
			}
			if (!store_value(&temp, index, value))
			{
				free_data(&temp);
				return EFAULT;
			}
		}
	}

	free_data(&instance->data);
	instance->data = temp;
	instance->limit = round_sz(instance->data.capacity * instance->load_factor);
	instance->total = instance->valid;

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

	if (!alloc_data(&instance->data, (initial_capacity > 0U) ? next_pow2(initial_capacity) : 1024U))
	{
		SAFE_FREE(instance);
		return NULL;
	}

	instance->load_factor = (load_factor > 0.0) ? BOUND(0.1, load_factor, 1.0) : 0.75;
	instance->options = options;
	instance->limit = round_sz(instance->data.capacity * instance->load_factor);

	return instance;
}

void hash_set_destroy(hash_set_t *instance)
{
	if (instance)
	{
		free_data(&instance->data);
		memset(instance, 0, sizeof(hash_set_t));
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

	while ((instance->total >= instance->limit) || (instance->valid >= instance->data.capacity))
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
			const errno_t error = grow_set(instance, safe_mult2(instance->data.capacity));
			if (error)
			{
				instance->limit = instance->data.capacity;
				if (error == ENOMEM)
				{
					if ((instance->options & HASHSET_OPT_FAILFAST) || (instance->valid >= instance->data.capacity))
					{
						return ENOMEM; /*malloc has failed!*/
					}
				}
				else
				{
					return error;
				}
			}
			else
			{
				if (find_slot(&instance->data, value, &index))
				{
					return EFAULT;
				}
			}
		}
	}

	if (store_value(&instance->data, index, value))
	{
		++instance->total;
	}
	
	++instance->valid;
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

errno_t hash_set_remove(hash_set_t* const instance, const uint64_t value)
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
	--instance->valid;

	return 0;
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

size_t hash_set_size(hash_set_t *const instance)
{
	return instance ? instance->valid : 0U;
}
