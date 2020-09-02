/**
 * @file DynaSort.h
 * @author Ken Kopelson (ken@metaera.com)
 * @brief 
 * @version 0.1
 * @date 2020-08-20
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef DYNASORT_H
#define DYNASORT_H

#include "DynaList.h"
#include "DynaArray.h"
#include "DynaAlloc.h"
#include "IDynaComparator.h"

/**
 * @brief Any array or list that has a number of items less than the threshold will automatically
 * use the insertion sort, instead of the merge sort. 
 * 
 */
static const int INSERTION_SORT_THRESHOLD = 7;


/**
 * @brief Convenience Macro to declare a DynaSortList object, with matching allocator
 * 
 */
#define MAKE_SORTLISTTYPE_DEF(C, T) \
    typedef DynaAllocVect<C> T##AllocVect; \
    typedef DynaSortList<C> T##SortList

/**
 * @brief Convenience Macro to declare a DynaSortArray object, with matching allocator
 * 
 */
#define MAKE_SORTARRAYTYPE_DEF(C, T) \
    typedef DynaAllocArray<C> T##AllocArray; \
    typedef DynaSortArray<C> T##SortArray

template <class T> class DynaSortList {
protected:
    static void _mergeSort( T** src, T** dest, int low, int high, IDynaComparator<T*>& comparator );
    static void _swap( T** x, int a, int b );

public:
    /**
     * @brief Sort an array of objects, typically those allocated with "new"
     * 
     * @param array of pointers to objects
     * @param length number of pointers in the array
     * @param low starting index (0-based)
     * @param high ending index (must be less than length)
     * @param comparator reference to the comparator function
     */
    static void mergeSort( T** array, int length, int low, int high, IDynaComparator<T*>& comparator );
    static void mergeSort( T** array, int length, IDynaComparator<T*>& comparator );

    /**
     * @brief Sort the objects in a DynaList (vector)
     * 
     * @param list pointer to the DynaList object
     * @param low starting index (0-based)
     * @param high ending index (must be less than length)
     * @param comparator reference to the comparator function
     */
    static void mergeSort( DynaList<T>* list, int low, int high, IDynaComparator<T*>& comparator );
    static void mergeSort( DynaList<T>* list, IDynaComparator<T*>& comparator );
};

template <class T> class DynaSortArray {
protected:
    static void _mergeSort( T* src, T* dest, int low, int high, IDynaComparator<T>& comparator );
    static void _swap( T* x, int a, int b );

public:
    /**
     * @brief Sort an array of scalar values
     * 
     * @param array of scalar values, e.g. int, long, bool
     * @param length number of scalar values in the array
     * @param low starting index (0-based)
     * @param high ending index (must be less than length)
     * @param comparator reference to the comparator function
     */
    static void mergeSort( T* array, int length, int low, int high, IDynaComparator<T>& comparator );
    static void mergeSort( T* array, int length, IDynaComparator<T>& comparator );

    /**
     * @brief Sort the scalar values in a DynaArray
     * 
     * @param array pointer to the DynaArray object
     * @param low starting index (0-based)
     * @param high ending index (must be less than length)
     * @param comparator reference to the comparitor function
     */
    static void mergeSort( DynaArray<T>* array, int low, int high, IDynaComparator<T>& comparator );
    static void mergeSort( DynaArray<T>* array, IDynaComparator<T>& comparator );
};


#endif