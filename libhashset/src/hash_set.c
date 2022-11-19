/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#include "hash_set.h"

/* CRT */
#include <string.h>
#include <errno.h>
#include <stdbool.h>

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
	uint8_t *used;
	size_t capacity;
};

struct _hash_set
{
	double load_factor;
	int fail_fast;
	size_t size, limit;
	struct _hash_set_data data;
};

#define BOUND(MIN,VAL,MAX) (((VAL) < (MIN)) ? (MIN) : (((VAL) > (MAX)) ? (MAX) : (VAL)))

#define BOOLIFY(X) (!!(X))

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

static INLINE size_t round(double d)
{
	return (d >= 0.0) ? ((size_t)(d + 0.5)) : ((size_t)(d - ((double)((size_t)(d - 1))) + 0.5)) + ((size_t)(d - 1));
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

static INLINE bool alloc_data(struct _hash_set_data *const data, const size_t capacity)
{
	memset(data, 0, sizeof(struct _hash_set_data));
	
	data->values = (uint64_t*) calloc(capacity, sizeof(uint64_t));
	if (!data->values)
	{
		return false;
	}

	data->used = (uint8_t*) calloc((capacity / 8U) + ((capacity % 8U != 0U) ? 1U : 0U), sizeof(uint8_t));
	if (!data->used)
	{
		free(data->values);
		data->values = NULL;
		return false;
	}

	data->capacity = capacity;
	return true;
}

static INLINE void free_data(struct _hash_set_data *const data)
{
	if (data)
	{
		data->capacity = 0U;
		if (data->values)
		{
			free(data->values);
			data->values = NULL;
		}
		if (data->used)
		{
			free(data->used);
			data->used = NULL;
		}
	}
}

static INLINE bool is_used(struct _hash_set_data *const data, const size_t index)
{
	return (data->used[index / 8U] >> (index % 8U)) & 1U;
}

static INLINE bool find_slot(struct _hash_set_data* const data, const uint64_t value, size_t *const index_out)
{
	size_t index = hash(value, data->capacity);

	while (is_used(data, index))
	{
		if (data->values[index] == value)
		{
			if (index_out)
			{
				*index_out = index;
			}
			return true;
		}
		if (++index >= data->capacity)
		{
			index = 0U;
		}
	}

	if (index_out)
	{
		*index_out = index;
	}

	return false;
}

static INLINE bool insert_value(struct _hash_set_data *const data, const size_t index, const uint64_t value)
{
	if (is_used(data, index))
	{
		return false;
	}

	data->values[index] = value;
	data->used[index / 8U] |= UINT8_C(1) << (index % 8U);

	return true;
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
		if (is_used(&instance->data, k))
		{
			const uint64_t value = instance->data.values[k];
			if (find_slot(&temp, value, &index))
			{
				free_data(&temp);
				return EFAULT;
			}
			if (!insert_value(&temp, index, value))
			{
				free_data(&temp);
				return EFAULT;
			}
		}
	}

	free_data(&instance->data);
	instance->data = temp;
	instance->limit = round(instance->data.capacity * instance->load_factor);

	return 0;
}

/* ========================================================================= */
/* PUBLIC FUNCTIONS                                                          */
/* ========================================================================= */

hash_set_t *hash_set_create(const double load_factor, const int fail_fast, const size_t initial_capacity)
{
	hash_set_t *const instance = (hash_set_t*) calloc(1U, sizeof(hash_set_t));
	if (!instance)
	{
		return NULL;
	}

	if (!alloc_data(&instance->data, (initial_capacity > 0U) ? next_pow2(initial_capacity) : 1024U))
	{
		free(instance);
		return NULL;
	}

	instance->load_factor = (load_factor > 0.0) ? BOUND(0.001953125, load_factor, 1.0) : 0.666;
	instance->fail_fast = BOOLIFY(fail_fast);
	instance->limit = round(instance->data.capacity * instance->load_factor);

	return instance;
}

void hash_set_destroy(hash_set_t *const instance)
{
	if (instance)
	{
		free_data(&instance->data);
		memset(instance, 0, sizeof(hash_set_t));
		free(instance);
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

	if ((instance->size >= instance->limit) || (instance->size >= instance->data.capacity))
	{
		if (instance->data.capacity == SIZE_MAX)
		{
			if (instance->fail_fast || (instance->size >= instance->data.capacity))
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
					if (instance->fail_fast || (instance->size >= instance->data.capacity))
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

	if (!insert_value(&instance->data, index, value))
	{
		return EFAULT;
	}

	++instance->size;
	return 0;
}

errno_t hash_set_contains(hash_set_t *const instance, const uint64_t value)
{
	if ((!instance) || (!instance->data.values))
	{
		return EINVAL;
	}

	return find_slot(&instance->data, value, NULL) ? 0 : ENOENT;
}

size_t hash_set_size(hash_set_t *const instance)
{
	return instance ? instance->size : 0U;
}
