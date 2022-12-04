/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#include "tests.h"
#include "../../shared/include/time_in.h"

#include <stdio.h>
#include <inttypes.h>

#define RUN_TEST_CASE(X) do \
{ \
	if (test_function_##X(hash_set) != EXIT_SUCCESS) \
	{ \
		goto failure; \
	} \
} \
while(0)

/* ========================================================================= */
/* MAIN                                                                      */
/* ========================================================================= */

int main(void)
{
	hash_map64_t *hash_set;
	uint64_t clk_begin, clk_end;

	printf("LibHashSet Hash-Map Test v%" PRIu16 ".%" PRIu16 ".%" PRIu16 " [%s]\n\n",
		HASHSET_VERSION_MAJOR, HASHSET_VERSION_MINOR, HASHSET_VERSION_PATCH, HASHSET_BUILD_DATE);

	clk_begin = clock_query();

	hash_set = hash_map_create64(0U, -1.0, clock_query());
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
	clk_end = clock_query();
	
	printf("Tests completed successfully [%.2f].", (clk_end - clk_begin) / ((double)clock_frequency()));
	return EXIT_SUCCESS;

failure:
	hash_map_destroy64(hash_set);
	puts("\nSomething went wrong !!!");
	return EXIT_FAILURE;
}
