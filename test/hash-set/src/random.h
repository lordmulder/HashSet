/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#ifndef _TEST_RANDOM_INCLUDED
#define _TEST_RANDOM_INCLUDED

#include <stdlib.h>
#include <stdint.h>

typedef struct
{
	size_t offset;
	uint64_t buffer[16U];
}
random_t;

void random_init(random_t *const rnd);
uint64_t random_next(random_t *const rnd);

#endif /*_TEST_RANDOM_INCLUDED*/
