/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#include <hash_map.h>

#define NAME_SUFFIX 16
typedef hash_map16_t hash_map_t;
typedef hash_map_callback16_t hash_map_callback_t;
typedef uint16_t value_t;

#include "generic_hash_map.h"
