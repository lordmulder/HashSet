/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#include "hash_set.h"

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#ifndef _WIN32
#  include <unistd.h>
#endif

#if defined(__GNUC__)
#  define INLINE __inline__
#elif defined(_MSC_VER)
#  define INLINE __inline
#else
#  define INLINE
#endif

/* ========================================================================= */
/* Utilities                                                                 */
/* ========================================================================= */

#define ARRAY_SIZE(X) (sizeof(X) / sizeof((X)[0U]))

#ifdef _WIN32
#define RtlGenRandom SystemFunction036
#define getentropy(X,Y) (RtlGenRandom((X),(uint32_t)(Y)) ? 0 : (-1))
unsigned char __stdcall RtlGenRandom(void *buffer, uint32_t length);
#endif

typedef struct
{
	size_t offset;
	uint64_t buffer[16U];
}
random_t;

static INLINE void random_init(random_t *const rnd)
{
	memset(rnd, 0, sizeof(random_t));
	rnd->offset = SIZE_MAX;
}

static INLINE uint64_t random_next(random_t *const rnd)
{
	if (rnd->offset >= ARRAY_SIZE(rnd->buffer))
	{
		rnd->offset = 0U;
		if (getentropy(rnd->buffer, sizeof(rnd->buffer)) < 0)
		{
			abort();
		}
	}

	return rnd->buffer[rnd->offset++];
}

#define PRINT_SET_INFO() do \
{\
	if (!hash_set_info(hash_set, &capacity, &valid, &deleted, &limit)) \
	{ \
		printf("capacity: %010zu, valid: %010zu, deleted: %010zu, limit: %010zu\n", capacity, valid, deleted, limit); \
	} \
} \
while(0)

/* ========================================================================= */
/* TEST #1                                                                   */
/* ========================================================================= */

#define MAXIMUM 425984U

static int test_function_1(hash_set_t *const hash_set)
{
	size_t capacity, valid, deleted, limit;

	for (size_t r = 0U; r < 5U; ++r)
	{
		for (uint64_t i = 0; i < MAXIMUM; ++i)
		{
			if ((i != 3167U) && (i != 9887U) && (i != 185903U) && (i != 387083U))
			{
				const errno_t error = hash_set_insert(hash_set, i);
				if (error)
				{
					printf("Insert operation has failed! (error: %d)\n", error);
					return EXIT_FAILURE;
				}
			}
			PRINT_SET_INFO();
		}

		if (hash_set_size(hash_set) != MAXIMUM - 4U)
		{
			puts("Invalid size!");
			return EXIT_FAILURE;
		}

		for (uint64_t i = 0; i < MAXIMUM; ++i)
		{
			if ((i != 3167U) && (i != 9887U) && (i != 387083U))
			{
				const errno_t error = hash_set_insert(hash_set, i);
				if (error != ((i != 185903U) ? EEXIST : 0))
				{
					printf("Insert operation has failed! (error: %d)\n", error);
					return EXIT_FAILURE;
				}
			}
		}

		if (hash_set_size(hash_set) != MAXIMUM - 3U)
		{
			puts("Invalid size!");
			return EXIT_FAILURE;
		}

		for (uint64_t i = 0; i < MAXIMUM; ++i)
		{
			const errno_t error = hash_set_contains(hash_set, i);
			if (error != ((i != 3167U) && (i != 9887U) && (i != 387083U)) ? 0 : ENOENT)
			{
				printf("Contains operation has failed! (error: %d)\n", error);
				return EXIT_FAILURE;
			}
		}

		for (uint64_t i = 0; i < MAXIMUM; ++i)
		{
			if ((i != 3167U) && (i != 9887U) && (i != 216263U) && (i != 387083U))
			{
				const errno_t error = hash_set_remove(hash_set, i);
				if (error)
				{
					printf("Remove operation has failed! (error: %d)\n", error);
					return EXIT_FAILURE;
				}
			}
			PRINT_SET_INFO();
		}

		if (hash_set_size(hash_set) != 1U)
		{
			puts("Invalid size!");
			return EXIT_FAILURE;
		}

		for (uint64_t i = 0; i < MAXIMUM; ++i)
		{
			const errno_t error = hash_set_contains(hash_set, i);
			if (error != ((i != 216263U) ? ENOENT : 0))
			{
				printf("Contains operation has failed! (error: %d)\n", error);
				return EXIT_FAILURE;
			}
		}

		if (!hash_set_remove(hash_set, 9887U))
		{
			puts("Final remove operation has failed!");
			return EXIT_FAILURE;
		}

		if (hash_set_remove(hash_set, 216263U))
		{
			puts("Final remove operation has failed!");
			return EXIT_FAILURE;
		}

		if (hash_set_size(hash_set) != 0U)
		{
			puts("Invalid size!");
			return EXIT_FAILURE;
		}

		PRINT_SET_INFO();
		puts("--------");
	}

	return EXIT_SUCCESS;
}

