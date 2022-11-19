/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#ifndef _INC_HASH_SET_H
#define _INC_HASH_SET_H

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _hash_set;
typedef struct _hash_set hash_set_t;

hash_set_t *hash_set_create(const double load_factor, const int fail_fast, const size_t initial_capacity);
void hash_set_destroy(hash_set_t *const instance);

int hash_set_insert(hash_set_t *const instance, const uint64_t value);
int hash_set_contains(hash_set_t *const instance, const uint64_t value);

size_t hash_set_current_size(hash_set_t *const instance);

#ifdef __cplusplus
}
#endif
#endif
