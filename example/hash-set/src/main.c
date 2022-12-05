/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#include <hash_set.h>
#include <stdio.h>
#include <inttypes.h>
#include "input.h"

/* ========================================================================= */
/* MAIN                                                                      */
/* ========================================================================= */

int main(void)
{
	hash_set64_t *hash_set;
	uint64_t item;
	size_t cursor = 0U, offset = 0U;

	/* print logo */
	printf("LibHashSet Hash-Set Example v%" PRIu16 ".%" PRIu16 ".%" PRIu16 " [%s]\n\n",
		HASHSET_VERSION_MAJOR, HASHSET_VERSION_MINOR, HASHSET_VERSION_PATCH, HASHSET_BUILD_DATE);

	/* create new hash set instance */
	hash_set = hash_set_create64(0U, -1.0, 42U);
	if (!hash_set)
	{
		fputs("Allocation has failed!\n", stderr);
		return EXIT_FAILURE;
	}

	/* add a number of items to the hash set, the set will grow as needed */
	puts("Inserting items into hash set, please wait...");
	while (have_more_items(offset))
	{
		const errno_t error = hash_set_insert64(hash_set, get_next_item(offset++));
		if (error)
		{
			fprintf(stderr, "Insert operation failed! (error: %d)\n", error);
			return EXIT_FAILURE;
		}
	}
	puts("Done.\n");

	/* print total number of items in the hash set*/
	printf("Total number of items in the set: %zu\n\n", hash_set_size64(hash_set));

	/* print all items in the set */
	while (hash_set_iterate64(hash_set, &cursor, &item) == 0)
	{
		printf("Item: 0x%016" PRIX64 "\n", item);
	}
	puts("");

	/* destroy the hash set, when it is no longer needed! */
	hash_set_destroy64(hash_set);
	return EXIT_SUCCESS;
}
