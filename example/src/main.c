/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#include <hash_set.h>
#include <stdio.h>
#include <inttypes.h>

#define INPUT_COUNT 100U

/* ========================================================================= */
/* I/O Functions                                                             */
/* ========================================================================= */

static size_t g_offset = 0U;

static const uint64_t g_input[INPUT_COUNT] =
{
	0x0005, 0x0007, 0x000B, 0x0017, 0x002F, 0x003B, 0x0053, 0x006B, 0x00A7, 0x00B3,
	0x00E3, 0x0107, 0x015B, 0x0167, 0x017F, 0x01D3, 0x01DF, 0x01F7, 0x0233, 0x024B,
	0x02CF, 0x0347, 0x035F, 0x0377, 0x03D7, 0x03FB, 0x04A3, 0x0503, 0x051B, 0x0527,
	0x0557, 0x059F, 0x05CF, 0x05F3, 0x0653, 0x071F, 0x0773, 0x07EB, 0x07F7, 0x080F,
	0x0833, 0x089F, 0x098F, 0x099B, 0x0A13, 0x0B03, 0x0B3F, 0x0B57, 0x0B93, 0x0BB7,
	0x0BCF, 0x0C2F, 0x0C5F, 0x0C83, 0x0D8B, 0x0E27, 0x0EC3, 0x0EDB, 0x0F17, 0x0F6B,
	0x0FA7, 0x0FEF, 0x101F, 0x102B, 0x10A3, 0x10BB, 0x11C3, 0x1247, 0x125F, 0x12B3,
	0x12BF, 0x1337, 0x13DF, 0x13EB, 0x150B, 0x1517, 0x156B, 0x1583, 0x1607, 0x16AF,
	0x16F7, 0x1727, 0x1733, 0x179F, 0x19C7, 0x1A03, 0x1A3F, 0x1A7B, 0x1AAB, 0x1AF3,
	0x1B47, 0x1BA7, 0x1C13, 0x1C4F, 0x1D63, 0x1D87, 0x1DB7, 0x1DDB, 0x1E17, 0x1E2F
};

static int have_more_items(void)
{
	return (g_offset < INPUT_COUNT);
}

static uint64_t get_next_item(void)
{
	return g_input[g_offset++];
}

/* ========================================================================= */
/* MAIN                                                                      */
/* ========================================================================= */

int main(void/*int argc, char* argv[]*/)
{
	hash_set64_t *hash_set;
	uint64_t value;
	uintptr_t cursor = 0U;

	/* print logo */
	printf("LibHashSet Example v%" PRIu16 ".%" PRIu16 ".%" PRIu16 " [%s]\n\n",
		HASHSET_VERSION_MAJOR, HASHSET_VERSION_MINOR, HASHSET_VERSION_PATCH, HASHSET_BUILD_DATE);

	/* create new hash set instance */
	hash_set = hash_set_create64(0U, -1.0);
	if (!hash_set)
	{
		fputs("Allocation has failed!\n", stderr);
		return EXIT_FAILURE;
	}

	/* add a number of items to the hash set, the set will grow as needed */
	puts("Insertign items, please wait...");
	while (have_more_items())
	{
		const errno_t error = hash_set_insert64(hash_set, get_next_item());
		if (error)
		{
			fprintf(stderr, "Insert operation has failed! (error: %d)\n", error);
			return EXIT_FAILURE;
		}
	}
	puts("Done.\n");

	/* print total number of items in the hash set*/
	printf("Total number of items: %zu\n\n", hash_set_size64(hash_set));

	/* print all items in the set */
	while (hash_set_iterate64(hash_set, &cursor, &value) == 0)
	{
		printf("Item: %016llX\n", value);
	}

	/* destroy the hash set, when it is no longer needed! */
	hash_set_destroy64(hash_set);
	return EXIT_SUCCESS;
}