/* ========================================================================= */
/* TEST #2                                                                   */
/* ========================================================================= */

static int test_function_2(hash_set_t *const hash_set)
{
	size_t capacity, valid, deleted, limit;
	uint8_t spinner = 0U;
	clock_t last_update = clock();

	random_t random;
	random_init(&random);

	for (;;)
	{
		const uint64_t rnd = random_next(&random) & UINT64_C(0x3FFFFFFFFFFFFFF);
		const errno_t error = hash_set_insert(hash_set, rnd);
		if (error)
		{
			if (error != EEXIST)
			{
				printf("Insert operation has failed! (error: %d)\n", error);
				return EXIT_FAILURE;
			}
			else
			{
				PRINT_SET_INFO();
				printf("Collision detected! [%016llX]\n", rnd);
				break;
			}
		}
		if (!(++spinner & 0x7F))
		{
			const clock_t clock_now = clock();
			if ((clock_now < last_update) || (clock_now >= last_update + CLOCKS_PER_SEC))
			{
				PRINT_SET_INFO();
				last_update = clock_now;
			}
		}
	}

	PRINT_SET_INFO();

	if (hash_set_clear(hash_set))
	{
		puts("Clear operation has failed!");
		return EXIT_FAILURE;
	}
	
	PRINT_SET_INFO();
	puts("--------");

	return EXIT_SUCCESS;
}

/* ========================================================================= */
/* TEST #3                                                                   */
/* ========================================================================= */

static int test_function_3(hash_set_t *const hash_set)
{
	size_t capacity, valid, deleted, limit;
	uint8_t spinner = 0U;
	clock_t last_update = clock();

	for (uint64_t value = 0U; value < ((uint64_t)INT32_MAX); ++value)
	{
		const errno_t error = hash_set_insert(hash_set, value);
		if (error)
		{
			PRINT_SET_INFO();
			printf("Insert operation has failed! (error: %d)\n", error);
			return EXIT_FAILURE;
		}
		if (!(++spinner & 0x7F))
		{
			const clock_t clock_now = clock();
			if ((clock_now < last_update) || (clock_now >= last_update + CLOCKS_PER_SEC))
			{
				PRINT_SET_INFO();
				last_update = clock_now;
			}
		}
	}

	for (uint64_t value = 0U; value < ((uint64_t)INT32_MAX); ++value)
	{
		const errno_t error = hash_set_remove(hash_set, value);
		if (error)
		{
			PRINT_SET_INFO();
			printf("Remove operation has failed! (error: %d)\n", error);
			return EXIT_FAILURE;
		}
		if (!(++spinner & 0x7F))
		{
			const clock_t clock_now = clock();
			if ((clock_now < last_update) || (clock_now >= last_update + CLOCKS_PER_SEC))
			{
				PRINT_SET_INFO();
				last_update = clock_now;
			}
		}
	}

	PRINT_SET_INFO();
	
	if (hash_set_size(hash_set) != 0U)
	{
		puts("Invalid size!");
		return EXIT_FAILURE;
	}

	puts("--------");

	return EXIT_SUCCESS;
}

/* ========================================================================= */
/* MAIN                                                                      */
/* ========================================================================= */

int main(void)
{
	hash_set_t *const hash_set = hash_set_create(0U, -1.0);
	if (!hash_set)
	{
		puts("Allocation has failed!");
		return EXIT_FAILURE;
	}

	if (test_function_1(hash_set) != EXIT_SUCCESS)
	{
		goto failure;
	}

	if (test_function_2(hash_set) != EXIT_SUCCESS)
	{
		goto failure;
	}
	
	if (test_function_3(hash_set) != EXIT_SUCCESS)
	{
		goto failure;
	}

	hash_set_destroy(hash_set);
	puts("Test completed successfully.");
	return EXIT_SUCCESS;

failure:
	hash_set_destroy(hash_set);
	puts("Something went wrong !!!");
	return EXIT_FAILURE;
}
