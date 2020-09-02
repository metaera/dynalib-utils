//
// Created by Ken Kopelson on 30/04/18.
//

/*
------------------------------------------------------------------------------
ISAACRandom.h: definitions for a crypto number generator
Bob Jenkins, 1996, Public Domain
------------------------------------------------------------------------------
*/
#ifndef STANDARD
#include "standard.h"
#endif

#ifndef ISAAC
#define ISAAC

#include <cstdint>
#include <cstddef>
#include "../TypeDefs.h"

using namespace std;

#define RANDSIZL   (8)
#define RANDSIZ    (1<<RANDSIZL)

struct RandContext {
    uint32_t randcnt;
    uint32_t randrsl[RANDSIZ];
    uint32_t randmem[RANDSIZ];
    uint32_t randa;
    uint32_t randb;
    uint32_t randc;
};

/*
------------------------------------------------------------------------------
 If (flag==TRUE), then use the contents of randrsl[0..255] as the seed.
------------------------------------------------------------------------------
*/
extern void     isaacSeed(char* seed, RandContext* ctx);
extern void     isaacInit(bool hasSeed, RandContext* ctx);
extern void     isaacRandom(RandContext* ctx);
extern uint32_t randInt(RandContext* ctx);
extern void     randBytes(RandContext* ctx, uchar* buf, int count);

#endif  /* RAND */
