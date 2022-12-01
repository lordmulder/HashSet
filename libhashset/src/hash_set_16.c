/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#include <hash_set.h>

#define NAME_SUFFIX 16
typedef hash_set16_t hash_set_t;
typedef hash_set_callback16_t hash_set_callback_t;
typedef uint16_t value_t;

#include "generic_hash_set.h"
