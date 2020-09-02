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
#include "ISAAC64Random.h"

#define ind(mm, x)  (*(uint64_t *)((ub1 *)(mm) + ((x) & ((RANDSIZ-1)<<3))))
#define rngstep(mix, a, b, mm, m, m2, r, x) \
{ \
  (x) = *(m);  \
  (a) = (mix) + *((m2)++); \
  *((m)++) = y = ind(mm,x) + (a) + (b); \
  *((r)++) = b = ind(mm,y>>RANDSIZL) + (x); \
}

#define mix(a, b, c, d, e, f, g, h) \
{ \
   a-=e; f^=h>>9;  h+=a; \
   b-=f; g^=a<<9;  a+=b; \
   c-=g; h^=b>>23; b+=c; \
   d-=h; a^=c<<15; c+=d; \
   e-=a; b^=d>>14; d+=e; \
   f-=b; c^=e<<20; e+=f; \
   g-=c; d^=f>>17; f+=g; \
   h-=d; e^=g<<14; g+=h; \
}

#define shuffle(a, b, mm, m, m2, r, x) \
{ \
    rngstep(~((a) ^ ((a) << 21)), a, b, mm, m, m2, r, x); \
    rngstep(  (a) ^ ((a) >> 5),   a, b, mm, m, m2, r, x); \
    rngstep(  (a) ^ ((a) << 12),  a, b, mm, m, m2, r, x); \
    rngstep(  (a) ^ ((a) >> 33),  a, b, mm, m, m2, r, x); \
}

void isaac64Seed(char* seed, Rand64Context* ctx) {
    size_t   len = strlen(seed);
    if (len > RANDSIZ)
        len = RANDSIZ;
    int      i   = 0;
    uint64_t *r  = ctx->randrsl;
    for (; i < len; ++i) {
        r[i] = (ub8)seed[i];
    }
    for (; i < RANDSIZ; ++i) {
        r[i] = 0;
    }
}

void isaac64Init(bool hasSeed, Rand64Context* ctx) {
    uint64_t a, b, c, d, e, f, g, h, *m, *r;

    m = ctx->randmem;
    r = ctx->randrsl;

    ctx->randa = ctx->randb = ctx->randc = (uint64_t)0;

    a=b=c=d=e=f=g=h = 0x9e3779b97f4a7c13ULL;  /* the golden ratio */

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
        m[i] = a;
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
            m[i] = a;
            m[i + 1] = b;
            m[i + 2] = c;
            m[i + 3] = d;
            m[i + 4] = e;
            m[i + 5] = f;
            m[i + 6] = g;
            m[i + 7] = h;
        }
    }

    isaac64Random(ctx);          /* fill in the first set of results */
    ctx->randcnt = RANDSIZ;      /* prepare to use the first set of results */
}

void isaac64Random(Rand64Context* ctx) {
    uint64_t a, b, x, y, *mm, *m, *m2, *r, *mend;
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

uint64_t rand64Int(Rand64Context* ctx) {
    if (ctx->randcnt--  == 0) {
        isaac64Random(ctx);
        ctx->randcnt = RANDSIZ - 1;
    }
    return ctx->randrsl[ctx->randcnt];
}

void rand64Bytes(Rand64Context* ctx, uint8_t* buf, int count) {
    uint64_t value = 0;
    for (int i = 0, j = 7; i < count; ++i) {
        if (j == 7) {
            value = rand64Int(ctx);
        }
        buf[i] = (uint8_t)((value >> (uint)j) & 0xFFu);
        if (--j < 0)
            j = 7;
    }
}

