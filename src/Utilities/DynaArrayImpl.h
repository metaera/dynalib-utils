//
// Created by Ken Kopelson on 15/10/17.
//

/**
 * Dyna Array Implementation
 *
 * This file includes the implementation of the DynaArray template.  This file should be included in any .cpp file
 * that will instantiate a new type of DynaArray.  The regular "DynaArray.h" file is then included everywhere a
 * DynaArray is used.
 */

#ifndef DYNAARRAYIMPL_H
#define DYNAARRAYIMPL_H

#include <iostream>
#include <type_traits>
using namespace std;
#include "BitManip.h"
#include "DynaAllocImpl.h"
#include "DynaArray.h"
#include "CheckForError.h"
#include "Exception.h"

#define MAKE_ARRAYTYPE_INSTANCE(C, T) \
    template class DynaAllocArray<C>; \
    template class DynaArray<C>; \
    typedef DynaAllocArray<C> T##AllocArray; \
    typedef DynaArray<C> T##Array

//===========================================================================
//                              Static Methods
//===========================================================================

//===========================================================================
//                         Constructors/Destructor
//===========================================================================
template <class T> DynaArray<T>::DynaArray(uint initCapacity) {
    adjustSize(initCapacity);
}

template <class T> DynaArray<T>::DynaArray(T* array, int count) {
    _insertArray(array, 0, 0, count);
}

template <class T> DynaArray<T>::~DynaArray() {
    DynaAllocArray<T>::deleteArray(_members);
    _members = nullptr;
}

template <class T> DynaArray<T>::DynaArray(const DynaArray<T>& other) {
    _flags = other._flags;
    _insertArray(other._members, 0, 0, other._count);
}

template<class T> DynaArray<T>* DynaArray<T>::copy() {
    return new DynaArray<T>(*this);
}

//===========================================================================
//                             Private Methods
//===========================================================================
template <class T> void DynaArray<T>::_insertArray(T* array, int arrayOffset, int toIndex, int count) {
    if ((_count + count) >= _capacity)
        adjustSizeLog(count);
    if (toIndex < _count)
        memmove(_members + toIndex + count, _members + toIndex, (_count - toIndex) * sizeof(T));
    _count += count;
    memmove(_members + toIndex, array + arrayOffset, count * sizeof(T));
}

template <class T> void DynaArray<T>::_swap(int index1, int index2) {
    T tmp              = _members[ index1 ];
    _members[ index1 ] = _members[ index2 ];
    _members[ index2 ] = tmp;
}

template <class T> DynaArray<T>* DynaArray<T>::_copyRange(int frIndex, int toIndex) {
    int range = toIndex - frIndex + 1;
    auto* newArray = new DynaArray<T>(range);
    newArray->setAllFlags(_flags);
    newArray->_insertArray(getInternalTypedArray(), frIndex, 0, range);
    return newArray;
}

template <class T> void DynaArray<T>::_deleteExcessCapacity() {
    if ((_capacity - _count) >= DynaAllocArray<T>::getAllocUnits())
        adjustSize(0);
}

template <class T> void DynaArray<T>::_deleteRange(int frIndex, int toIndex) {
    if (toIndex == END_OF_LIST)
        toIndex = _count;
    else
        ++toIndex;
    memmove(_members + frIndex, _members + toIndex, (_count - toIndex) * sizeof(T));
    _count -= (toIndex - frIndex);
    // Zero out any remaining slots at the end of the array
    for (int idx = _count; idx < _capacity; ++idx) {
        _members[idx] = 0;
    }
    if (IS_ALL_SET(_flags, AUTO_TRIM))
        _deleteExcessCapacity();
}

template <class T> void DynaArray<T>::_clearRange(int frIndex, int toIndex) {
    if (toIndex == END_OF_LIST)
        toIndex = _count - 1;
    for (int idx = frIndex; idx <= toIndex; ++idx) {
        _members[idx] = 0;
    }
    SET_BITS(_flags, PACK_NEEDED);
    if (IS_ALL_SET(_flags, AUTO_TRIM))
        _deleteExcessCapacity();
}

template <class T> void DynaArray<T>::_deleteOrClear(int frIndex, int toIndex) {
    if (IS_ALL_SET(_flags, AUTO_PACK))
        _deleteRange(frIndex, toIndex);
    else
        _clearRange(frIndex, toIndex);
}

template <class T> bool DynaArray<T>::_setAutoPack(bool isPack) {
    if (isPack) {
        setFlags(AUTO_PACK);
        pack();
    }
    else
        clearFlags(AUTO_PACK);
    return isPack;
}

template <class T> bool DynaArray<T>::_setAutoTrim(bool isTrim) {
    if (isTrim) {
        setFlags(AUTO_TRIM);
        trim();
    }
    else
        clearFlags(AUTO_TRIM);
    return isTrim;
}

