/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#ifndef _LIBHASHSET_COMMON_INCLUDED
#define _LIBHASHSET_COMMON_INCLUDED

/* CRT */
#include <string.h>
#include <errno.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <assert.h>

typedef int bool_t;
#define TRUE  1
#define FALSE 0

#if defined(__GNUC__)
#  define INLINE __inline__
#  define FORCE_INLINE __attribute__((always_inline)) __inline__
#elif defined(_MSC_VER)
#  define INLINE __inline
#  define FORCE_INLINE __forceinline
#else
#  define INLINE
#endif

static const size_t MINIMUM_CAPACITY =  128U;
static const size_t DEFAULT_CAPACITY = 8192U;
static const double DEFAULT_LOADFCTR =   0.8;

/* ------------------------------------------------- */
/* Utilities                                         */
/* ------------------------------------------------- */

#define IS_VALID(X,Y) (get_flag((X).used, (Y)) && (!get_flag((X).deleted, (Y))))

#define BOUND(MIN,VAL,MAX) (((VAL) < (MIN)) ? (MIN) : (((VAL) > (MAX)) ? (MAX) : (VAL)))

#define SAFE_SET(X,Y) do { if((X)) { *(X) = (Y); } } while(0)

#define SAFE_FREE(X) do { if ((X)) { free((X)); (X) = NULL; } } while(0)

#define _NAME_GLUE_HELPER(X,Y) X##Y
#define NAME_GLUE(X,Y) _NAME_GLUE_HELPER(X,Y)

/* ------------------------------------------------- */
/* Math                                              */
/* ------------------------------------------------- */

static FORCE_INLINE size_t div_ceil(const size_t value, const size_t divisor)
{
	return (value / divisor) + ((value % divisor != 0U) ? 1U : 0U);
}

static FORCE_INLINE size_t round_sz(const double d)
{

	return ((!isnan(d)) && (d >= 0.0)) ? ((d + 0.5 >= ((double)SIZE_MAX)) ? SIZE_MAX : ((size_t)(d + 0.5))) : 0U;
}

static FORCE_INLINE size_t safe_add(const size_t a, const size_t b)
{
	return ((SIZE_MAX - a) > b) ? (a + b) : SIZE_MAX;
}

static FORCE_INLINE size_t safe_incr(const size_t value)
{
	return (value < SIZE_MAX) ? (value + 1U) : value;
}

static FORCE_INLINE size_t safe_decr(const size_t value)
{
	return (value > 0U) ? (value - 1U) : value;
}

static FORCE_INLINE size_t safe_mult(const size_t a, const size_t b)
{
	const size_t result = a * b;
	return ((a == 0U) || (result / a == b)) ? result : SIZE_MAX;
}

static FORCE_INLINE size_t safe_times2(const size_t value)
{
	return (value <= (SIZE_MAX / 2U)) ? (2U * value) : SIZE_MAX;
}

static FORCE_INLINE size_t next_pow2(const size_t target)
{
	size_t result = MINIMUM_CAPACITY;

	while (result < target)
	{
		result = safe_times2(result);
	}

	return result;
}

/* ------------------------------------------------- */
/* Flags                                             */
/* ------------------------------------------------- */

static INLINE bool_t get_flag(const uint8_t* const flags, const size_t index)
{
	return (flags[index / 8U] >> (index % 8U)) & UINT8_C(1);
}

static INLINE void set_flag(uint8_t* const flags, const size_t index)
{
	flags[index / 8U] |= UINT8_C(1) << (index % 8U);
}

static INLINE void clear_flag(uint8_t* const flags, const size_t index)
{
	flags[index / 8U] &= ~(UINT8_C(1) << (index % 8U));
}

#endif
