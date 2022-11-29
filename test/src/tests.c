/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#include "tests.h"
#include "random.h"

#include <stdio.h>
#include <inttypes.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#define TEST_COUNT 4

/* ========================================================================= */
/* Utilities                                                                 */
/* ========================================================================= */

#define UNUSED(X) ((void)X)

#define INVERT(X) do { (X) = (!(X)); } while(0)

#define PRINT_SET_INFO(X) do \
{\
	if (!hash_set_info64(hash_set, &capacity, &valid, &deleted, &limit)) \
	{ \
		fprintf(stdout, "[Test %d/%d] capacity: %010zu, valid: %010zu, deleted: %010zu, limit: %010zu\n", (X), TEST_COUNT, capacity, valid, deleted, limit); \
		fflush(stdout); \
	} \
} \
while(0)

/* ========================================================================= */
/* TEST #1                                                                   */
/* ========================================================================= */

#define MAXIMUM 425984U

static int dump_callback(const size_t index, const char status, const uint64_t value)
{
#ifndef NDEBUG
	printf("%016zX: %c -> %016" PRIX64 "\n", index, status, value);
#else
	UNUSED(index); UNUSED(status); UNUSED(value);
#endif
	return 1;
}

int test_function_1(hash_set64_t *const hash_set)
{
	size_t r, capacity, valid, deleted, limit;
	uint64_t i;
	uint8_t spinner = 0U;

	for (r = 0U; r < 5U; ++r)
	{
		for (i = 0; i < MAXIMUM; ++i)
		{
			if ((i != 3167U) && (i != 9887U) && (i != 185903U) && (i != 387083U))
			{
				const errno_t error = hash_set_insert64(hash_set, i);
				if (error)
				{
					printf("Insert operation has failed! (error: %d)\n", error);
					return EXIT_FAILURE;
				}
			}
			if (!(++spinner & 0x0F))
			{
				PRINT_SET_INFO(1);
			}
		}

		if (hash_set_size64(hash_set) != MAXIMUM - 4U)
		{
			puts("Invalid size!");
			return EXIT_FAILURE;
		}

		for (i = 0; i < MAXIMUM; ++i)
		{
			if ((i != 3167U) && (i != 9887U) && (i != 387083U))
			{
				const errno_t error = hash_set_insert64(hash_set, i);
				if (error != ((i != 185903U) ? EEXIST : 0))
				{
					printf("Insert operation has failed! (error: %d)\n", error);
					return EXIT_FAILURE;
				}
			}
		}

		if (hash_set_size64(hash_set) != MAXIMUM - 3U)
		{
			puts("Invalid size!");
			return EXIT_FAILURE;
		}

		for (i = 0; i < MAXIMUM; ++i)
		{
			const errno_t error = hash_set_contains64(hash_set, i);
			if (error != ((i != 3167U) && (i != 9887U) && (i != 387083U)) ? 0 : ENOENT)
			{
				printf("Contains operation has failed! (error: %d)\n", error);
				return EXIT_FAILURE;
			}
		}

		if (hash_set_dump64(hash_set, dump_callback))
		{
			puts("Dump operation has failed!");
			return EXIT_FAILURE;
		}

		for (i = 0; i < MAXIMUM; ++i)
		{
			if ((i != 3167U) && (i != 9887U) && (i != 216263U) && (i != 387083U))
			{
				const errno_t error = hash_set_remove64(hash_set, i);
				if (error)
				{
					printf("Remove operation has failed! (error: %d)\n", error);
					return EXIT_FAILURE;
				}
			}
			if (!(++spinner & 0x0F))
			{
				PRINT_SET_INFO(1);
			}
		}

		if (hash_set_size64(hash_set) != 1U)
		{
			puts("Invalid size!");
			return EXIT_FAILURE;
		}

		for (i = 0; i < MAXIMUM; ++i)
		{
			const errno_t error = hash_set_contains64(hash_set, i);
			if (error != ((i != 216263U) ? ENOENT : 0))
			{
				printf("Contains operation has failed! (error: %d)\n", error);
				return EXIT_FAILURE;
			}
		}

		if (!hash_set_remove64(hash_set, 9887U))
		{
			puts("Final remove operation has failed!");
			return EXIT_FAILURE;
		}

		if (hash_set_remove64(hash_set, 216263U))
		{
			puts("Final remove operation has failed!");
			return EXIT_FAILURE;
		}

		if (hash_set_size64(hash_set) != 0U)
		{
			puts("Invalid size!");
			return EXIT_FAILURE;
		}

		PRINT_SET_INFO(1);
		puts("---------");
	}

	return EXIT_SUCCESS;
}

/* ========================================================================= */
/* TEST #2                                                                   */
/* ========================================================================= */

#define TEST_SIZE 499979U

