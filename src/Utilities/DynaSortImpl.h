/**
 * Dyna Sort Implementation
 *
 * This file includes the implementation of the DynaSort template.  This file should be included in any .cpp file
 * that will instantiate a new type of DynaSort.  The regular "DynaSort.h" file is then included everywhere a
 * DynaSort is used.
 */

#ifndef DYNASORTIMPL_H
#define DYNASORTIMPL_H

#include <iostream>
#include <type_traits>

#include "DynaSort.h"
#include "BitManip.h"
#include "DynaAllocImpl.h"
#include "DynaList.h"
#include "CheckForError.h"
#include "Exception.h"

#define MAKE_SORTLISTTYPE_INSTANCE(C, T) \
    template class DynaSortList<C>; \
    typedef DynaSortList<C> T##SortList; \
    typedef DynaAllocVect<C> T##AllocVect; \

#define MAKE_SORTARRAYTYPE_INSTANCE(C, T) \
    template class DynaSortArray<C>; \
    typedef DynaSortArray<C> T##SortArray; \
    typedef DynaAllocArray<C> T##AllocVect; \

//===========================================================================
//                              DynaSortList
//===========================================================================
template <class T> void DynaSortList<T>::_swap( T** x, int a, int b ) {
    T* t   = x[ a ];
    x[ a ] = x[ b ];
    x[ b ] = t;
}

template <class T> void DynaSortList<T>::_mergeSort( T** src, T** dest, int low, int high, IDynaComparator<T*>& comparator ) {
    try {
        int length = high - low;
        if ( length < INSERTION_SORT_THRESHOLD ) {
            for ( int i = low; i < high; ++i )
                for ( int j = i; j > low && comparator.compare( dest[ j - 1 ], dest[ j ] ) > 0; --j )
                    _swap( dest, j, j - 1 );
            return;
        }
        int mid = ( low + high ) >> 1;
        _mergeSort( dest, src, low, mid, comparator );
        _mergeSort( dest, src, mid, high, comparator );
        if ( comparator.compare( src[ mid - 1 ], src[ mid ] ) <= 0 ) {
            memcpy(dest + low, src + low, length * sizeof(T*));
            return;
        }
        for( int d = low, l = low, m = mid; d < high; ++d ) {
            if ( m >= high || ( l < mid && comparator.compare( src[ l ], src[ m ] ) <= 0 ) )
                dest[ d ] = src[ l++ ];
            else
                dest[ d ] = src[ m++ ];
        }
    }
    catch ( Exception e ) {
    }
}

template <class T> void DynaSortList<T>::mergeSort( T** array, int length, int low, int high, IDynaComparator<T*>& comparator ) {
    if ( array != nullptr && length > 1 && low >= 0 && high < length ) {
        auto copy = DynaAllocVect<T>::newVect(length);
        memcpy(copy, array, length * sizeof(T*));
        _mergeSort( copy, array, low, high + 1, comparator );
        delete copy;
    }
}

template <class T> void DynaSortList<T>::mergeSort( T** array, int length, IDynaComparator<T*>& comparator ) {
    mergeSort( array, length, 0, length - 1, comparator );
}

template <class T> void DynaSortList<T>::mergeSort( DynaList<T>* list, int low, int high, IDynaComparator<T*>& comparator ) {
    if ( list != nullptr )
        mergeSort( list->getInternalTypedArray(), list->count(), low, high, comparator );
}

template <class T> void DynaSortList<T>::mergeSort( DynaList<T>* list, IDynaComparator<T*>& comparator ) {
    if ( list != nullptr )
        mergeSort( list->getInternalTypedArray(), list->count(), 0, list->count() - 1, comparator );
}

//===========================================================================
//                             DynaSortArray
//===========================================================================
template <class T> void DynaSortArray<T>::_swap( T* x, int a, int b ) {
    T t    = x[ a ];
    x[ a ] = x[ b ];
    x[ b ] = t;
}

template <class T> void DynaSortArray<T>::_mergeSort( T* src, T* dest, int low, int high, IDynaComparator<T>& comparator ) {
    try {
        int length = high - low;
        if ( length < INSERTION_SORT_THRESHOLD ) {
            for ( int i = low; i < high; ++i )
                for ( int j = i; j > low && comparator.compare( dest[ j - 1 ], dest[ j ] ) > 0; --j )
                    _swap( dest, j, j - 1 );
            return;
        }
        int mid = ( low + high ) >> 1;
        _mergeSort( dest, src, low, mid, comparator );
        _mergeSort( dest, src, mid, high, comparator );
        if ( comparator.compare( src[ mid - 1 ], src[ mid ] ) <= 0 ) {
            memcpy(dest + low, src + low, length * sizeof(T));
            return;
        }
        for( int d = low, l = low, m = mid; d < high; ++d ) {
            if ( m >= high || ( l < mid && comparator.compare( src[ l ], src[ m ] ) <= 0 ) )
                dest[ d ] = src[ l++ ];
            else
                dest[ d ] = src[ m++ ];
        }
    }
    catch ( Exception e ) {
    }
}

template <class T> void DynaSortArray<T>::mergeSort( T* array, int length, int low, int high, IDynaComparator<T>& comparator ) {
    if ( array != nullptr && length > 1 && low >= 0 && high < length ) {
        auto copy = DynaAllocArray<T>::newArray(length);
        memcpy(copy, array, length * sizeof(T));
        _mergeSort( copy, array, low, high + 1, comparator );
        delete copy;
    }
}

template <class T> void DynaSortArray<T>::mergeSort( T* array, int length, IDynaComparator<T>& comparator ) {
    mergeSort( array, length, 0, length - 1, comparator );
}

template <class T> void DynaSortArray<T>::mergeSort( DynaArray<T>* array, int low, int high, IDynaComparator<T>& comparator ) {
    if ( array != nullptr )
        mergeSort( array->getInternalTypedArray(), array->count(), low, high, comparator );
}

template <class T> void DynaSortArray<T>::mergeSort( DynaArray<T>* array, IDynaComparator<T>& comparator ) {
    if ( array != nullptr )
        mergeSort( array->getInternalTypedArray(), array->count(), 0, array->count() - 1, comparator );
}

#endif
