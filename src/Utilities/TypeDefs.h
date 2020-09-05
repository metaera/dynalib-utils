/*
 * TypeDefs.h
 *
 *  Created on: 2/10/2017
 *      Author: ken
 */
#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#include <cstdint>

// #if defined(__APPLE__) && defined(__MACH__)
// //#define MODEL_LP64
// #elif defined(__MINGW32__)
// #elif defined(_WIN32)
// #elif defined(_WIN64)
// #elif defined(__MINGW64__)
// #elif defined(__linux__)
// #elif defined(__unix__)
// #endif

typedef uint8_t  uchar;
typedef uint16_t ushort;

#if defined(__LP32__)
    typedef uint16_t uint;
    typedef uint32_t ulong;
    typedef uint64_t ulonglong;
#elif defined(__ILP32__)
    typedef uint32_t uint;
    typedef uint32_t ulong;
    typedef uint64_t ulonglong;
#elif defined(__LLP64__)
    typedef uint32_t uint;
    typedef uint32_t ulong;
    typedef uint64_t ulonglong;
#elif defined(__LP64__)
    typedef uint32_t uint;
    typedef uint64_t ulong;
    typedef uint64_t ulonglong;
#endif


#define __NOOP  do {} while (0)

#endif /* TYPEDEFS_H_ */
