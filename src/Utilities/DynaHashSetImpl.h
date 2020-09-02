//
// Created by Ken Kopelson on 27/12/17.
//

#ifndef DYNAHASHSETIMPL_H
#define DYNAHASHSETIMPL_H

#include <iostream>
#include <type_traits>
using namespace std;
#include "BitManip.h"
#include "DynaAllocImpl.h"
#include "DynaHashSet.h"
#include "CheckForError.h"

#define MAKE_SETTYPE_INSTANCE(E, T) \
    template class DynaHashSet<E>; \
    typedef DynaHashSet<E> T##Set

//===========================================================================
//                         Private/Protected Methods
//===========================================================================
template <typename V> void DynaHashSet<V>::_init(int size) {
    _table = new SetEntry<V>*[size];
    memset((void*)_table, 0, size * sizeof(SetEntry<V>*));
}

template <typename V> int DynaHashSet<V>::_getHashCode(const V* value) const {
    int hash = 0;
    try {
        hash = value->hashCode();
    }
    catch (Exception& e) {
        hash = 0;
    }
    return hash;
}

template <typename V> void DynaHashSet<V>::_reHash(int newCapacity) {
    int oldCapacity = _capacity;
    auto** newTable = new SetEntry<V>*[newCapacity];
    memset((void*)newTable, 0, newCapacity * sizeof(SetEntry<V>*));
    for (int idx = 0; idx < oldCapacity; ++idx) {
        SetEntry<V>* obj = _table[idx];
        if (obj == nullptr || obj == deletedObject)
            continue;
        int hash   = _getHashCode(obj->getValue());
        int index  = ( hash & 0x7FFFFFF ) % newCapacity;
        int offset = 1;
        while (newTable[index] != nullptr) {
            index  = ((index + offset) & 0x7FFFFFFF) % newCapacity;
            offset = offset * 2 + 1;
            if (offset == -1)
                offset = 2;
        }
        newTable[index] = obj;
    }
    delete _table;
    _table     = newTable;
    _capacity  = newCapacity;
    _freeCells = newCapacity - _count;
}

//===========================================================================
//                         Constructors/Destructor
//===========================================================================
/**
 * Entry Constructor
 * @tparam K
 * @tparam V
 * @param key
 * @param value
 */
template <typename V> SetEntry<V>::SetEntry(V* value, bool ownsValue) :
        _value(value), _ownsValue(ownsValue) {
}

template <typename V> SetEntry<V>::SetEntry(const SetEntry<V>& other) {
    _ownsValue = other._ownsValue;
    if (_ownsValue) {
        if (other._value != nullptr) {
            try {
                _value = (V*) (other._value)->copy();
            }
            catch (Exception& e) {
                _value     = other._value;
                _ownsValue = false;
            }
        }
    }
    else {
        _value = other._value;
    }
}

template <typename V> SetEntry<V>::~SetEntry() {
    if (_ownsValue && (_value != DynaHashSet<V>::nullObject)) {
        delete _value;
    }
}

template <typename V> SetEntry<V>* SetEntry<V>::copy() {
    return new SetEntry(*this);
};


/**
 * DynaHashMap Constructor
 * @tparam K
 * @tparam V
 */
template <typename V> DynaHashSet<V>::DynaHashSet() :
        _count(0), _modCount(0), _capacity(INITIAL_SIZE), _freeCells(INITIAL_SIZE), _ownsMembers(true) {
    _init(INITIAL_SIZE);
}

template <typename V> DynaHashSet<V>::DynaHashSet(int size) :
        _count(0), _modCount(0), _capacity(size), _freeCells(size), _ownsMembers(true) {
    _init(size);
}

template <typename V> DynaHashSet<V>::~DynaHashSet() {
    for (int i = 0; i < _capacity; ++i) {
        SetEntry<V>* entry = _table[i];
        if (entry != nullptr) {
            if (entry != DynaHashSet<V>::deletedObject)
                delete entry;
            _table[i] = nullptr;
        }
    }
    delete[] _table;
}

