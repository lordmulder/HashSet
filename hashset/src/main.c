/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#define _CRT_RAND_S 1

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "hash_set.h"

static uint64_t next_rand(void)
{
	uint32_t a, b;
	if (rand_s(&a) || rand_s(&b))
	{
		abort();
	}
	return (((uint64_t)a) << 32) | b;
}
#define MAXIMUM 393216U

int main()
{
	uint64_t rnd;
	size_t capacity, valid, deleted, limit;
	uint8_t spinner;
	clock_t last_update;

	hash_set_t *const hash_set = hash_set_create(0U, -1.0, HASHSET_OPT_FAILFAST);
	if (!hash_set)
	{
		puts("Allocation has failed!");
		return EXIT_FAILURE;
	}

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
			if (!hash_set_info(hash_set, &capacity, &valid, &deleted, &limit))
			{
				printf("%010zu, %010zu, %010zu, %010zu\n", capacity, valid, deleted, limit);
			}
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
			if (!hash_set_info(hash_set, &capacity, &valid, &deleted, &limit))
			{
				printf("%010zu, %010zu, %010zu, %010zu\n", capacity, valid, deleted, limit);
			}
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

		if (!hash_set_info(hash_set, &capacity, &valid, &deleted, &limit))
		{
			printf("%010zu, %010zu, %010zu, %010zu\n", capacity, valid, deleted, limit);
		}

		puts("-----");
	}

	spinner = 0U;
	last_update = clock();

	for (;;)
	{
		const errno_t error = hash_set_insert(hash_set, rnd = next_rand() & 0x3FFFFFFFFFFFFFFllu);
		if (error)
		{
			if (error != EEXIST)
			{
				printf("Insert operation has failed! (error: %d)\n", error);
				return EXIT_FAILURE;
			}
			else
			{
				printf("Collision detected! [%016llX]\n", rnd);
				break;
			}
		}
		if (!(++spinner & 0x7F))
		{
			const clock_t now = clock();
			if ((now < last_update) || (now >= last_update + CLOCKS_PER_SEC))
			{
				if (!hash_set_info(hash_set, &capacity, &valid, &deleted, &limit))
				{
					printf("%010zu, %010zu, %010zu, %010zu\n", capacity, valid, deleted, limit);
				}
				last_update = now;
			}
		}
	}

	if (!hash_set_info(hash_set, &capacity, &valid, &deleted, &limit))
	{
		printf("%010zu, %010zu, %010zu, %010zu\n", capacity, valid, deleted, limit);
	}

	hash_set_destroy(hash_set);
	puts("Test completed successfully.");

	return EXIT_SUCCESS;
}
