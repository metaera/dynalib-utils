//
// Created by Ken Kopelson on 27/12/17.
//

#ifndef DYNAHASHSET_H
#define DYNAHASHSET_H

#include <typeinfo>
#include <iostream>
#include "TypeDefs.h"
#include "Exception.h"
#include "ICopyable.h"
#include "IHashable.h"

#define INVALID_INDEX   -1

#define MAKE_SETTYPE_DEF(E, T) \
    typedef DynaHashSet<E> T##Set

/**
 * Template for a HashSet Entry object
 *
 * @tparam E Entry
 */
template <typename V> class SetEntry : IComparable<SetEntry<V>>, ICopyable<SetEntry<V>> {
    V*   _value = nullptr;
    bool _ownsValue;

public:
    SetEntry(V* value, bool ownsValue);
    virtual ~SetEntry<V>();

    SetEntry(const SetEntry<V>& other);
    SetEntry<V>* copy() override;

    V* getValue() const { return _value; }

    const V* setValue(V* newValue);
    bool operator== (const SetEntry<V>& other) const override;
    bool isOwnsValue() const { return _ownsValue; }
};

/**
 * Template for Marker Object to mark "nullptr" entries and deleted entries
 * @tparam E
 */
template <typename V> class SetObject : IHashable<V> {
public:
    SetObject<V>() = default;
    virtual ~SetObject() = default;

    int hashCode() const override {
        return 0;
    }
    bool operator== (const V& other) const override {
        return *this == other;
    }
};

template <typename V> class DynaSetIter;
template <typename V> class SetIter;

template <typename V> class DynaHashSet : public ICopyable<DynaHashSet<V>> {
protected:

    friend class SetEntry<V>;
    friend class DynaSetIter<V>;
    friend class SetIter<V>;

    constexpr static int    INITIAL_SIZE = 3;
    constexpr static double LOAD_FACTOR  = 0.75;

    static SetEntry<V>  deletedObjectInstance;
    static SetEntry<V>* deletedObject;

    SetEntry<V>**  _table = nullptr;
    int            _count;
    int            _capacity;
    int            _freeCells;     // _capacity - _bufEnd + deleteCells
    int            _modCount;
    bool           _ownsMembers;

    void _init(int size);
    int  _getHashCode(const V* value) const;
    void _reHash(int newCapacity);
    int  _getTableIndex(V* value);

public:
    static SetObject<V> nullObjectInstance;
    static V*           nullObject;

    explicit DynaHashSet();
    explicit DynaHashSet(int size);
    virtual ~DynaHashSet();

    DynaHashSet(const DynaHashSet<V>& other);
    DynaHashSet<V>* copy() override;

    //=========================================================================================
    //                                   Getters/Setters
    //=========================================================================================
    int  count() const { return _count; }
    int  capacity() const { return _capacity; }
    int  freeCells() const { return _freeCells; }
    bool isEmpty() const { return _count == 0; }
    bool contains(V* obj) { return getEntry(obj) != nullptr; }
    bool isOwnsMembers() const { return _ownsMembers; }
    void setOwnsMembers(bool ownsMembers) {
        _ownsMembers = ownsMembers;
    }

    //=========================================================================================
    //                                   Public Methods
    //=========================================================================================
    SetEntry<V>* getEntry(V* value);
    SetEntry<V>* getEntry(V value);
    bool add(V* value);
    bool add(V value);
    V* remove(V* value);
    V* remove(V value);
    void deleteEntry(V* value);
    void deleteEntry(V value);
    SetEntry<V>* removeEntry(SetEntry<V>* entry);
    void clear();

    DynaSetIter<V> begin();
    DynaSetIter<V> end();

    SetIter<V> values();
};

template <typename V> class DynaSetIter {
    int                      _curIndex = 0;
    int                      _lastReturned = INVALID_INDEX;
    int                      _expectedModCount;
    const DynaHashSet<V>*    _set = nullptr;

public:
    DynaSetIter (const DynaHashSet<V>* set, int start)
            : _curIndex(start), _expectedModCount(0), _set(set) {}

    int  getIndex() { return _curIndex; }
    bool hasNext()  { return _curIndex < _set->_capacity; }
    bool hasPrev()  { return _curIndex >= 0; }

    bool operator== (const DynaSetIter<V>& other) const {
        return _curIndex == other._curIndex;
    }

    bool operator!= (const DynaSetIter<V>& other) const {
        return _curIndex != other._curIndex;
    }

    SetEntry<V>* operator* () const {
        return _set->_table[_curIndex];
    }

    const DynaSetIter<V>& operator++ () {
        ++_curIndex;
        return *this;
    }

    const DynaSetIter<V>& operator-- () {
        --_curIndex;
        return *this;
    }
};

template <typename V> class SetIter {
    int                    _curIndex = 0;
    const DynaHashSet<V>*  _set = nullptr;

public:
    SetIter (const DynaHashSet<V>* set, int start);
    explicit SetIter (const DynaHashSet<V>* set) : SetIter(set, 0) {}

    int  getIndex() { return _curIndex; }
    bool hasNext()  { return _curIndex < _set->_capacity; }
    bool hasPrev()  { return _curIndex >= 0; }

    bool operator== (const SetIter<V>& other) const {
        return _curIndex == other._curIndex;
    }

    bool operator!= (const SetIter<V>& other) const {
        return _curIndex != other._curIndex;
    }

    V* operator* () const {
        V* value = _set->_table[_curIndex]->getValue();
        return value == _set->nullObject ? nullptr : value;
    }

    const SetIter<V>& operator++ ();
    const SetIter<V>& operator-- ();

    SetIter<V> begin() {
        return SetIter<V>( _set, _curIndex );
    }

    SetIter<V> end() {
        return SetIter<V>( _set, _set->capacity() );
    }
};


#endif //DYNAHASHSET_H