template <typename V> DynaHashSet<V>::DynaHashSet(const DynaHashSet<V>& other) {
    SetEntry<V>* entry;
    _count       = other._count;
    _capacity    = other._capacity;
    _modCount    = other._modCount;
    _freeCells   = other._freeCells;
    _ownsMembers = other._ownsMembers;
    _table       = new SetEntry<V>*[other._capacity];
    for (int i = 0, len = other._capacity; i < len; ++i) {
        entry = other._table[i];
        if (entry != nullptr) {
            try {
                _table[i] = entry->copy();
            }
            catch (Exception& e) {
            }
        }
    }
}

template <typename V> DynaHashSet<V>* DynaHashSet<V>::copy() {
    return new DynaHashSet<V>(*this);
};

template <typename V> SetIter<V>::SetIter(const DynaHashSet<V>* set, int start) : _curIndex(start), _set(set) {
    SetEntry<V>** table = _set->_table;
    for (; hasNext() && (table[_curIndex] == nullptr || table[_curIndex] == _set->deletedObject); ++_curIndex);
}

template <typename V> int DynaHashSet<V>::_getTableIndex(V* value) {
    if (value == nullptr)
        value = nullObject;
    int hash   = _getHashCode(value);
    int index  = ( hash & 0x7FFFFFF ) % _capacity;
    int offset = 1;

    SetEntry<V>* entry = _table[index];
    while ( entry != nullptr &&
            (entry == deletedObject || !((_getHashCode(entry->getValue()) == hash) && (*(entry->getValue()) == *value)))) {
        index  = ((index + offset) & 0x7FFFFFFF) % _capacity;
        entry  = _table[index];
        offset = offset * 2 + 1;
        if (offset == -1)
            offset = 2;
    }
    return index;
}


//===========================================================================
//                             Public Methods
//===========================================================================
/**
 * Entry SetValue setter
 * @tparam V
 * @param newValue
 * @return
 */
template <typename V> const V* SetEntry<V>::setValue(V* newValue) {
    V* oldValue = _value;
    _value = newValue;
    return oldValue;
}
template <typename V> bool SetEntry<V>::operator== (const SetEntry<V>& other) const {
    V* v1 = getValue();
    V* v2 = other.getValue();
    return v1 == v2 || (v1 != nullptr && *v1 == *v2);
}

/**
 * DynaHashMap get Entry based on Key
 * @tparam K
 * @tparam V
 * @param key
 * @return
 */
template <typename V> SetEntry<V>* DynaHashSet<V>::getEntry(V* value) {
    int index = _getTableIndex(value);
    return _table[index];
};

template <typename V> SetEntry<V>* DynaHashSet<V>::getEntry(V value) {
    return getEntry(&value);
}

template <typename V> bool DynaHashSet<V>::add(V* value) {
    if (value == nullptr)
        value = nullObject;
    int hash   = _getHashCode(value);
    int index  = ( hash & 0x7FFFFFF ) % _capacity;
    int offset = 1;
    int deletedIdx = -1;

    SetEntry<V>* entry = _table[index];
    while ( entry != nullptr &&
            (entry == deletedObject || !((_getHashCode(entry->getValue()) == hash) && (*(entry->getValue()) == *value)))) {
        if (entry == deletedObject)
            deletedIdx = index;

        index  = ((index + offset) & 0x7FFFFFFF) % _capacity;
        entry  = _table[index];
        offset = offset * 2 + 1;
        if (offset == -1)
            offset = 2;
    }

    if (entry == nullptr) {
        if (deletedIdx != -1)
            index = deletedIdx;
        else
            --_freeCells;
        ++_modCount;
        ++_count;
        _table[index] = new SetEntry<V>(value, _ownsMembers);

        if (1 - (_freeCells / (double)_capacity) > LOAD_FACTOR) {
            _reHash(_capacity);
            if (1 - (_freeCells / (double)_capacity) > LOAD_FACTOR) {
                _reHash(_capacity * 2 + 1);
            }
        }
    }
    else {
        return false;
    }
    return true;
}

