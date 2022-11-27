/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#include <hash_set.h>
#include "common.h"

#define NAME_SUFFIX 64
#define SET_TYPE hash_set64_t
#define VALUE_TYPE uint64_t

#include "generic.h"
