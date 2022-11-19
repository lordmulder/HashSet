/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#include "hash_set.h"

/* CRT */
#include <string.h>

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
	struct _hash_set_data data;
	size_t size, limit;
};

#define BOUND(MIN,VAL,MAX) (((VAL) < (MIN)) ? (MIN) : (((VAL) > (MAX)) ? (MAX) : (VAL)))

#define BOOLIFY(X) (!!(X))

/* ========================================================================= */
/* PRIVATE FUNCTIONS                                                         */
/* ========================================================================= */

static INLINE size_t hash(uint64_t value, const size_t capacity)
{
#if SIZE_MAX > UINT32_MAX
	uint64_t h = UINT64_C(14695981039346656037);
	for (size_t i = 0U; i < sizeof(uint64_t); ++i, value >>= CHAR_BIT)
	{
		h ^= value & 0xFF;
		h *= UINT64_C(1099511628211);
	}
#else
	uint32_t h = UINT32_C(2166136261);
	for (size_t i = 0U; i < sizeof(uint64_t); ++i, value >>= CHAR_BIT)
	{
		h ^= value & 0xFF;
		h *= UINT32_C(16777619);
	}
#endif
	return ((size_t) h) % capacity;
}

static INLINE size_t round(double d)
{
	return (d >= 0.0) ? ((size_t)(d + 0.5)) : ((size_t)(d - ((double)((size_t)(d - 1))) + 0.5)) + ((size_t)(d - 1));
}

static INLINE size_t safe_mult(const size_t a, const size_t b)
{
	if ((a == 0U) || (b == 0U))
	{
		return 0U;
	}

	const size_t result = a * b;
	return(result / a != b) ? SIZE_MAX : result;
}

static INLINE size_t next_pow2(const size_t minimum)
{
	size_t result = 2U;
	while (result < minimum)
	{
		result = safe_mult(result, 2U);
	}
	return result;
}

static INLINE int alloc_data(struct _hash_set_data *const data, const size_t capacity)
{
	memset(data, 0, sizeof(struct _hash_set_data));
	
	data->values = (uint64_t*) calloc(capacity, sizeof(uint64_t));
	if (!data->values)
	{
		return 0;
	}

	data->used = (uint8_t*) calloc((capacity + 7U) / 8U, sizeof(uint8_t));
	if (!data->used)
	{
		free(data->values);
		data->values = NULL;
		return 0;
	}

	data->capacity = capacity;
	return 1;
}

static INLINE void free_data(struct _hash_set_data *const data)
{
	if (data)
	{
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

		data->capacity = 0U;
	}
}

static INLINE int is_used(struct _hash_set_data *const data, const size_t index)
{
	return (data->used[index / 8U] >> (index % 8U)) & 1U;
}

static INLINE int find_value(struct _hash_set_data* const data, const uint64_t value, size_t *const index_out)
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
			return 1;
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

	return 0;
}

static INLINE void insert_value(struct _hash_set_data *const data, const size_t index, const uint64_t value)
{
	data->values[index] = value;
	data->used[index / 8U] |= 1U << (index % 8U);
}

static INLINE void swap_data(struct _hash_set_data *const a, struct _hash_set_data *const b)
{
	struct _hash_set_data temp = *a;
	*a = *b;
	*b = temp;
}

static INLINE int grow_set(hash_set_t *const instance, const size_t new_capacity)
{
	struct _hash_set_data temp;
	if (!alloc_data(&temp, new_capacity))
	{
		return 0;
	}

	for (size_t i = 0; i < instance->data.capacity; ++i)
	{
		if (is_used(&instance->data, i))
		{
			const uint64_t value = instance->data.values[i];
			size_t index;
			if (find_value(&temp, value, &index))
			{
				abort(); /*whoops!*/
			}
			insert_value(&temp, index, value);
		}
	}

	swap_data(&instance->data, &temp);
	free_data(&temp);
	instance->limit = round(instance->data.capacity * instance->load_factor);
	return 1;
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

	instance->load_factor = (load_factor > 0.0) ? BOUND(0.001953125, load_factor, 1.0) : 0.75;
	instance->fail_fast = BOOLIFY(fail_fast);
	instance->limit = round(instance->data.capacity * instance->load_factor);
	return instance;
}

void hash_set_destroy(hash_set_t *const instance)
{
	if (instance)
	{
		free_data(&instance->data);
		instance->size = 0U;
		instance->limit = 0U;
	}
}

int hash_set_insert(hash_set_t *const instance, const uint64_t value)
{
	if ((!instance) || (!instance->data.values))
	{
		return -1;
	}

	size_t index;
	if (find_value(&instance->data, value, &index))
	{
		return 0;
	}

	if ((instance->size >= instance->limit) || (instance->size >= instance->data.capacity))
	{
		if (instance->data.capacity == SIZE_MAX)
		{
			if (instance->fail_fast || (instance->size >= instance->data.capacity))
			{
				return -1; /*malloc has failed!*/
			}
		}
		else
		{
			if (grow_set(instance, safe_mult(instance->data.capacity, 2U)))
			{
				if (find_value(&instance->data, value, &index))
				{
					abort(); /*whoops!*/
				}
			}
			else
			{
				if (instance->fail_fast || (instance->size >= instance->data.capacity))
				{
					return -1; /*malloc has failed!*/
				}
				instance->limit = instance->data.capacity;
			}
		}
	}

	insert_value(&instance->data, index, value);
	++instance->size;
	return 1;
}

int hash_set_contains(hash_set_t *const instance, const uint64_t value)
{
	if ((!instance) || (!instance->data.values))
	{
		return -1;
	}

	return find_value(&instance->data, value, NULL);
}

size_t hash_set_current_size(hash_set_t *const instance)
{
	return instance ? instance->size : 0U;
}
