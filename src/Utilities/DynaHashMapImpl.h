//
// Created by Ken Kopelson on 20/10/17.
//

#ifndef DYNAHASHMAPIMPL_H
#define DYNAHASHMAPIMPL_H
#include <iostream>
#include <type_traits>
using namespace std;
#include "BitManip.h"
#include "DynaAllocImpl.h"
#include "DynaHashMap.h"
#include "CheckForError.h"
//#include "SharedGlobals.h"

#define MAKE_MAPTYPE_INSTANCE(K, V, T) \
    template class DynaHashMap<K,V>; \
    typedef DynaHashMap<K,V> T##Map

//===========================================================================
//                              Static Methods
//===========================================================================

//===========================================================================
//                         Private/Protected Methods
//===========================================================================
template <typename K, typename V> void DynaHashMap<K,V>::_init(int size) {
    _table = new MapEntry<K,V>*[size];
    memset((void*)_table, 0, size * sizeof(MapEntry<K,V>*));
}

template <typename K, typename V> int DynaHashMap<K,V>::_getHashCode(const K* key) const {
    int hash = 0;
    try {
        hash = key->hashCode();
    }
    catch (Exception& e) {
        hash = 0;
    }
    return hash;
}

template <typename K, typename V> void DynaHashMap<K,V>::_reHash(int newCapacity) {
    int oldCapacity = _capacity;
    auto** newTable = new MapEntry<K,V>*[newCapacity];
    memset((void*)newTable, 0, newCapacity * sizeof(MapEntry<K,V>*));
    for (int idx = 0; idx < oldCapacity; ++idx) {
        MapEntry<K,V>* obj = _table[idx];
        if (obj == nullptr || obj == deletedObject)
            continue;
        int hash   = _getHashCode(obj->getKey());
        int index  = ( hash & 0x7FFFFFF ) % newCapacity;
        int offset = 1;
        while (newTable[index] != nullptr) {
            index  = ((index + offset) & 0x7FFFFFFF) % newCapacity;
            offset = offset * 2 + 1;
            if (offset == -1)
                offset = 2;
        }
        newTable[index] = obj;
        _table[idx]     = nullptr;
    }
    delete[] _table;
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
template <typename K, typename V> MapEntry<K,V>::MapEntry(K* key, V* value, bool ownsValue) :
        _key(key), _value(value), _ownsValue(ownsValue) {
}

template <typename K, typename V> MapEntry<K,V>::MapEntry(const MapEntry<K,V>& entry) {
    _key       = (entry._key)->copy();
    _ownsValue = entry._ownsValue;
    if (entry._value != nullptr) {
        try {
            _value = _ownsValue ? (V*)(entry._value)->copy() : entry._value;
        }
        catch (Exception& e) {
            _value     = entry._value;
            _ownsValue = false;
        }
    }
}

template <typename K, typename V> MapEntry<K,V>::~MapEntry() {
    if (_key != DynaHashMap<K,V>::nullObject) {
        delete _key;
        _key = nullptr;
    }
    if (_ownsValue) {
        delete _value;
        _value = nullptr;
    }
}

template <typename K, typename V> MapEntry<K,V>* MapEntry<K,V>::copy() {
    return new MapEntry<K,V>(*this);
};


/**
 * DynaHashMap Constructor
 * @tparam K
 * @tparam V
 */
template <typename K, typename V> DynaHashMap<K,V>::DynaHashMap() :
        _count(0), _modCount(0), _capacity(INITIAL_SIZE), _freeCells(INITIAL_SIZE), _ownsMembers(true) {
    _init(INITIAL_SIZE);
}

template <typename K, typename V> DynaHashMap<K,V>::DynaHashMap(int size) :
        _count(0), _modCount(0), _capacity(size), _freeCells(size), _ownsMembers(true) {
    _init(size);
}

template <typename K, typename V> DynaHashMap<K,V>::~DynaHashMap() {
    clear();
    delete[] _table;
}

template <typename K, typename V> DynaHashMap<K,V>::DynaHashMap(const DynaHashMap<K,V>& other) {
    MapEntry<K,V>* entry;
    _count       = other._count;
    _capacity    = other._capacity;
    _modCount    = other._modCount;
    _freeCells   = other._freeCells;
    _ownsMembers = other._ownsMembers;
    _table       = new MapEntry<K,V>*[other._capacity];
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

template <typename K, typename V> DynaHashMap<K,V>* DynaHashMap<K,V>::copy() {
    return new DynaHashMap<K,V>(*this);
};

template <typename K, typename V> MapKeyIter<K,V>::MapKeyIter(const DynaHashMap<K,V>* map, int start) : _curIndex(start), _map(map) {
    MapEntry<K,V>** table = _map->_table;
    for (; hasNext() && (table[_curIndex] == nullptr || table[_curIndex] == _map->deletedObject); ++_curIndex);
}

template <typename K, typename V> MapValueIter<K,V>::MapValueIter(const DynaHashMap<K,V>* map, int start) : _curIndex(start), _map(map) {
    MapEntry<K,V>** table = _map->_table;
    for (; hasNext() && (table[_curIndex] == nullptr || table[_curIndex] == _map->deletedObject); ++_curIndex);
}

template <typename K, typename V> int DynaHashMap<K,V>::_getTableIndex(K* key) {
    if (key == nullptr)
        key = nullObject;
    int hash   = _getHashCode(key);
    int index  = ( hash & 0x7FFFFFF ) % _capacity;
    int offset = 1;

    MapEntry<K,V>* entry = _table[index];
    while (entry != nullptr &&
            (entry == deletedObject || !((_getHashCode(entry->getKey()) == hash) && (*(entry->getKey()) == *key)))) {
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
 * @tparam K
 * @tparam V
 * @param newValue
 * @return
 */
template <typename K, typename V> const V*  MapEntry<K,V>::setValue(V* newValue) {
    V* oldValue = _value;
    _value = newValue;
    return oldValue;
}
template <typename K, typename V> bool MapEntry<K,V>::operator== (const MapEntry<K,V>& other) const {
    K* k1 = getKey();
    K* k2 = other.getKey();
    if (k1 == k2 || (k1 != nullptr && k1 != DynaHashMap<K,V>::nullObject && *k1 == *k2)) {
        V* v1 = getValue();
        V* v2 = other.getValue();
        if (v1 == v2 || (v1 != nullptr && *v1 == *v2))
            return true;
    }
    return false;
}

/**
 * DynaHashMap get Entry based on Key
 * @tparam K
 * @tparam V
 * @param key
 * @return
 */
template <typename K, typename V> MapEntry<K,V>* DynaHashMap<K,V>::getEntry(K* key) {
    int index = _getTableIndex(key);
    return _table[index];
};

template <typename K, typename V> MapEntry<K, V>* DynaHashMap<K,V>::getEntry(K key) {
    return getEntry(&key);
}

template <typename K, typename V> V* DynaHashMap<K,V>::get(K* key) {
    MapEntry<K,V>* entry = getEntry(key);
    return entry != nullptr ? entry->getValue() : nullptr;
}

template <typename K, typename V> V* DynaHashMap<K,V>::get(K key) {
    return get(&key);
}

template <typename K, typename V> V* DynaHashMap<K,V>::put(K* key, V* value) {
    if (key == nullptr)
        key = nullObject;
    int hash   = _getHashCode(key);
    int index  = ( hash & 0x7FFFFFF ) % _capacity;
    int offset = 1;
    int deletedIdx = -1;

    MapEntry<K,V>* entry = _table[index];
    while ( entry != nullptr &&
            (entry == deletedObject || !((_getHashCode(entry->getKey()) == hash) && (*(entry->getKey()) == *key)))) {
        if (entry == deletedObject)
            deletedIdx = index;

        index  = ((index + offset) & 0x7FFFFFFF) % _capacity;
        entry  = _table[index];
        offset = offset * 2 + 1;
        if (offset == -1)
            offset = 2;
    }

    V* oldValue = nullptr;
    if (entry == nullptr) {
        if (deletedIdx != -1)
            index = deletedIdx;
        else
            --_freeCells;
        ++_modCount;
        ++_count;
        _table[index] = new MapEntry<K,V>(key, value, _ownsMembers);

        if (1 - (_freeCells / (double)_capacity) > LOAD_FACTOR) {
            _reHash(_capacity);
            if (1 - (_freeCells / (double)_capacity) > LOAD_FACTOR) {
                _reHash(_capacity * 2 + 1);
            }
        }
    }
    else {
        oldValue = _table[index]->getValue();
        _table[index]->setValue(value);
        delete key;    // Delete the key that was passed in, since it was not added to the map
    }
    return oldValue;
}

template <typename K, typename V> V* DynaHashMap<K,V>::put(K key, V* value) {
    return put(new K(key), value);
}

template <typename K, typename V> V* DynaHashMap<K,V>::remove(K* key) {
    int index = _getTableIndex(key);

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

template <typename K, typename V> V* DynaHashMap<K,V>::remove(K key) {
    return remove(&key);
}

template <typename K, typename V> void DynaHashMap<K,V>::deleteEntry(K* key) {
    int index = _getTableIndex(key);

    if (_table[index] != nullptr && _table[index] != deletedObject) {
        delete _table[index];               // delete the entry from the table
        _table[index] = deletedObject;      // Mark the table entry as deleted
        ++_modCount;
        --_count;
    }
}

template <typename K, typename V> void DynaHashMap<K,V>::deleteEntry(K key) {
    return deleteEntry(&key);
};

template <typename K, typename V> MapEntry<K, V>* DynaHashMap<K,V>::removeEntry(MapEntry<K,V>* ent) {
    if (ent == nullptr)
        return nullptr;
    int index = _getTableIndex(ent->getKey());

    if (_table[index] != nullptr && _table[index] != deletedObject) {
        MapEntry<K,V>* saveEntry = _table[index];
        _table[index] = deletedObject;
        ++_modCount;
        --_count;
        return saveEntry;
    }
    else
        return nullptr;
}

template <typename K, typename V> void DynaHashMap<K,V>::clear() {
    for (int idx = 0; idx < _capacity; ++idx) {
        MapEntry<K,V>* entry = _table[idx];
        if (entry != nullptr) {
          if (entry != DynaHashMap<K,V>::deletedObject) {
              delete entry;
              --_count;
          }
          _table[idx] = nullptr;
        }
    }
    _count = 0;
    _freeCells = _capacity;
    ++_modCount;
}

template <typename K, typename V> DynaMapIter<K,V> DynaHashMap<K,V>::begin() {
    return DynaMapIter<K,V>( this, 0 );
}

template <typename K, typename V> DynaMapIter<K,V> DynaHashMap<K,V>::end() {
    return DynaMapIter<K,V>( this, _capacity );
}

template <typename K, typename V> MapKeyIter<K,V> DynaHashMap<K,V>::keys() {
    return MapKeyIter<K,V>(this);
};

template <typename K, typename V> MapValueIter<K,V> DynaHashMap<K,V>::values() {
    return MapValueIter<K,V>(this);
};

template <typename K, typename V> const MapKeyIter<K,V>& MapKeyIter<K,V>::operator++ () {
    MapEntry<K,V>** table = _map->_table;
    for (++_curIndex; hasNext() &&
                      (table[_curIndex] == nullptr || table[_curIndex] == _map->deletedObject); ++_curIndex);
    return *this;
}

template <typename K, typename V> const MapKeyIter<K,V>& MapKeyIter<K,V>::operator-- () {
    MapEntry<K,V>** table = _map->_table;
    for (--_curIndex; hasPrev() &&
                      (table[_curIndex] == nullptr || table[_curIndex] == _map->deletedObject); --_curIndex);
    return *this;
}

template <typename K, typename V> const MapValueIter<K,V>& MapValueIter<K,V>::operator++ () {
    MapEntry<K,V>** table = _map->_table;
    for (++_curIndex; hasNext() &&
                      (table[_curIndex] == nullptr || table[_curIndex] == _map->deletedObject); ++_curIndex);
    return *this;
}

template <typename K, typename V> const MapValueIter<K,V>& MapValueIter<K,V>::operator-- () {
    MapEntry<K,V>** table = _map->_table;
    for (--_curIndex; hasPrev() &&
                      (table[_curIndex] == nullptr || table[_curIndex] == _map->deletedObject); --_curIndex);
    return *this;
}

//===========================================================================
//                          Static Initialization
//===========================================================================
template <typename K, typename V> MapObject<K>  DynaHashMap<K,V>::nullObjectInstance = MapObject<K>();
template <typename K, typename V> K* DynaHashMap<K,V>::nullObject                    = (K*)(void*)(&DynaHashMap<K,V>::nullObjectInstance);

template <typename K, typename V> MapEntry<K,V> DynaHashMap<K,V>::deletedObjectInstance = MapEntry<K,V>(DynaHashMap<K,V>::nullObject, nullptr, false);
template <typename K, typename V> MapEntry<K,V>* DynaHashMap<K,V>::deletedObject        = &deletedObjectInstance;

#endif //DYNAHASHMAPIMPL_H