template <class T> void DynaArray<T>::setAutoPackTrim(bool pack, bool trim) {
    if (!_setAutoPack(pack))
        _setAutoTrim(trim);
}

//===========================================================================
//                              Public Methods
//===========================================================================
template <class T> DynaArray<T>& DynaArray<T>::operator= (const DynaArray<T>& array) {
    if (this == &array)
        return *this;

    // do the copy
    // return the existing object so we can chain this operator
    return *this;
}

template <class T> DynaArray<T>& DynaArray<T>::operator+=(T value) {
    append(value);
    return *this;
};

template <class T> DynaArray<T>& DynaArray<T>::operator+=(T& value) {
    append(value);
    return *this;
}

template <class T> DynaArray<T>& DynaArray<T>::operator-=(int index) {
    deleteItem(index);
    return *this;
};

template <class T> DynaArray<T>& DynaArray<T>::operator<<(T value) {
    append(value);
    return *this;
}

template <class T> DynaArray<T>& DynaArray<T>::operator<<(T& value) {
    append(value);
    return *this;
}

template <class T> void DynaArray<T>::setCapacity(uint newCapacity) {
    if (newCapacity != _capacity) {
        _members  = DynaAllocArray<T>::reallocArray(_members, _capacity, newCapacity);
        _capacity = newCapacity;
        if (_count > _capacity)
            _count = _capacity;
    }
}

template <class T> void DynaArray<T>::determineTheCount(int invalidValue) {
    _count     = 0;
    bool found = false;
    for (int i = 0; i < _capacity; ++i) {
        if (_members[i] == invalidValue) {
            found  = true;
            _count = (uint)i;
            break;
        }
    }
    if (!found) {
      _count = _capacity;
    }
}

// template <class T> void DynaArray<T>::determineTheCountFromEnd(int invalidValue) {
//     _count     = _capacity;
//     bool found = false;
//     for (int i = _capacity - 1; i >= 0; --i) {
//         if (!found) {
//             if (_members[i] == invalidValue) {
//                 found  = true;
//                 _count = (uint)i;
//             }
//         }
//         else {
//             if (_members[i] != invalidValue)
//                 break;
//             _count = (uint)i;
//         }
//     }
// }

template <class T> void DynaArray<T>::setCount(uint count) {
    if (count != _count) {
        if (count > _capacity)
            count = _capacity;
        _count = count;
    }
}

template <class T> void DynaArray<T>::adjustSize(int delta) {
    setCapacity(_count + delta);
}

template <class T> void DynaArray<T>::adjustSizeLog(int delta) {
    setCapacity(((_count + delta) * 3 / 2) + 1);
}

template <class T> void DynaArray<T>::pack() {
    for (int idx = _count - 1; idx >= 0; --idx) {
        if (_members[idx] == 0)
            remove(idx);
    }
}

template <class T> void DynaArray<T>::clear() {
    if (_count > 0)
        deleteItems(0, _count - 1);
}

template <class T> int DynaArray<T>::indexOf(T value, uint start, uint step) {
    uint cnt = getCount();
    for (uint i = start; i < cnt; i += step) {
        if (_members[i] == value)
            return i;
    }
    return END_OF_LIST;
}

template <class T> int DynaArray<T>::indexOf(T* value, uint arraySize, uint start, uint step) {
    uint cnt = getCount();
    for (uint i = start; i < cnt; i += step) {
        for (uint j = 0; j < arraySize; ++j) {
            if (_members[i] == value[j])
                return i;
        }
    }
    return END_OF_LIST;
}

template <class T> int DynaArray<T>::indexOf(T value) {
    return indexOf(value, 0, 1);
}

template <class T> int DynaArray<T>::indexOf(T* value, uint arraySize) {
    return indexOf(value, arraySize, 0, 1);
}

template <class T> int  DynaArray<T>::revIndexOf(T value, uint start, uint step) {
    int last = getCount() - 1;
    for (int i = last - start; i >= 0; i -= step) {
        if (_members[i] == value)
            return i;
    }
    return END_OF_LIST;
}

template <class T> int  DynaArray<T>::revIndexOf(T* value, uint arraySize, uint start, uint step) {
    int last = getCount() - 1;
    for (int i = last - start; i >= 0; i -= step) {
        for (int j = arraySize - 1; j >= 0; --j) {
            if (_members[i] == value[j])
                return i;
        }
    }
    return END_OF_LIST;
}

template <class T> int DynaArray<T>::revIndexOf(T value) {
    return revIndexOf(value, 0, 1);
}

template <class T> int DynaArray<T>::revIndexOf(T* value, uint arraySize) {
    return revIndexOf(value, arraySize, 0, 1);
}

