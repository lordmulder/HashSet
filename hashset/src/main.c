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

int main()
{
	uint8_t spinner = 0U;
	clock_t last_update = clock();

	hash_set_t *const hash_set = hash_set_create(0U, -1.0, HASHSET_OPT_FAILFAST);
	if (!hash_set)
	{
		puts("Allocation has failed!");
		return EXIT_FAILURE;
	}

	for (;;)
	{
		const errno_t error = hash_set_insert(hash_set, next_rand() & 0x3FFFFFFFFFFFFFFllu);
		if (error)
		{
			printf("Insert operation has failed! (error: %d)\n", error);
			break;
		}
		if (!(++spinner & 0x7F))
		{
			const clock_t now = clock();
			if ((now < last_update) || (now >= last_update + CLOCKS_PER_SEC))
			{
				size_t capacity, valid, deleted, limit;
				if (!hash_set_info(hash_set, &capacity, &valid, &deleted, &limit))
				{
					printf("%010zu, %010zu, %010zu, %010zu\n", capacity, valid, deleted, limit);
				}
				else
				{
					printf("%zu\n", hash_set_size(hash_set));
				}
				last_update = now;
			}
		}
	}

	hash_set_destroy(hash_set);
	return EXIT_SUCCESS;
}
