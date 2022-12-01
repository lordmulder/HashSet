/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#include "tests.h"
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#define RUN_TEST_CASE(X) do \
{ \
	if (test_function_##X(hash_set) != EXIT_SUCCESS) \
	{ \
		goto failure; \
	} \
} \
while(0)

#define SEED ((uint32_t)time(NULL))

/* ========================================================================= */
/* MAIN                                                                      */
/* ========================================================================= */

int main(void)
{
	hash_map64_t *hash_set;

	printf("LibHashSet Hash-Map Test v%" PRIu16 ".%" PRIu16 ".%" PRIu16 " [%s]\n\n",
		HASHSET_VERSION_MAJOR, HASHSET_VERSION_MINOR, HASHSET_VERSION_PATCH, HASHSET_BUILD_DATE);

	hash_set = hash_map_create64(0U, -1.0, SEED);
	if (!hash_set)
	{
		puts("Allocation has failed!");
		return EXIT_FAILURE;
	}

	RUN_TEST_CASE(1);
	RUN_TEST_CASE(2);
	RUN_TEST_CASE(3);
	RUN_TEST_CASE(4);

	hash_map_destroy64(hash_set);
	puts("Tests completed successfully.");
	return EXIT_SUCCESS;

failure:
	hash_map_destroy64(hash_set);
	puts("\nSomething went wrong !!!");
	return EXIT_FAILURE;
}
