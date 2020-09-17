/**
 * @file DynaListImpl.h
 * @author Ken Kopelson (ken@metaera.com)
 * @brief 
 * @version 0.1
 * @date 2020-01-10
 * 
 * @copyright Copyright (c) 2020
 * 
 */

/**
 * Dyna Array Implementation
 *
 * This file includes the implementation of the DynaList template.  This file should be included in any .cpp file
 * that will instantiate a new type of DynaList.  The regular "DynaList.h" file is then included everywhere a
 * DynaList is used.
 */

#ifndef DYNALISTIMPL_H
#define DYNALISTIMPL_H

#include <iostream>
#include <type_traits>

#include "BitManip.h"
#include "DynaAllocImpl.h"
#include "DynaList.h"
#include "CheckForError.h"
#include "Exception.h"

#define MAKE_LISTTYPE_INSTANCE(C, T) \
    template class DynaAllocVect<C>; \
    template class DynaList<C>; \
    typedef DynaAllocVect<C> T##AllocVect; \
    typedef DynaList<C> T##List

//===========================================================================
//                              Static Methods
//===========================================================================

//===========================================================================
//                         Constructors/Destructor
//===========================================================================
template <class T> DynaList<T>::DynaList() {
    adjustSize(0);
}

template <class T> DynaList<T>::DynaList(uint initCapacity) {
    adjustSize(initCapacity);
}

template <class T> DynaList<T>::DynaList(T* array, int count) {
    _insertArray(&array, 0, 0, count);
}

template <class T> DynaList<T>::~DynaList() {
    clear();
    _members = DynaAllocVect<T>::deleteVect(_members, _count, isFlags(OWNS_MEMBERS));
}

template <class T> DynaList<T>::DynaList(const DynaList<T>& other) {
    T* item;
    _count     = other._count;
    _capacity  = other._capacity;
    _flags     = other._flags;
    _members   = new T*[other._capacity];
    for (int i = 0, len = other._capacity; i < len; ++i) {
        item = other._members[i];
        if (item != nullptr) {
            try {
                T* newItem = item->copy();
                _members[i] = newItem;
            }
            catch (Exception& e) {
            }
        }
    }
}

template <class T> DynaList<T>* DynaList<T>::copy() {
    return new DynaList<T>(*this);
};

//===========================================================================
//                             Private Methods
//===========================================================================
template <class T> void DynaList<T>::_insertArray(T** array, int arrayOffset, int toIndex, int count) {
    if ((_count + count) >= _capacity)
        adjustSizeLog(count);
    if (toIndex < _count)
        memmove(_members + toIndex + count, _members + toIndex, (_count - toIndex) * sizeof(T*));
    _count += count;
    memmove(_members + toIndex, array + arrayOffset, count * sizeof(T*));
}

template <class T> void DynaList<T>::_swap(int index1, int index2) {
    T* tmp             = _members[ index1 ];
    _members[ index1 ] = _members[ index2 ];
    _members[ index2 ] = tmp;
}

template <class T> void DynaList<T>::_trimTheCount() {
    int idx = _count - 1;
    for (; idx >= 0; --idx) {
        if (_members[idx] != nullptr)
            break;
    }
    _count = idx + 1;
}

template <class T> DynaList<T>* DynaList<T>::_copyRange(int frIndex, int toIndex) {
    int range = toIndex - frIndex + 1;
    auto* newList = new DynaList<T>(range);
    newList->setAllFlags(_flags);
    newList->_insertArray(getInternalTypedArray(), frIndex, 0, range);
    return newList;
}

template <class T> void DynaList<T>::_deleteExcessCapacity() {
    if ((_capacity - _count) >= DynaAllocVect<T>::getAllocUnits())
        adjustSize(0);
}

template <class T> void DynaList<T>::_deleteRange(int frIndex, int toIndex) {
    if (toIndex == END_OF_LIST)
        toIndex = _count;
    else
        ++toIndex;
    // First go through and delete the items in the range
    if (isFlags(OWNS_MEMBERS)) {
        for (int idx = frIndex; idx < toIndex; ++idx) {
            delete _members[idx];
        }
    }
    // Then slide everything down over the items in the range
    if (_count > toIndex) {
        memmove(_members + frIndex, _members + toIndex, (_count - toIndex) * sizeof(T*));
    }
    _count -= (toIndex - frIndex);
    // Null out any remaining slots at the end of the list (the pointers will be in the list twice otherwise)
    for (int idx = _count; idx < _capacity; ++idx) {
        _members[idx] = nullptr;
    }
    if (isFlags(AUTO_TRIM))
        _deleteExcessCapacity();
}

template <class T> void DynaList<T>::_clearRange(int frIndex, int toIndex) {
    if (toIndex == END_OF_LIST)
        toIndex = _count - 1;
    if (isFlags(OWNS_MEMBERS)) {
        for (int idx = frIndex; idx <= toIndex; ++idx) {
            delete _members[idx];
            _members[idx] = nullptr;
        }
    }
    else {
        for (int idx = frIndex; idx <= toIndex; ++idx) {
            _members[idx] = nullptr;
        }
    }
    setFlags(PACK_NEEDED);
    if (isFlags(AUTO_TRIM))
        _deleteExcessCapacity();
}

