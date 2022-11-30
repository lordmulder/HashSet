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
	if (!hash_map_info64(hash_set, &capacity, &valid, &deleted, &limit)) \
	{ \
		fprintf(stdout, "[Test %d/%d] capacity: %010zu, valid: %010zu, deleted: %010zu, limit: %010zu\n", (X), TEST_COUNT, capacity, valid, deleted, limit); \
		fflush(stdout); \
	} \
} \
while(0)

/* ========================================================================= */
/* TEST #1                                                                   */
/* ========================================================================= */

#define TEST_SIZE 499979U

int test_function_1(hash_map64_t *const hash_set)
{
	size_t r, j, cursor, capacity, valid, deleted, limit;
	uint64_t key, value;
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
				const errno_t error = hash_map_insert64(hash_set, j, 0U);
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
		while (!hash_map_iterate64(hash_set, &cursor, &key, &value))
		{
			if ((!test1[key]) || test2[key])
			{
				puts("Iteration error has been detected!");
				return EXIT_FAILURE;
			}
			INVERT(test2[key]);
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
				const errno_t error = hash_map_remove64(hash_set, j);
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

		if (hash_map_size64(hash_set) != 0U)
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
