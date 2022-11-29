/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#include <hash_set.h>
#include <stdio.h>
#include <inttypes.h>

#define ARRAY_SIZE(X) (sizeof(X) / sizeof((X)[0U]))

/* ========================================================================= */
/* I/O Functions                                                             */
/* ========================================================================= */

static const uint64_t INPUT_DATA[] =
{
	UINT64_C(0xEC7D0A721D57EC50), UINT64_C(0x2D6A7AB6BF627C22), UINT64_C(0x4FDD65C766A875B3), UINT64_C(0x820325C26E55AADE), UINT64_C(0xDAAA501D96EEF99B),
	UINT64_C(0x8144E2BD50513672), UINT64_C(0x292CE22A29D5DB2B), UINT64_C(0xBC6CF8B2053A5334), UINT64_C(0x6D1401A1D373E7D1), UINT64_C(0x24ADEDD753FA66AE),
	UINT64_C(0xC10245EBF7AA2022), UINT64_C(0x023529AB3C7CD028), UINT64_C(0x36D2B9AC4939E5B6), UINT64_C(0xC03EBAFFF82F66AA), UINT64_C(0xF192D661B62529F8),
	UINT64_C(0xB704F3B5D339E013), UINT64_C(0xA9FF1DACC3E51172), UINT64_C(0x56B07D81D1075239), UINT64_C(0xB1871C613F7AABE2), UINT64_C(0xA8EF994549F8FE4A),
	UINT64_C(0xA57BB923C5253144), UINT64_C(0x00D86833851697B3), UINT64_C(0xDA9E46FDD32DC65D), UINT64_C(0x7ACB9C0ADDDFAB93), UINT64_C(0xB5B5850E76C843DB),
	UINT64_C(0x3B6A7C89C7E81A5D), UINT64_C(0x4EB1262597175FCD), UINT64_C(0xB6FBE2171D1D7EA0), UINT64_C(0x081137EA1D536FC4), UINT64_C(0xE0445CD0FAD5C7DD),
	UINT64_C(0x893007682A3BF169), UINT64_C(0xD0C19098FBD42472), UINT64_C(0xEDB93F32F0A49B30), UINT64_C(0x2EA9A358058AB291), UINT64_C(0x8DAEDB071BA37583),
	UINT64_C(0x7D82157789453793), UINT64_C(0xBB2773CA12083932), UINT64_C(0x6303F020DEFC11A9), UINT64_C(0xD41F0AD15E0189DC), UINT64_C(0x19B057F06E39C89E),
	UINT64_C(0x4253F25E0B30719C), UINT64_C(0x09463D8E5BA23234), UINT64_C(0x34D04E31976724AA), UINT64_C(0x4671121B954F5A79), UINT64_C(0xE92C10C3DB5D8D1B),
	UINT64_C(0xE9CBC62D55911AE7), UINT64_C(0x2A1AB1C994DB788E), UINT64_C(0xC8C33ECF2C582D77), UINT64_C(0x5FA82FE6C15B629E), UINT64_C(0x466CEED23C511177),
	UINT64_C(0x036D0870BAE19AE1), UINT64_C(0x6606CC0204159016), UINT64_C(0xD6238C8747BC5852), UINT64_C(0x223E4185F09E378F), UINT64_C(0xA9B3F004F76E5444),
	UINT64_C(0x23C9471F9E0284DD), UINT64_C(0x8B8FD1A11AA899F1), UINT64_C(0x0AB371E3FC9D3B56), UINT64_C(0xAC2E67112F3A040D), UINT64_C(0x192A053B04315830),
	UINT64_C(0x432D3397538C1F92), UINT64_C(0xE959C8CB414D0179), UINT64_C(0x2CBAD874DD3E7ED3), UINT64_C(0x27C1B56C45A07443), UINT64_C(0x985DCE41B814DB32),
	UINT64_C(0x7D89DA5C4F0A66B3), UINT64_C(0x8BDD31E4EBE03149), UINT64_C(0xDE099239E444C693), UINT64_C(0x1ADD95B03AF163A0), UINT64_C(0x36CD32A83DE833F2),
	UINT64_C(0xDF4A8C3B1DC66B09), UINT64_C(0xC29C92C5A077B9FE), UINT64_C(0x56950ECB370CBC27), UINT64_C(0x7FEAD937FCA475F1), UINT64_C(0x1E4AD443AD9D41EC),
	UINT64_C(0x9A265C879D7E5872), UINT64_C(0x455AB983F8BA1441), UINT64_C(0x027C2ED163D11C29), UINT64_C(0xA7A7D23CAA6F4DC6), UINT64_C(0xEE7939920BE1E7BC),
	UINT64_C(0x2AE02B16E28F02F6), UINT64_C(0xFE8A0CB4EB33DA1B), UINT64_C(0x8CCBF16F006BE241), UINT64_C(0x3371EF26F2850DAF), UINT64_C(0xC35EA57F39EA9E58),
	UINT64_C(0x20977AD263FB0272), UINT64_C(0x29BBDD2B8DFCD944), UINT64_C(0x3EF8BD07A2CA3369), UINT64_C(0x64FC3600F0B37716), UINT64_C(0x53CBD741D2433E51),
	UINT64_C(0x69459841B5CEE1AB), UINT64_C(0x170B6EBB26B0102C), UINT64_C(0xAF7D361D649AED05), UINT64_C(0x8843B951E1A79CA4), UINT64_C(0xCC0B139063D3EF0A),
	UINT64_C(0x85CB5DBD53986A64), UINT64_C(0x7C66F5CD3E46FDB7), UINT64_C(0xE863F4436E8BD7F8), UINT64_C(0x82496CD067153344), UINT64_C(0x863F1F732CE3751E),
};

static int have_more_items(const size_t offset)
{
	return offset < ARRAY_SIZE(INPUT_DATA);
}

static uint64_t get_next_item(const size_t offset)
{
	if (offset < ARRAY_SIZE(INPUT_DATA))
	{
		return INPUT_DATA[offset];
	}
	return 0U;
}

/* ========================================================================= */
/* MAIN                                                                      */
/* ========================================================================= */

int main(void)
{
	hash_set64_t *hash_set;
	uint64_t value;
	uintptr_t cursor = 0U;
	size_t offset = 0U;

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
	while (hash_set_iterate64(hash_set, &cursor, &value) == 0)
	{
		printf("Item: 0x%016" PRIX64 "\n", value);
	}

	/* destroy the hash set, when it is no longer needed! */
	hash_set_destroy64(hash_set);
	return EXIT_SUCCESS;
}
