/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#include <hash_map.h>
#include <stdio.h>
#include <inttypes.h>
#include "input.h"

/* ========================================================================= */
/* MAIN                                                                      */
/* ========================================================================= */

int main(void)
{
	hash_map64_t *hash_map;
	uint64_t key, value;
	size_t cursor = 0U, offset = 0U;

	/* print logo */
	printf("LibHashSet Hash-Map Example v%" PRIu16 ".%" PRIu16 ".%" PRIu16 " [%s]\n\n",
		HASHSET_VERSION_MAJOR, HASHSET_VERSION_MINOR, HASHSET_VERSION_PATCH, HASHSET_BUILD_DATE);

	/* create new hash map instance */
	hash_map = hash_map_create64(0U, -1.0, 42U);
	if (!hash_map)
	{
		fputs("Allocation has failed!\n", stderr);
		return EXIT_FAILURE;
	}

	/* add a number of items to the hash map, the map will grow as needed */
	puts("Inserting key-value pairs into hash map, please wait...");
	while (have_more_items(offset))
	{
		const pair_t input = get_next_item(offset++);
		const errno_t error = hash_map_insert64(hash_map, input.key, input.value);
		if (error)
		{
			fprintf(stderr, "Insert operation failed! (error: %d)\n", error);
			return EXIT_FAILURE;
		}
	}
	puts("Done.\n");

	/* print total number of items in the hash map */
	printf("Total number of entries in the map: %zu\n\n", hash_map_size64(hash_map));

	/* print all items in the map */
	while (hash_map_iterate64(hash_map, &cursor, &key, &value) == 0)
	{
		printf("Entry: 0x%016" PRIX64 " -> 0x%016" PRIX64 "\n", key, value);
	}

	/* destroy the hash map, when it is no longer needed! */
	hash_map_destroy64(hash_map);
	return EXIT_SUCCESS;
}
