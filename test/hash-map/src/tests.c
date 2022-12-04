/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#include "tests.h"
#include <random_in.h>

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
	if (!hash_map_info64(hash_map, &capacity, &valid, &deleted, &limit)) \
	{ \
		fprintf(stdout, "[MAP %d/%d] capacity: %010zu, valid: %010zu, deleted: %010zu, limit: %010zu\n", (X), TEST_COUNT, capacity, valid, deleted, limit); \
		fflush(stdout); \
	} \
} \
while(0)

/* ========================================================================= */
/* TEST #1                                                                   */
/* ========================================================================= */

#define TEST_SIZE 499979U

int test_function_1(hash_map64_t *const hash_map)
{
	size_t r, j, cursor, capacity, valid, deleted, limit;
	uint64_t key, value, *test_data;
	uint8_t spinner = 0U, *test_key1, *test_key2;

	random_t random;
	random_init(&random);

	test_key1 = (uint8_t*) malloc(TEST_SIZE * sizeof(uint8_t));
	if (!test_key1)
	{
		abort(); /*malloc has failed!*/
	}

	test_key2 = (uint8_t*) malloc(TEST_SIZE * sizeof(uint8_t));
	if (!test_key2)
	{
		abort(); /*malloc has failed!*/
	}

	test_data = (uint64_t*) malloc(TEST_SIZE * sizeof(uint64_t));
	if (!test_data)
	{
		abort(); /*malloc has failed!*/
	}

	for (r = 0U; r < 64U; ++r)
	{
		memset(test_key1, 0, TEST_SIZE * sizeof(uint8_t));
		memset(test_key2, 0, TEST_SIZE * sizeof(uint8_t));
		memset(test_data, 0, TEST_SIZE * sizeof(uint64_t));

		for (j = 0U; j < TEST_SIZE / 3U; ++j)
		{
			size_t rnd;
			do
			{
				rnd = random_next(&random) % TEST_SIZE;
			}
			while (test_key1[rnd]);
			INVERT(test_key1[rnd]);
		}

		for (j = 0U; j < TEST_SIZE; ++j)
		{
			if (test_key1[j])
			{
				const errno_t error = hash_map_insert64(hash_map, j, test_data[j] = random_next(&random), 1);
				if (error)
				{
					printf("Insert operation has failed! (error: %d)\n", error);
					return EXIT_FAILURE;
				}
				if (!(++spinner & 0x0F))
				{
					PRINT_SET_INFO(1);
				}
			}
		}

		for (j = 0U; j < TEST_SIZE; ++j)
		{
			const errno_t error = hash_map_contains64(hash_map, j);
			if (error != (test_key1[j] ? 0 : ENOENT))
			{
				printf("Contains operation has failed! (error: %d)\n", error);
				return EXIT_FAILURE;
			}
		}

		cursor = 0U;
		while (!hash_map_iterate64(hash_map, &cursor, &key, &value))
		{
			if ((!test_key1[key]) || test_key2[key])
			{
				puts("Iteration error has been detected!");
				return EXIT_FAILURE;
			}
			INVERT(test_key2[key]);
		}

		for (j = 0U; j < TEST_SIZE; ++j)
		{
			if (test_key1[j] != test_key2[j])
			{
				puts("Iteration error has been detected!");
				return EXIT_FAILURE;
			}
		}

		for (j = 0U; j < TEST_SIZE; ++j)
		{
			if (test_key1[j])
			{
				if (!hash_map_get64(hash_map, j, &value))
				{
					if (value != test_data[j])
					{
						puts("Value mismatch has been detected!");
						return EXIT_FAILURE;
					}
				}
				else
				{
					puts("Failed to retrieve the value!");
					return EXIT_FAILURE;
				}
			}
		}

		for (j = 0U; j < TEST_SIZE; ++j)
		{
			if (test_key1[j])
			{
				const errno_t error = hash_map_remove64(hash_map, j, NULL);
				if (error)
				{
					printf("Remove operation has failed! (error: %d)\n", error);
					return EXIT_FAILURE;
				}
				if (!(++spinner & 0x0F))
				{
					PRINT_SET_INFO(1);
				}
			}
		}

		for (j = 0U; j < TEST_SIZE; ++j)
		{
			const errno_t error = hash_map_contains64(hash_map, j);
			if (error != ENOENT)
			{
				printf("Contains operation has failed! (error: %d)\n", error);
				return EXIT_FAILURE;
			}
		}

		if (hash_map_size64(hash_map) != 0U)
		{
			puts("Invalid size!");
			return EXIT_FAILURE;
		}
	}

	free(test_key1);
	free(test_key2);
	free(test_data);

	PRINT_SET_INFO(1);
	puts("---------");

	return EXIT_SUCCESS;
}

