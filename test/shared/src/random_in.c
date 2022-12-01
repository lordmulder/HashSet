/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#include "../include/random_in.h"
#include <string.h>

#ifndef _WIN32
#  include <unistd.h>
#endif

#define ARRAY_SIZE(X) (sizeof(X) / sizeof((X)[0U]))

#ifdef _WIN32
#define RtlGenRandom SystemFunction036
#define getentropy(X,Y) (RtlGenRandom((X),(uint32_t)(Y)) ? 0 : (-1))
unsigned char __stdcall RtlGenRandom(void* buffer, uint32_t length);
#endif

void random_init(random_t *const rnd)
{
	memset(rnd, 0, sizeof(random_t));
	rnd->offset = SIZE_MAX;
}

uint64_t random_next(random_t *const rnd)
{
	if (rnd->offset >= ARRAY_SIZE(rnd->buffer))
	{
		rnd->offset = 0U;
		if (getentropy(rnd->buffer, sizeof(rnd->buffer)) < 0)
		{
			abort();
		}
	}

	return rnd->buffer[rnd->offset++];
}
