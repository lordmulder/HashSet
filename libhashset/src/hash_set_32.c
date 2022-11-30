/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#include <hash_set.h>

#define NAME_SUFFIX 32
typedef hash_set32_t hash_set_t;
typedef hash_set_callback32_t hash_set_callback_t;
typedef uint32_t value_t;

#include "generic_hash_set.h"