template <class T> void DynaList<T>::_nullRange(int frIndex, int toIndex) {
    if (toIndex == END_OF_LIST)
        toIndex = _count - 1;
    for (int idx = frIndex; idx <= toIndex; ++idx) {
        _members[idx] = nullptr;
    }
    if (isFlags(AUTO_TRIM))
        _deleteExcessCapacity();
}

template <class T> void DynaList<T>::_deleteOrClear(int frIndex, int toIndex) {
    if (isFlags(AUTO_PACK))
        _deleteRange(frIndex, toIndex);
    else
        _clearRange(frIndex, toIndex);
}

template <class T> bool DynaList<T>::_setAutoPack(bool isPack) {
    if (isPack) {
        setFlags(AUTO_PACK);
        pack();
    }
    else
        clearFlags(AUTO_PACK);
    return isPack;
}

template <class T> bool DynaList<T>::_setAutoTrim(bool isTrim) {
    if (isTrim) {
        setFlags(AUTO_TRIM);
        trim();
    }
    else
        clearFlags(AUTO_TRIM);
    return isTrim;
}

template <class T> DynaList<T>* DynaList<T>::setAutoPackTrim(bool pack, bool trim) {
    if (!_setAutoPack(pack))
        _setAutoTrim(trim);
    return this;
}

template <class T> DynaList<T>* DynaList<T>::setOwnsMembers(bool ownsMembers) {
    if (ownsMembers)
        setFlags(OWNS_MEMBERS);
    else
        clearFlags(OWNS_MEMBERS);
    return this;
}

template <class T> int DynaList<T>::_nextFreeSlot() {
    int idx = 0;
    for (; idx < _count; ++idx) {
        if (_members[idx] == nullptr)
            break;
    }
    if ((idx == _count) && (_count >= _capacity))
        adjustSizeLog(1);
    return idx;
}

//===========================================================================
//                              Public Methods
//===========================================================================
template <class T> DynaList<T>& DynaList<T>::operator+=(T* value) {
    append(value);
    return *this;
};

template <class T> DynaList<T>& DynaList<T>::operator+=(T&& value) {
    auto v = new T(value);
    append(v);
    return *this;
}

template <class T> DynaList<T>& DynaList<T>::operator-=(int index) {
    deleteItem(index);
    return *this;
};

template <class T> DynaList<T>& DynaList<T>::operator<<(T* value) {
    append(value);
    return *this;
}

template <class T> DynaList<T>& DynaList<T>::operator<<(T&& value) {
    auto v = new T(value);
    append(v);
    return *this;
}

template <class T> int DynaList<T>::firstNullSlot() {
    for (int idx = 0; idx < _count; ++idx) {
        if (_members[idx] == nullptr)
            return idx;
    }
    return INVALID_INDEX;
}

template <class T> DynaList<T>* DynaList<T>::setCapacity(uint newCapacity) {
    if (newCapacity != _capacity) {
        _members  = DynaAllocVect<T>::reallocVect(_members, (uint)_capacity, newCapacity, isFlags(OWNS_MEMBERS));
        _capacity = newCapacity;
        if (_count > _capacity)
            _count = _capacity;
    }
    return this;
}

template <class T> void DynaList<T>::adjustSize(int delta) {
    setCapacity(_count + delta);
}

template <class T> void DynaList<T>::adjustSizeLog(int delta) {
    setCapacity(((_count + delta) * 3 / 2) + 1);
}

template <class T> void DynaList<T>::trim() {
    _trimTheCount();
    adjustSize(0);
}

template <class T> void DynaList<T>::pack() {
    _trimTheCount();
    for (int idx = _count - 1; idx >= 0; --idx) {
        if (_members[idx] == nullptr)
            remove(idx);
    }
}

template <class T> void DynaList<T>::clear() {
    if (_count > 0)
        deleteItems(0, _count - 1);
}

template <class T> int DynaList<T>::indexOf(T* value, uint start, uint step) {
    uint cnt = getCount();
    for (uint i = start; i < cnt; i += step) {
        if (_members[i] == value)
            return i;
    }
    return END_OF_LIST;
}

template <class T> int DynaList<T>::indexOf(T** value, uint arraySize, uint start, uint step) {
    uint cnt = getCount();
    for (uint i = start; i < cnt; i += step) {
        for (uint j = 0; j < arraySize; ++j) {
            if (_members[i] == value[j])
                return i;
        }
    }
    return -1;
}

