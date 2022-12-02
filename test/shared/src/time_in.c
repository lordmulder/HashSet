/******************************************************************************/
/* HashSet for C99, by LoRd_MuldeR <MuldeR2@GMX.de>                           */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#include "../include/time_in.h"

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN 1
#  include <Windows.h>
#else
#  include <time.h>
#endif

uint64_t clock_now(void)
{
#ifdef _WIN32
    LARGE_INTEGER counter;
    if (QueryPerformanceCounter(&counter))
    {
        return counter.QuadPart;
    }
#else
    struct timespec spec;
    if (!clock_gettime(CLOCK_MONOTONIC, &spec))
    {
        return (((uint64_t)spec.tv_sec) << 32) | (((uint64_t)spec.tv_nsec) & UINT64_C(0xFFFFFFFF));
    }
#endif
    return 0U;
}