template <class T> int DynaArray<T>::insert(int index, T value) {
    if (index == END_OF_LIST)
        index = _count;
    else
        CheckForError::assertInBounds(index, _count);
    if (_count >= _capacity)
        adjustSizeLog(1);
    if (index < _count)
        memmove(_members + index + 1, _members + index, (_count - index) * sizeof(T));
    ++_count;
    _members[index] = value;
    return index;
}

template <class T> int DynaArray<T>::insert(int index, T* value) {
    return insert(index, *value);
}

template <class T> int DynaArray<T>::insert(int index, T* values, uint length) {
    return 0;
}

template <class T> int DynaArray<T>::insert(int index, uint8_t* values, uint offset, uint length) {
    return 0;
}

template <class T> int DynaArray<T>::insert(int index, DynaArray<T>* array) {
    return 0;
}

template <class T> int DynaArray<T>::append(T value) {
    return insert(END_OF_LIST, value);
}

template <class T> int DynaArray<T>::append(T* values, uint length) {
    return insert(END_OF_LIST, values, length);
}

template <class T> int DynaArray<T>::append(uint8_t* values, uint offset, uint length) {
    return insert(END_OF_LIST, values, offset, length);
}

template <class T> int DynaArray<T>::append(DynaArray<T>* array) {
    return insert(END_OF_LIST, array);
}

template <class T> T DynaArray<T>::setValue(int index, T value) {
    CheckForError::assertInBounds(index, _count - 1);
    T oldItem = _members[index];
    _members[index] = value;
    return oldItem;
}

template <class T> void DynaArray<T>::move(int index, int destIndex) {
    CheckForError::assertInBounds(index, _count - 1);
    CheckForError::assertInBounds(destIndex, _count);
    if (index != destIndex) {
        T item = _members[index];
        if (destIndex < index) {
            int moveCount = index - destIndex;
            if (moveCount == 1)
                _swap(index, destIndex);
            else {
                memmove(_members + destIndex + 1, _members + destIndex, (index - destIndex) * sizeof(T));
                _members[destIndex] = item;
            }
        }
        else if (index < --destIndex){
            int moveCount = destIndex - index;
            if (moveCount == 1)
                _swap(index, destIndex);
            else {
                memmove(_members + index, _members + index + 1, (destIndex - index) * sizeof(T));
                _members[destIndex] = item;
            }
        }
    }
}

template <class T> void DynaArray<T>::slide(int frIndex, int toIndex) {
    CheckForError::assertInBounds( frIndex, _count - 1 );
    CheckForError::assertInBounds( toIndex, _count - 1 );
    if ( frIndex != toIndex ) {
        T item = _members[ frIndex ];
        if ( frIndex < toIndex ) {
            memmove(_members + frIndex, _members + frIndex + 1, (toIndex - frIndex) * sizeof(T));
            _members[ toIndex ] = item;
        }
        else {
            memmove( _members + toIndex + 1, _members + toIndex, (frIndex - toIndex) * sizeof(T));
            _members[ toIndex ] = item;
        }
    }
}

template <class T> T DynaArray<T>::remove(int index) {
    CheckForError::assertInBounds(index, _count - 1);
    T item = _members[index];
    _deleteOrClear(index, index);
    return item;
}

template <class T> DynaArray<T>* DynaArray<T>::remove(int frIndex, int toIndex) {
    CheckForError::assertInBounds(frIndex, toIndex);
    CheckForError::assertInBounds(toIndex, _count - 1);
    DynaArray<T>* newArray = _copyRange(frIndex, toIndex);
    _deleteOrClear(frIndex, toIndex);
    return newArray;
}

template <class T> void DynaArray<T>::deleteItem(int index) {
    CheckForError::assertInBounds(index, _count - 1);
    _deleteOrClear(index, index);
}

template <class T> void DynaArray<T>::deleteItems(int frIndex, int toIndex) {
    CheckForError::assertInBounds(frIndex, toIndex);
    CheckForError::assertInBounds(toIndex, _count - 1);
    _deleteOrClear(frIndex, toIndex);
}

template <class T> void DynaArray<T>::push(T value) {
    insert(0, value);
}

template <class T> T DynaArray<T>::pop() {
    if (_count > 0)
        return remove(0);
    else
        throw EmptyStackException();
}

template <class T> T DynaArray<T>::popLast() {
    if (_count > 0)
        return remove(_count - 1);
    else
        throw EmptyStackException();
}

template <class T> DynaArrayIter<T> DynaArray<T>::begin () const {
    return DynaArrayIter<T>( this, 0 );
}

template <class T> DynaArrayIter<T> DynaArray<T>::end () const {
    return DynaArrayIter<T>( this, _count );
}


#endif //DYNAARRAYIMPL_H