template <class T> int DynaList<T>::insert(int index, T* value) {
    if (!isFlags(AUTO_PACK)) {
        if (index == FIRST_FREE)
            index = _nextFreeSlot();
        else {
            if (index == END_OF_LIST)
                index = _count;
            if (index >= _capacity)
                adjustSizeLog(index - _capacity + 1);
            CheckForError::assertValidArg(_members[index] == nullptr, "Slot " + to_string(index) + " is not null");
        }
        if (index >= _count)
            _count = index + 1;
    }
    else {
        CheckForError::assertNotNull(value);
        if (index == END_OF_LIST)
            index = _count;
        else
            CheckForError::assertInBounds(index, _count);
        if (_count >= _capacity)
            adjustSizeLog(1);
        if (index < _count)
            memmove(_members + index + 1, _members + index, (_count - index) * sizeof(T*));
        ++_count;
    }
    _members[index] = value;
    return index;
}

template <class T> int DynaList<T>::insert(int index, T&& value) {
    return insert(index, &value);
}

template <class T> int DynaList<T>::insert(int index, T** array, uint length) {
    if (length > 0) {
        CheckForError::assertNotNull(array);
        if (index == END_OF_LIST)
            index = _count;
        else
            CheckForError::assertInBounds(index, _count);
        _insertArray(array, 0, index, length);
    }
    return index + length - 1;
}

template <class T> int DynaList<T>::insert(int index, DynaList<T>* list) {
    int length = list != nullptr ? list->getCount() : 0;
    return length > 0 ? insert(index, list->getInternalTypedArray(), (uint)length) : END_OF_LIST;
}

template <class T> int DynaList<T>::append(T* value) {
    return insert(END_OF_LIST, value);
}

template <class T> int DynaList<T>::append(T&& value) {
    return append(&value);
}

template <class T> int DynaList<T>::append(T** array, uint length) {
    return insert(END_OF_LIST, array, length);
}

template <class T> int DynaList<T>::append(DynaList<T>* list) {
    return insert(END_OF_LIST, list);
}

template <class T> T* DynaList<T>::set(int index, T* value) {
    CheckForError::assertInBounds(index, _count - 1);
    T* oldItem = _members[index];
    _members[index] = value;
    return oldItem;
}

template <class T> void DynaList<T>::move(int index, int destIndex) {
    CheckForError::assertInBounds(index, _count - 1);
    CheckForError::assertInBounds(destIndex, _count);
    if (index != destIndex) {
        T* item = _members[index];
        if (destIndex < index) {
            int moveCount = index - destIndex;
            if (moveCount == 1)
                _swap(index, destIndex);
            else {
                memmove(_members + destIndex + 1, _members + destIndex, (index - destIndex) * sizeof(T*));
                _members[destIndex] = item;
            }
        }
        else if (index < --destIndex){
            int moveCount = destIndex - index;
            if (moveCount == 1)
                _swap(index, destIndex);
            else {
                memmove(_members + index, _members + index + 1, (destIndex - index) * sizeof(T*));
                _members[destIndex] = item;
            }
        }
    }
}

template <class T> void DynaList<T>::slide(int frIndex, int toIndex) {
    CheckForError::assertInBounds( frIndex, _count - 1 );
    CheckForError::assertInBounds( toIndex, _count - 1 );
    if ( frIndex != toIndex ) {
        T* item = _members[ frIndex ];
        if ( frIndex < toIndex ) {
            memmove(_members + frIndex, _members + frIndex + 1, (toIndex - frIndex) * sizeof(T*));
            _members[ toIndex ] = item;
        }
        else {
            memmove( _members + toIndex + 1, _members + toIndex, (frIndex - toIndex) * sizeof(T*));
            _members[ toIndex ] = item;
        }
    }
}

template <class T> T* DynaList<T>::remove(int index) {
    CheckForError::assertInBounds(index, _count - 1);
    T* item = _members[index];
    _nullRange(index, index);
    _deleteOrClear(index, index);
    return item;
}

template <class T> DynaList<T>* DynaList<T>::remove(int frIndex, int toIndex) {
    CheckForError::assertInBounds(frIndex, toIndex);
    CheckForError::assertInBounds(toIndex, _count - 1);
    DynaList<T>* newArray = _copyRange(frIndex, toIndex);
    _nullRange(frIndex, toIndex);
    _deleteOrClear(frIndex, toIndex);
    return newArray;
}

template <class T> void DynaList<T>::deleteItem(int index) {
    CheckForError::assertInBounds(index, _count - 1);
    _deleteOrClear(index, index);
}

template <class T> void DynaList<T>::deleteItems(int frIndex, int toIndex) {
    CheckForError::assertInBounds(frIndex, toIndex);
    CheckForError::assertInBounds(toIndex, _count - 1);
    _deleteOrClear(frIndex, toIndex);
}


template <class T> void DynaList<T>::push(T* value) {
    insert(0, value);
}

template <class T> T* DynaList<T>::pop() {
    if (_count > 0)
        return remove(0);
    else
        throw EmptyStackException();
}

template <class T> T* DynaList<T>::popLast() {
    if (_count > 0)
        return remove(_count - 1);
    else
        throw EmptyStackException();
}

template <class T> DynaListIter<T> DynaList<T>::begin () const {
    return DynaListIter<T>( this, 0 );
}

template <class T> DynaListIter<T> DynaList<T>::end () const {
    return DynaListIter<T>( this, _count );
}

#endif //DYNALISTIMPL_H
