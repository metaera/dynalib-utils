//
// Created by Ken Kopelson on 20/10/17.
//

#ifndef DYNAHASHMAP_H
#define DYNAHASHMAP_H

#include <typeinfo>
#include <iostream>
#include "TypeDefs.h"
#include "Exception.h"
#include "ICopyable.h"
#include "IHashable.h"

#define INVALID_INDEX   -1

#define MAKE_MAPTYPE_DEF(K, V, T) \
    typedef DynaHashMap<K,V> T##Map

/**
 * Template for a HashMap Entry object
 *
 * @tparam EK Entry Key
 * @tparam EV Entry Value
 */
template <typename K, typename V> class MapEntry : IComparable<MapEntry<K,V>>, ICopyable<MapEntry<K,V>> {
    K* _key = nullptr;
    V* _value = nullptr;
    bool _ownsValue;

public:
    MapEntry(K* key, V* value, bool ownsValue);
    MapEntry(const MapEntry<K,V>& entry);
    virtual ~MapEntry<K,V>();
    MapEntry<K,V>* copy() override;

    K* getKey() const { return _key; }
    V* getValue() const { return _value; }
    bool isOwnsValue() { return _ownsValue; }
    void setOwnsValue(bool ownsValue) { _ownsValue = ownsValue; }

    const V* setValue(V* newValue);
    bool operator== (const MapEntry<K,V>& other) const override;
};

/**
 * Template for Marker Object to mark "nullptr" entries and deleted entries
 * @tparam K
 */
// template <typename K> class MapObject : IHashable<K> {
// public:
//     MapObject<K>() = default;
//     virtual ~MapObject() = default;

//     int hashCode() const override {
//         return 0;
//     }
//     bool operator== (const K& other) const override {
//         return *this == other;
//     }
// };

template <typename K, typename V> class DynaMapIter;
template <typename K, typename V> class MapKeyIter;
template <typename K, typename V> class MapValueIter;

/**
 * Template for a HashMap object
 *
 * @tparam K type of all Keys in map
 * @tparam V type of all Values in map
 */
template <typename K, typename V> class DynaHashMap : public ICopyable<DynaHashMap<K,V>> {
protected:

    friend class MapEntry<K,V>;
    friend class DynaMapIter<K,V>;
    friend class MapKeyIter<K,V>;
    friend class MapValueIter<K,V>;

    static MapEntry<K,V>  deletedObjectInstance;
    static MapEntry<K,V>* deletedObject;

    constexpr static int    INITIAL_SIZE = 3;
    constexpr static double LOAD_FACTOR  = 0.75;

    MapEntry<K,V>** _table = nullptr;
    int             _count;
    int             _capacity;
    int             _freeCells;     // _capacity - _bufEnd + deleteCells
    int             _modCount;
    bool            _ownsMembers;

    void _init(int size);
    int  _getHashCode(const K* key) const;
    void _reHash(int newCapacity);
    int  _getTableIndex(K* key);

public:
    // static MapObject<K> nullObjectInstance;
    static K            nullObjectInstance;
    static K*           nullObject;

    explicit DynaHashMap();
    explicit DynaHashMap(int size);
    virtual ~DynaHashMap();

    DynaHashMap(const DynaHashMap<K,V>& other);
    DynaHashMap<K,V>* copy() override;

    //=========================================================================================
    //                                   Getters/Setters
    //=========================================================================================
    int  count() const         { return _count; }
    int  capacity() const      { return _capacity; }
    int  freeCells() const     { return _freeCells; }
    bool isEmpty() const       { return _count == 0; }
    bool containsKey(K* key)   { return getEntry(key) != nullptr; }
    bool containsKey(K key)    { return getEntry(key) != nullptr; }
    bool isOwnsMembers() const { return _ownsMembers; }
    void setOwnsMembers(bool ownsMembers) {
        _ownsMembers = ownsMembers;
    }

    //=========================================================================================
    //                                   Public Methods
    //=========================================================================================
    MapEntry<K,V>* getEntry(K* key);
    MapEntry<K,V>* getEntry(K key);
    V*   get(K* key);
    V*   get(K key);
    V*   put(K* key, V* value);
    V*   put(K key, V* value);
    V*   remove(K* key);
    V*   remove(K key);
    void deleteEntry(K* key);
    void deleteEntry(K key);
    MapEntry<K,V>* removeEntry(MapEntry<K,V>* entry);
    void clear();
    V* operator[](K key) { return get(key); }
    V* operator[](K* key) { return get(key); }

    DynaMapIter<K,V> begin();
    DynaMapIter<K,V> end();

    MapKeyIter<K,V> keys();
    MapValueIter<K,V> values();
};

