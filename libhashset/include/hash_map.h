/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#ifndef _LIBHASHSET_MAP_INCLUDED
#define _LIBHASHSET_MAP_INCLUDED

#include <stdlib.h>
#include <stdint.h>

#if defined(_WIN32) && defined(HASHSET_DLL)
#  ifdef _HASHSET_EXPORTS
#    define HASHSET_API extern __declspec(dllexport)
#  else
#    define HASHSET_API extern __declspec(dllimport)
#  endif
#else
#  define HASHSET_API extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_MSC_VER) && !defined(__MINGW32__) && !defined(_ERRNO_T_DEFINED)
typedef int errno_t;
#endif

HASHSET_API const uint16_t HASHSET_VERSION_MAJOR;
HASHSET_API const uint16_t HASHSET_VERSION_MINOR;
HASHSET_API const uint16_t HASHSET_VERSION_PATCH;

HASHSET_API const char *const HASHSET_BUILD_DATE;
HASHSET_API const char *const HASHSET_BUILD_TIME;

struct _hash_map32;
struct _hash_map64;

typedef struct _hash_map32 hash_map32_t;
typedef struct _hash_map64 hash_map64_t;

HASHSET_API hash_map32_t *hash_map_create32(const size_t initial_capacity, const double load_factor);
HASHSET_API hash_map64_t *hash_map_create64(const size_t initial_capacity, const double load_factor);

HASHSET_API void hash_map_destroy32(hash_map32_t *const instance);
HASHSET_API void hash_map_destroy64(hash_map64_t *const instance);

HASHSET_API errno_t hash_map_insert32(hash_map32_t *const instance, const uint32_t key, const uintptr_t value);
HASHSET_API errno_t hash_map_insert64(hash_map64_t *const instance, const uint64_t key, const uintptr_t value);

HASHSET_API errno_t hash_map_remove32(hash_map32_t *const instance, const uint32_t key);
HASHSET_API errno_t hash_map_remove64(hash_map64_t *const instance, const uint64_t key);

HASHSET_API errno_t hash_map_clear32(hash_map32_t *const instance);
HASHSET_API errno_t hash_map_clear64(hash_map64_t *const instance);

HASHSET_API errno_t hash_map_contains32(const hash_map32_t *const instance, const uint32_t key);
HASHSET_API errno_t hash_map_contains64(const hash_map64_t *const instance, const uint64_t key);

HASHSET_API errno_t hash_map_get32(const hash_map32_t *const instance, const uint32_t key, uintptr_t *const value);
HASHSET_API errno_t hash_map_get64(const hash_map64_t *const instance, const uint64_t key, uintptr_t *const value);

HASHSET_API errno_t hash_map_iterate32(const hash_map32_t *const instance, size_t *const cursor, uint32_t *const key, uintptr_t *const value);
HASHSET_API errno_t hash_map_iterate64(const hash_map64_t *const instance, size_t *const cursor, uint64_t *const key, uintptr_t *const value);

HASHSET_API size_t hash_map_size32(const hash_map32_t *const instance);
HASHSET_API size_t hash_map_size64(const hash_map64_t *const instance);

HASHSET_API errno_t hash_map_info32(const hash_map32_t *const instance, size_t *const capacity, size_t *const valid, size_t *const deleted, size_t *const limit);
HASHSET_API errno_t hash_map_info64(const hash_map64_t *const instance, size_t *const capacity, size_t *const valid, size_t *const deleted, size_t *const limit);

HASHSET_API errno_t hash_map_dump32(const hash_map32_t *const instance, int (*const callback)(const size_t index, const char status, const uint32_t key, const uintptr_t value));
HASHSET_API errno_t hash_map_dump64(const hash_map64_t *const instance, int (*const callback)(const size_t index, const char status, const uint64_t key, const uintptr_t value));

#ifdef __cplusplus
}
#endif
#endif /*_LIBHASHSET_MAP_INCLUDED*/