/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#ifndef _LIBHASHSET_SET_INCLUDED
#define _LIBHASHSET_SET_INCLUDED

#include <stdlib.h>
#include <stdint.h>

#if defined(_MSC_VER) && defined(HASHSET_DLL)
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

#if !defined(_MSC_VER) && !defined(__MINGW32__) && !defined(_LIBHASHSET_ERRNO_DEFINED)
typedef int errno_t;
#define _LIBHASHSET_ERRNO_DEFINED 1
#endif

/* ------------------------------------------------- */
/* Globals                                           */
/* ------------------------------------------------- */

#ifndef _LIBHASHSET_GLOBALS_DEFINED
#define _LIBHASHSET_GLOBALS_DEFINED

HASHSET_API const uint16_t HASHSET_VERSION_MAJOR;
HASHSET_API const uint16_t HASHSET_VERSION_MINOR;
HASHSET_API const uint16_t HASHSET_VERSION_PATCH;

HASHSET_API const char *const HASHSET_BUILD_DATE;
HASHSET_API const char *const HASHSET_BUILD_TIME;

#endif /*_LIBHASHSET_GLOBALS_DEFINED*/

/* ------------------------------------------------- */
/* Types                                             */
/* ------------------------------------------------- */

struct _hash_set16;
struct _hash_set32;
struct _hash_set64;

typedef struct _hash_set16 hash_set16_t;
typedef struct _hash_set32 hash_set32_t;
typedef struct _hash_set64 hash_set64_t;

typedef int (*hash_set_callback16_t)(const size_t index, const char status, const uint16_t item);
typedef int (*hash_set_callback32_t)(const size_t index, const char status, const uint32_t item);
typedef int (*hash_set_callback64_t)(const size_t index, const char status, const uint64_t item);

/* ------------------------------------------------- */
/* Functions                                         */
/* ------------------------------------------------- */

HASHSET_API hash_set16_t *hash_set_create16(const size_t initial_capacity, const double load_factor, const uint32_t seed);
HASHSET_API hash_set32_t *hash_set_create32(const size_t initial_capacity, const double load_factor, const uint32_t seed);
HASHSET_API hash_set64_t *hash_set_create64(const size_t initial_capacity, const double load_factor, const uint32_t seed);

HASHSET_API void hash_set_destroy16(hash_set16_t *const instance);
HASHSET_API void hash_set_destroy32(hash_set32_t *const instance);
HASHSET_API void hash_set_destroy64(hash_set64_t *const instance);

HASHSET_API errno_t hash_set_insert16(hash_set16_t *const instance, const uint16_t item);
HASHSET_API errno_t hash_set_insert32(hash_set32_t *const instance, const uint32_t item);
HASHSET_API errno_t hash_set_insert64(hash_set64_t *const instance, const uint64_t item);

HASHSET_API errno_t hash_set_remove16(hash_set16_t *const instance, const uint16_t item);
HASHSET_API errno_t hash_set_remove32(hash_set32_t *const instance, const uint32_t item);
HASHSET_API errno_t hash_set_remove64(hash_set64_t *const instance, const uint64_t item);

HASHSET_API errno_t hash_set_clear16(hash_set16_t *const instance);
HASHSET_API errno_t hash_set_clear32(hash_set32_t *const instance);
HASHSET_API errno_t hash_set_clear64(hash_set64_t *const instance);

HASHSET_API errno_t hash_set_contains16(const hash_set16_t *const instance, const uint16_t item);
HASHSET_API errno_t hash_set_contains32(const hash_set32_t *const instance, const uint32_t item);
HASHSET_API errno_t hash_set_contains64(const hash_set64_t *const instance, const uint64_t item);

HASHSET_API errno_t hash_set_iterate16(const hash_set16_t *const instance, size_t *const cursor, uint16_t *const item);
HASHSET_API errno_t hash_set_iterate32(const hash_set32_t *const instance, size_t *const cursor, uint32_t *const item);
HASHSET_API errno_t hash_set_iterate64(const hash_set64_t *const instance, size_t *const cursor, uint64_t *const item);

HASHSET_API size_t hash_set_size16(const hash_set16_t *const instance);
HASHSET_API size_t hash_set_size32(const hash_set32_t *const instance);
HASHSET_API size_t hash_set_size64(const hash_set64_t *const instance);

HASHSET_API errno_t hash_set_info16(const hash_set16_t *const instance, size_t *const capacity, size_t *const valid, size_t *const deleted, size_t *const limit);
HASHSET_API errno_t hash_set_info32(const hash_set32_t *const instance, size_t *const capacity, size_t *const valid, size_t *const deleted, size_t *const limit);
HASHSET_API errno_t hash_set_info64(const hash_set64_t *const instance, size_t *const capacity, size_t *const valid, size_t *const deleted, size_t *const limit);

HASHSET_API errno_t hash_set_dump16(const hash_set16_t *const instance, const hash_set_callback16_t callback);
HASHSET_API errno_t hash_set_dump32(const hash_set32_t *const instance, const hash_set_callback32_t callback);
HASHSET_API errno_t hash_set_dump64(const hash_set64_t *const instance, const hash_set_callback64_t callback);

#ifdef __cplusplus
}
#endif
#endif /*_LIBHASHSET_SET_INCLUDED*/