int test_function_2(hash_set64_t *const hash_set)
{
	size_t r, j, cursor, capacity, valid, deleted, limit;
	uint64_t value;
	uint8_t spinner = 0U, *test1, *test2;

	random_t random;
	random_init(&random);

	test1 = (uint8_t*) malloc(TEST_SIZE * sizeof(uint8_t));
	if (!test1)
	{
		abort(); /*malloc has failed!*/
	}

	test2 = (uint8_t*) malloc(TEST_SIZE * sizeof(uint8_t));
	if (!test2)
	{
		abort(); /*malloc has failed!*/
	}

	for (r = 0U; r < 64U; ++r)
	{
		memset(test1, 0, TEST_SIZE * sizeof(uint8_t));
		memset(test2, 0, TEST_SIZE * sizeof(uint8_t));

		for (j = 0U; j < TEST_SIZE / 3U; ++j)
		{
			size_t rnd;
			do
			{
				rnd = random_next(&random) % TEST_SIZE;
			}
			while (test1[rnd]);
			INVERT(test1[rnd]);
		}

		for (j = 0U; j < TEST_SIZE; ++j)
		{
			if (test1[j])
			{
				const errno_t error = hash_set_insert64(hash_set, j);
				if (error)
				{
					printf("Insert operation has failed! (error: %d)\n", error);
					return EXIT_FAILURE;
				}
				if (!(++spinner & 0x0F))
				{
					PRINT_SET_INFO(2);
				}
			}
		}

		cursor = 0U;
		while (!hash_set_iterate64(hash_set, &cursor, &value))
		{
			if ((!test1[value]) || test2[value])
			{
				puts("Iteration error has been detected!");
				return EXIT_FAILURE;
			}
			INVERT(test2[value]);
		}

		for (j = 0U; j < TEST_SIZE; ++j)
		{
			if (test1[j] != test2[j])
			{
				puts("Iteration error has been detected!");
				return EXIT_FAILURE;
			}
		}

		for (j = 0U; j < TEST_SIZE; ++j)
		{
			if (test1[j])
			{
				const errno_t error = hash_set_remove64(hash_set, j);
				if (error)
				{
					printf("Remove operation has failed! (error: %d)\n", error);
					return EXIT_FAILURE;
				}
				if (!(++spinner & 0x0F))
				{
					PRINT_SET_INFO(2);
				}
			}
		}

		if (hash_set_size64(hash_set) != 0U)
		{
			puts("Invalid size!");
			return EXIT_FAILURE;
		}
	}

	free(test1);
	free(test2);

	PRINT_SET_INFO(2);
	puts("---------");

	return EXIT_SUCCESS;
}

/* ========================================================================= */
/* TEST #3                                                                   */
/* ========================================================================= */

int test_function_3(hash_set64_t *const hash_set)
{
	size_t r, capacity, valid, deleted, limit;
	uint8_t spinner = 0U;
	clock_t last_update = clock();

	random_t random;
	random_init(&random);

	for (r = 0U; r < 3U; ++r)
	{
		for (;;)
		{
			const uint64_t rnd = random_next(&random) & UINT64_C(0x3FFFFFFFFFFFFFF);
			const errno_t error = hash_set_insert64(hash_set, rnd);
			if (error)
			{
				if (error != EEXIST)
				{
					printf("Insert operation has failed! (error: %d)\n", error);
					return EXIT_FAILURE;
				}
				else
				{
					PRINT_SET_INFO(3);
					printf("Collision detected! [%016" PRIx64 "]\n", rnd);
					break;
				}
			}
			if (!(++spinner & 0x7F))
			{
				const clock_t clock_now = clock();
				if ((clock_now < last_update) || (clock_now >= last_update + CLOCKS_PER_SEC))
				{
					PRINT_SET_INFO(3);
					last_update = clock_now;
				}
			}
		}

		PRINT_SET_INFO(3);

		if (hash_set_clear64(hash_set))
		{
			puts("Clear operation has failed!");
			return EXIT_FAILURE;
		}
	}

	PRINT_SET_INFO(3);
	puts("---------");

	return EXIT_SUCCESS;
}

/* ========================================================================= */
/* TEST #4                                                                   */
/* ========================================================================= */

#define LIMIT (((uint64_t)UINT32_MAX) >> 2)

int test_function_4(hash_set64_t *const hash_set)
{
	size_t capacity, valid, deleted, limit;
	uint64_t value;
	uint8_t spinner = 0U;
	clock_t last_update = clock();

	for (value = 0U; value < LIMIT; ++value)
	{
		const errno_t error = hash_set_insert64(hash_set, value);
		if (error)
		{
			PRINT_SET_INFO(4);
			printf("Insert operation has failed! (error: %d)\n", error);
			return EXIT_FAILURE;
		}
		if (!(++spinner & 0x7F))
		{
			const clock_t clock_now = clock();
			if ((clock_now < last_update) || (clock_now >= last_update + CLOCKS_PER_SEC))
			{
				PRINT_SET_INFO(4);
				last_update = clock_now;
			}
		}
	}

	for (value = 0U; value < LIMIT; ++value)
	{
		const errno_t error = hash_set_remove64(hash_set, value);
		if (error)
		{
			PRINT_SET_INFO(4);
			printf("Remove operation has failed! (error: %d)\n", error);
			return EXIT_FAILURE;
		}
		if (!(++spinner & 0x7F))
		{
			const clock_t clock_now = clock();
			if ((clock_now < last_update) || (clock_now >= last_update + CLOCKS_PER_SEC))
			{
				PRINT_SET_INFO(4);
				last_update = clock_now;
			}
		}
	}

	if (hash_set_size64(hash_set) != 0U)
	{
		puts("Invalid size!");
		return EXIT_FAILURE;
	}

	PRINT_SET_INFO(4);
	puts("---------");

	return EXIT_SUCCESS;
}
