//
// Created by Ken Kopelson on 27/04/18.
//

/*
------------------------------------------------------------------------------
ISAAC64Random.c: My crypto number generator for 64-bit machines.
By Bob Jenkins, 1996.  Public Domain.
------------------------------------------------------------------------------
*/
#include <cstring>
#include "../TypeDefs.h"
#include "standard.h"
#include "ISAACRandom.h"

#define ind(mm, x)  (*(uint32_t *)((ub1 *)(mm) + ((x) & ((RANDSIZ-1)<<2))))
#define rngstep(mix, a, b, mm, m, m2, r, x) \
{ \
  x = *m;  \
  a = (a^(mix)) + *(m2++); \
  *(m++) = y = ind(mm,x) + a + b; \
  *(r++) = b = ind(mm,y>>RANDSIZL) + x; \
}

#define mix(a, b, c, d, e, f, g, h) \
{ \
   a^=b<<11; d+=a; b+=c; \
   b^=c>>2;  e+=b; c+=d; \
   c^=d<<8;  f+=c; d+=e; \
   d^=e>>16; g+=d; e+=f; \
   e^=f<<10; h+=e; f+=g; \
   f^=g>>4;  a+=f; g+=h; \
   g^=h<<8;  b+=g; h+=a; \
   h^=a>>9;  c+=h; a+=b; \
}

#define shuffle(a, b, mm, m, m2, r, x) \
{ \
    rngstep((a << 13),    a, b, mm, m, m2, r, x); \
    rngstep((a >> 6),     a, b, mm, m, m2, r, x); \
    rngstep((a << 2),     a, b, mm, m, m2, r, x); \
    rngstep((a >> 16),    a, b, mm, m, m2, r, x); \
}

void isaacSeed(char* seed, RandContext* ctx) {
    size_t   len = strlen(seed);
    if (len > RANDSIZ)
        len = RANDSIZ;
    int      i   = 0;
    uint32_t *r  = ctx->randrsl;
    for (; i < len; ++i) {
        r[i] = (uint32_t)seed[i];
    }
    for (; i < RANDSIZ; ++i) {
        r[i] = 0;
    }
}

void isaacInit(bool hasSeed, RandContext* ctx) {
    uint32_t a, b, c, d, e, f, g, h, *m, *r;

    m = ctx->randmem;
    r = ctx->randrsl;

    ctx->randa = ctx->randb = ctx->randc = (uint32_t)0;

    a=b=c=d=e=f=g=h = 0x9e3779b9;            /* the golden ratio */

    for (int i = 0; i < 4; ++i) {            /* scramble it */
        mix(a, b, c, d, e, f, g, h);
    }

    for (int i = 0; i < RANDSIZ; i += 8) { /* fill in mm[] with messy stuff */
        if (hasSeed) {                     /* use all the information in the seed */
            a += r[i];
            b += r[i + 1];
            c += r[i + 2];
            d += r[i + 3];
            e += r[i + 4];
            f += r[i + 5];
            g += r[i + 6];
            h += r[i + 7];
        }
        mix(a, b, c, d, e, f, g, h);
        m[i]     = a;
        m[i + 1] = b;
        m[i + 2] = c;
        m[i + 3] = d;
        m[i + 4] = e;
        m[i + 5] = f;
        m[i + 6] = g;
        m[i + 7] = h;
    }

    if (hasSeed) {     /* do a second pass to make all of the seed affect all of mm */
        for (int i = 0; i < RANDSIZ; i += 8) {
            a += m[i];
            b += m[i + 1];
            c += m[i + 2];
            d += m[i + 3];
            e += m[i + 4];
            f += m[i + 5];
            g += m[i + 6];
            h += m[i + 7];
            mix(a, b, c, d, e, f, g, h);
            m[i]     = a;
            m[i + 1] = b;
            m[i + 2] = c;
            m[i + 3] = d;
            m[i + 4] = e;
            m[i + 5] = f;
            m[i + 6] = g;
            m[i + 7] = h;
        }
    }

    isaacRandom(ctx);          /* fill in the first set of results */
    ctx->randcnt = RANDSIZ;    /* prepare to use the first set of results */
}

void isaacRandom(RandContext* ctx) {
    uint32_t a, b, x, y, *mm, *m, *m2, *r, *mend;
    mm = ctx->randmem;
    r  = ctx->randrsl;
    a  = ctx->randa;
    b  = ctx->randb + (++ctx->randc);
    for (m = mm, mend = m2 = m + (RANDSIZ / 2); m < mend;) {
        shuffle(a, b, mm, m, m2, r, x);
    }
    for (m2 = mm; m2 < mend;) {
        shuffle(a, b, mm, m, m2, r, x);
    }
    ctx->randb = b;
    ctx->randa = a;
}

uint32_t randInt(RandContext* ctx) {
    if (ctx->randcnt--  == 0) {
        isaacRandom(ctx);
        ctx->randcnt = RANDSIZ - 1;
    }
    return ctx->randrsl[ctx->randcnt];
}

void randBytes(RandContext* ctx, uchar* buf, int count) {
    uint32_t value = 0;
    for (int i = 0, j = 3; i < count; ++i) {
        if (j == 3) {
            value = randInt(ctx);
        }
        buf[i] = (uchar)((value >> (uint)j) & 0xFFu);
        if (--j < 0)
            j = 3;
    }
}
