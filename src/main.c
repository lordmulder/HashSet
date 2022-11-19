#define _CRT_RAND_S 1

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
	hash_set_t *const set = hash_set_create(-1.0, 0, 0U);
	if (!set)
	{
		puts("Allocation has failed!");
		return EXIT_FAILURE;
	}

	clock_t now, next_update = clock();
	for (;;)
	{
		const int ret = hash_set_insert(set, next_rand() & 0xFFFFFFFFFFFFFFllu);
		if (ret != 1)
		{
			printf("Error! (%d)\n", ret);
			break;
		}
		if ((now = clock()) >= next_update)
		{
			printf("%zu\n", hash_set_current_size(set));
			next_update = now + (3U * CLOCKS_PER_SEC);
		}
	}

	hash_set_destroy(set);
	return EXIT_SUCCESS;
}