template <typename V> bool DynaHashSet<V>::add(V value) {
    return add(&value);
}

template <typename V> V* DynaHashSet<V>::remove(V* value) {
    int index = _getTableIndex(value);

    if (_table[index] != nullptr && _table[index] != deletedObject) {
        V* saveValue = _table[index]->getValue();
        _table[index]->setValue(nullptr);   // Prevent the value object from being deleted
        delete _table[index];               // delete the entry from the table
        _table[index] = deletedObject;      // Mark the table entry as deleted
        ++_modCount;
        --_count;
        return saveValue;
    }
    else
        return nullptr;
}

template <typename V> V* DynaHashSet<V>::remove(V value) {
    return remove(&value);
}

template <typename V> void DynaHashSet<V>::deleteEntry(V* value) {
    int index = _getTableIndex(value);

    if (_table[index] != nullptr && _table[index] != deletedObject) {
        delete _table[index];               // delete the entry from the table
        _table[index] = deletedObject;      // Mark the table entry as deleted
        ++_modCount;
        --_count;
    }
}

template <typename V> void DynaHashSet<V>::deleteEntry(V value) {
    return deleteEntry(&value);
}

template <typename V> SetEntry<V>* DynaHashSet<V>::removeEntry(SetEntry<V>* ent) {
    if (ent == nullptr)
        return nullptr;
    int index = _getTableIndex(ent->getValue());

    if (_table[index] != nullptr && _table[index] != deletedObject) {
        SetEntry<V>* saveEntry = _table[index];
        _table[index] = deletedObject;
        ++_modCount;
        --_count;
        return saveEntry;
    }
    else
        return nullptr;
}


template <typename V> void DynaHashSet<V>::clear() {
    for (int idx = 0; idx < _capacity; ++idx) {
        SetEntry<V>* entry = _table[idx];
        if (entry != nullptr) {
            if (entry != DynaHashSet<V>::deletedObject) {
                delete entry;
                --_count;
            }
            _table[idx] = nullptr;
        }
    }
    _count     = 0;
    _freeCells = _capacity;
    ++_modCount;
}

template <typename V> DynaSetIter<V> DynaHashSet<V>::begin() {
    return DynaSetIter<V>( this, 0 );
}

template <typename V> DynaSetIter<V> DynaHashSet<V>::end() {
    return DynaSetIter<V>( this, _capacity );
}

template <typename V> SetIter<V> DynaHashSet<V>::values() {
    return SetIter<V>(this);
};

template <typename V> const SetIter<V>& SetIter<V>::operator++ () {
    SetEntry<V>** table = _set->_table;
    for (++_curIndex; hasNext() &&
                      (table[_curIndex] == nullptr || table[_curIndex] == _set->deletedObject); ++_curIndex);
    return *this;
}

template <typename V> const SetIter<V>& SetIter<V>::operator-- () {
    SetEntry<V>** table = _set->_table;
    for (--_curIndex; hasPrev() &&
                      (table[_curIndex] == nullptr || table[_curIndex] == _set->deletedObject); --_curIndex);
    return *this;
}

//===========================================================================
//                          Static Initialization
//===========================================================================
template <typename V> SetObject<V>  DynaHashSet<V>::nullObjectInstance = SetObject<V>();
template <typename V> V* DynaHashSet<V>::nullObject                    = (V*)(void*)(&DynaHashSet<V>::nullObjectInstance);

template <typename V> SetEntry<V> DynaHashSet<V>::deletedObjectInstance = SetEntry<V>(DynaHashSet<V>::nullObject, false);
template <typename V> SetEntry<V>* DynaHashSet<V>::deletedObject        = &deletedObjectInstance;

#endif //DYNAHASHSETIMPL_H
