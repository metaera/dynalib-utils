//
// Created by Ken Kopelson on 4/05/18.
//

#ifndef ICACHECONST_H
#define ICACHECONST_H

#include "IndexConfig.h"
#include "DynaArray.h"
#include "DynaHashMap.h"

#define CHAR_BYTES          sizeof(char)        // 1 byte
#define SHORT_BYTES         sizeof(short)       // 2 bytes
#define UUID_BYTES          16
#define STR_BYTES           255

#ifdef USE_LONG
    #define INDEX_BYTES     sizeof(int64_t)     // 8 bytes
    typedef int64_t index_t;
#else
    #define INDEX_BYTES     sizeof(int32_t)     // 4 bytes
    typedef int32_t index_t;
#endif

MAKE_ARRAYTYPE_DEF(index_t, IndexRef);

#endif //ICACHECONST_H
