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

#define HASHSET_OPT_FAILFAST UINT16_C(0x1)

struct _hash_set;
typedef struct _hash_set hash_set_t;

hash_set_t *hash_set_create(const size_t initial_capacity, const double load_factor, const uint16_t options);
void hash_set_destroy(hash_set_t *instance);

errno_t hash_set_insert(hash_set_t *const instance, const uint64_t value);
errno_t hash_set_remove(hash_set_t *const instance, const uint64_t value);
errno_t hash_set_clear(hash_set_t *const instance);
errno_t hash_set_shrink(hash_set_t *const instance);

errno_t hash_set_contains(const hash_set_t *const instance, const uint64_t value);
errno_t hash_set_iterate(const hash_set_t *const instance, size_t *const offset, uint64_t *const value);
size_t hash_set_size(const hash_set_t *const instance);
errno_t hash_set_info(const hash_set_t *const instance, size_t *const capacity, size_t *const valid, size_t *const deleted, size_t *const limit);

#ifdef __cplusplus
}
#endif
#endif
