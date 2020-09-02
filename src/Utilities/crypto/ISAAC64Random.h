//
// Created by Ken Kopelson on 27/04/18.
//

/*
------------------------------------------------------------------------------
ISAAC64Random.h: definitions for a crypto number generator
Bob Jenkins, 1996, Public Domain
------------------------------------------------------------------------------
*/
#ifndef STANDARD
#include "standard.h"
#endif

#ifndef ISAAC64
#define ISAAC64

#include <cstdint>
#include <cstddef>
#include "../TypeDefs.h"

using namespace std;

#define RANDSIZL   (8)
#define RANDSIZ    (1<<RANDSIZL)

struct Rand64Context {
    uint64_t randcnt;
    uint64_t randrsl[RANDSIZ];
    uint64_t randmem[RANDSIZ];
    uint64_t randa;
    uint64_t randb;
    uint64_t randc;
};

/*
------------------------------------------------------------------------------
 If (flag==TRUE), then use the contents of randrsl[0..255] as the seed.
------------------------------------------------------------------------------
*/
extern void     isaac64Seed(char* seed, Rand64Context* ctx);
extern void     isaac64Init(bool hasSeed, Rand64Context* ctx);
extern void     isaac64Random(Rand64Context* ctx);
extern uint64_t rand64Int(Rand64Context* ctx);
extern void     rand64Bytes(Rand64Context* ctx, uint8_t* buf, int count);

#endif  /* RAND */