/* ========================================================================= */
/* TEST #2                                                                   */
/* ========================================================================= */

int test_function_2(hash_map64_t *const hash_map)
{
	size_t r, j, capacity, valid, deleted, limit;
	uint64_t value, *test_val;
	uint8_t spinner = 0U, *test_key;

	random_t random;
	random_init(&random);

	test_key = (uint8_t*) malloc(TEST_SIZE * sizeof(uint8_t));
	if (!test_key)
	{
		abort(); /*malloc has failed!*/
	}

	test_val = (uint64_t*) malloc(TEST_SIZE * sizeof(uint64_t));
	if (!test_val)
	{
		abort(); /*malloc has failed!*/
	}

	for (r = 0U; r < 64U; ++r)
	{
		memset(test_key, 0, TEST_SIZE * sizeof(uint8_t));
		memset(test_val, 0, TEST_SIZE * sizeof(uint64_t));

		for (j = 0U; j < TEST_SIZE / 3U; ++j)
		{
			size_t rnd;
			do
			{
				rnd = random_next(&random) % TEST_SIZE;
			}
			while (test_key[rnd]);
			INVERT(test_key[rnd]);
		}

		for (j = 0U; j < TEST_SIZE; ++j)
		{
			if (test_key[j])
			{
				const errno_t error = hash_map_insert64(hash_map, j, test_val[j] = random_next(&random), 1);
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

		for (j = 0U; j < TEST_SIZE; ++j)
		{
			if (test_key[j])
			{
				const errno_t error = hash_map_insert64(hash_map, j, test_val[j] = random_next(&random), 1);
				if (error != EEXIST)
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

		for (j = 0U; j < TEST_SIZE; ++j)
		{
			if (test_key[j])
			{
				if (!hash_map_get64(hash_map, j, &value))
				{
					if (value != test_val[j])
					{
						puts("Value mismatch has been detected!");
						return EXIT_FAILURE;
					}
				}
				else
				{
					puts("Failed to retrieve the value!");
					return EXIT_FAILURE;
				}
			}
		}

		if (hash_map_clear64(hash_map))
		{
			puts("Failed to clear the map!");
			return EXIT_FAILURE;
		}
	}

	free(test_key);
	free(test_val);

	PRINT_SET_INFO(2);
	puts("---------");

	return EXIT_SUCCESS;
}

/* ========================================================================= */
/* TEST #3                                                                   */
/* ========================================================================= */

#define VALUE(X) (((X) ^ UINT64_C(0xB7E151628AED2A6A)) * UINT64_C(65599))

int test_function_3(hash_map64_t *const hash_map)
{
	size_t r, cursor, capacity, valid, deleted, limit;
	uint8_t spinner = 0U;
	clock_t last_update = clock();
	uint64_t key, value;

	random_t random;
	random_init(&random);

	for (r = 0U; r < 3U; ++r)
	{
		for (;;)
		{
			const uint64_t rnd = random_next(&random) & UINT64_C(0x1FFFFFFFFFFFFFF);
			const errno_t error = hash_map_insert64(hash_map, rnd, VALUE(rnd), 1);
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
					printf("Collision detected! [0x%016" PRIX64 "]\n", rnd);
					cursor = 0U;
					while (!hash_map_iterate64(hash_map, &cursor, &key, &value))
					{
						if (value != VALUE(key))
						{
							puts("Value mismatch has been detected!");
							return EXIT_FAILURE;
						}
					}
					puts("Value verification successful.");
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

		if (hash_map_clear64(hash_map))
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

#define LIMIT (((uint64_t)UINT32_MAX) >> 3)

int test_function_4(hash_map64_t *const hash_map)
{
	size_t capacity, valid, deleted, limit;
	uint64_t key;
	uint8_t spinner = 0U;
	clock_t last_update = clock();

	for (key = 0U; key < LIMIT; ++key)
	{
		const errno_t error = hash_map_insert64(hash_map, key, VALUE(key), 1);
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

	for (key = 0U; key < LIMIT; ++key)
	{
		const errno_t error = hash_map_remove64(hash_map, key, NULL);
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

	if (hash_map_size64(hash_map) != 0U)
	{
		puts("Invalid size!");
		return EXIT_FAILURE;
	}

	PRINT_SET_INFO(4);
	puts("---------");

	return EXIT_SUCCESS;
}
