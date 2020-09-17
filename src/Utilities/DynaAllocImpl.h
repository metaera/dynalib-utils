//
// Created by Ken Kopelson on 18/10/17.
//

#ifndef DYNAALLOCIMPL_H
#define DYNAALLOCIMPL_H

#include <iostream>
#include <type_traits>
using namespace std;
#include "BitManip.h"
#include "DynaAlloc.h"
#include "CheckForError.h"

template <class T> T* DynaAllocArray<T>::newArray(uint count) {
    return reallocArray(nullptr, 0, count);
}

template <class T> T* DynaAllocArray<T>::reallocArray(T* array, uint oldCount, uint newCount) {
    if (newCount != oldCount || array == nullptr) {
        try {
            if (array == nullptr) {
                array = new T[newCount];
                memset((void*)array, 0, newCount * sizeof(T));
            }
            else if (newCount == 0) {
                array = deleteArray(array);
            }
            else {
                T*   newArray  = new T[newCount];
                uint copyCount = newCount > oldCount ? oldCount : newCount;
                memcpy(newArray, array, copyCount * sizeof(T));

                delete[] array;
                array = newArray;
                if (newCount > oldCount)
                    memset((array + oldCount), 0, (newCount - oldCount) * sizeof(T));
            }
        }
        catch (std::exception& e) {
        }
    }
    return array;
}

template <class T> T* DynaAllocArray<T>::deleteArray(T* array) {
    delete[] array;
    array = nullptr;
    return array;
}

template <class T> T* DynaAllocArray<T>::clearArray(T* array, uint count) {
    memset((void*)array, 0, count * sizeof(T));
    return array;
}

template <class T> T** DynaAllocVect<T>::newVect(uint count) {
    return reallocVect(nullptr, 0, count, true);
}

template <class T> T** DynaAllocVect<T>::reallocVect(T** array, uint oldCount, uint newCount, bool isOwner) {
    if (newCount != oldCount || array == nullptr) {
        try {
            if (array == nullptr) {
                array = new T*[newCount];
                memset(array, 0, newCount * sizeof(T*));
            }
            else if (newCount == 0) {
                array = deleteVect(array, oldCount, isOwner);
            }
            else {
                T**  newArray  = new T*[newCount];
                uint copyCount = newCount > oldCount ? oldCount : newCount;
                memcpy(newArray, array, copyCount * sizeof(T*));
                if (isOwner && (newCount < oldCount)) {
                    /**
                     * Delete any items that are past the new list size
                     */
                    for (uint idx = newCount; idx < oldCount; ++idx) {
                        delete array[idx];
                    }
                }
                /**
                 * Zero out all the pointers from the old array so that deleting it will
                 * not invoke any destructors, since we have already invoked them just above
                 * if we are the owners of the members.
                 */
                memset(array, 0, oldCount * sizeof(T*));
                delete[] array;
                
                if (newCount > oldCount) {
                    memset((newArray + oldCount), 0, (newCount - oldCount) * sizeof(T*));
                }
                array = newArray;
            }
        }
        catch (std::exception& e) {
        }
    }
    return array;
}

template <class T> T** DynaAllocVect<T>::deleteVect(T** array, uint arrayCount, bool isOwner) {
    if (isOwner) {
        for (uint idx = 0; idx < arrayCount; ++idx) {
            delete array[idx];
        }
    }
    memset(array, 0, arrayCount * sizeof(T*));
    delete[] array;
    array = nullptr;
    return array;
}

template <class T> uint DynaAllocArray<T>::ALLOC_UNITS = 10;

template <class T> uint DynaAllocVect<T>::ALLOC_UNITS = 10;

#endif //DYNAALLOCIMPL_H