template <typename K, typename V> class DynaMapIter {
    int                      _curIndex = 0;
    int                      _lastReturned = INVALID_INDEX;
    int                      _expectedModCount;
    const DynaHashMap<K,V>*  _map = nullptr;

public:
    DynaMapIter (const DynaHashMap<K,V>* map, int start)
            : _curIndex(start), _expectedModCount(0), _map(map) {}

    int  getIndex() { return _curIndex; }
    bool hasNext()  { return _curIndex < _map->_capacity; }
    bool hasPrev()  { return _curIndex >= 0; }

    bool operator== (const DynaMapIter<K,V>& other) const {
        return _curIndex == other._curIndex;
    }

    bool operator!= (const DynaMapIter<K,V>& other) const {
        return _curIndex != other._curIndex;
    }

    MapEntry<K,V>* operator* () const {
        return _map->_table[_curIndex];
    }

    const DynaMapIter<K,V>& operator++ () {
        ++_curIndex;
        return *this;
    }

    const DynaMapIter<K,V>& operator-- () {
        --_curIndex;
        return *this;
    }
};

template <typename K, typename V> class MapKeyIter {
    int                      _curIndex = 0;
    const DynaHashMap<K,V>*  _map = nullptr;

public:
    MapKeyIter (const DynaHashMap<K,V>* map, int start);
    explicit MapKeyIter (const DynaHashMap<K,V>* map) : MapKeyIter(map, 0) {};

    int  getIndex() { return _curIndex; }
    bool hasNext()  { return _curIndex < _map->_capacity; }
    bool hasPrev()  { return _curIndex >= 0; }

    bool operator== (const MapKeyIter<K,V>& other) const {
        return _curIndex == other._curIndex;
    }

    bool operator!= (const MapKeyIter<K,V>& other) const {
        return _curIndex != other._curIndex;
    }

    K* operator* () const {
        auto* entry = _map->_table[_curIndex];
        if (entry != nullptr) {
            K* key = entry->getKey();
            return key == _map->nullObject ? nullptr : key;
        }
        return nullptr;
    }

    const MapKeyIter<K,V>& operator++ ();
    const MapKeyIter<K,V>& operator-- ();

    MapKeyIter<K,V> begin() {
        return MapKeyIter<K,V>( _map, _curIndex );
    }

    MapKeyIter<K,V> end() {
        return MapKeyIter<K,V>( _map, _map->capacity() );
    }
};

template <typename K, typename V> class MapValueIter {
    int                      _curIndex = 0;
    const DynaHashMap<K,V>*  _map = nullptr;

public:
    MapValueIter (const DynaHashMap<K,V>* map, int start);
    explicit MapValueIter (const DynaHashMap<K,V>* map) : MapValueIter(map, 0) {};

    int  getIndex() { return _curIndex; }
    bool hasNext()  { return _curIndex < _map->_capacity; }
    bool hasPrev()  { return _curIndex >= 0; }

    bool operator== (const MapValueIter<K,V>& other) const {
        return _curIndex == other._curIndex;
    }

    bool operator!= (const MapValueIter<K,V>& other) const {
        return _curIndex != other._curIndex;
    }

    V* operator* () const {
        auto* entry = _map->_table[_curIndex];
        return entry != nullptr ? entry->getValue() : nullptr;
    }

    const MapValueIter<K,V>& operator++ ();
    const MapValueIter<K,V>& operator-- ();

    MapValueIter<K,V> begin() {
        return MapValueIter<K,V>( _map, _curIndex );
    }

    MapValueIter<K,V> end() {
        return MapValueIter<K,V>( _map, _map->capacity() );
    }
};

#endif //DYNAHASHMAP_H
