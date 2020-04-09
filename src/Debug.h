#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <cstdio>

#ifdef DISABLE_SWO_OUTPUT
#define DEBUG_PRINTF(fmt, ...)
#define DEBUG_MSG(...)
#else
#define DEBUG_PRINTF(fmt, ...) printf(fmt, __VA_ARGS__)
#define DEBUG_MSG(str) puts(str)
#endif

#endif /* __DEBUG_H__ */
