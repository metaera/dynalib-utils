//
// Created by Ken Kopelson on 18/10/17.
//

#ifndef DYNAALLOC_H
#define DYNAALLOC_H

#include <exception>
#include <type_traits>
#include <cstring>
#include "TypeDefs.h"

template <class T> class DynaAllocArray {
    static uint ALLOC_UNITS;

public:
    static T*   newArray(uint count);
    static T*   reallocArray(T* array, uint oldCount, uint newCount);
    static void deleteArray(T* array);
    static void clearArray(T* array, uint count);

    static void setAllocUnits(uint allocUnits) { ALLOC_UNITS = allocUnits; }
    static uint getAllocUnits() { return ALLOC_UNITS; }
};

template <class T> class DynaAllocVect {
    static uint ALLOC_UNITS;
public:
    static T**  newVect(uint count);
    static T**  reallocVect(T** array, uint oldCount, uint newCount, bool isOwner);
    static void deleteVect(T** array, uint arrayCount, bool isOwner);

    static void setAllocUnits(uint allocUnits) { ALLOC_UNITS = allocUnits; }
    static uint getAllocUnits() { return ALLOC_UNITS; }
};


#endif //DYNAALLOC_H
