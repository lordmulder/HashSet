/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#include <hash_set.h>
#include "common.h"

#define NAME_SUFFIX 32
typedef hash_set32_t hash_set_t;
typedef uint32_t value_t;

#include "generic.h"
