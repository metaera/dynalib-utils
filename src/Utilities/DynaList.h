/**
 * @file DynaList.h
 * @author Ken Kopelson (ken@metaera.com)
 * @brief 
 * @version 0.1
 * @date 2020-01-10
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef DYNALIST_H
#define DYNALIST_H

#include <exception>
#include <type_traits>
//#include <cstring>
#include "TypeDefs.h"
#include "ICopyable.h"

using namespace std;

#define AUTO_PACK       0x01
#define PACK_NEEDED     0x02
#define AUTO_TRIM       0x04
#define OWNS_MEMBERS    0x08
#define END_OF_LIST     -1
#define INVALID_INDEX   -1
#define FIRST_FREE      -2

class String;

#define MAKE_LISTTYPE_DEF(C, T) \
    typedef DynaList<C> T##List

template <class T> class DynaListIter;

template <class T> class DynaList : public ICopyable< DynaList<T> > {

    friend class DynaListIter<T>;

    T**     _members  = nullptr;
    uint    _capacity = 0;
    uint    _count    = 0;
    uint8_t _flags    = AUTO_PACK | AUTO_TRIM | OWNS_MEMBERS;

protected:
    void _insertArray(T** array, int arrayOffset, int toIndex, int count);
    void _swap(int index1, int index2);
    void _trimTheCount();
    DynaList<T>* _copyRange(int frIndex, int toIndex);
    void _deleteExcessCapacity();
    void _deleteRange(int frIndex, int toIndex);
    void _clearRange(int frIndex, int toIndex);
    void _nullRange(int frIndex, int toIndex);
    void _deleteOrClear(int frIndex, int toIndex);
    bool _setAutoPack(bool isPack);
    bool _setAutoTrim(bool isTrim);
    int  _nextFreeSlot();

    inline void setFlags(uint8_t flags)    { _flags |= flags; }
    inline void setAllFlags(uint8_t flags) { _flags = flags; }
    inline void clearFlags(uint8_t flags)  { _flags &= ~flags; }
    inline void clearAllFlags()            { _flags = 0x00; }
    inline bool isFlags(uint8_t flags)     { return (_flags & flags) == flags; }

public:
    //========================================================================================
    //                                   Instance Methods
    //========================================================================================
    DynaList();
    explicit DynaList(uint initCapacity);
    explicit DynaList(T* array, int count);
    virtual ~DynaList();

    DynaList(const DynaList<T>& other);
    DynaList<T>* copy() override;

    inline uint   getCapacity() { return _capacity; }
    inline uint   getCount() { return _count; }
    inline uint   capacity() { return _capacity; }
    inline uint   count() { return _count; }
    inline bool   isFull() { return _count == _capacity; }
    inline bool   isEmpty() { return _count == 0; }
    inline T*     get(uint index) const { return _members[index]; }
    inline T**    getInternalTypedArray() { return _members; }
    inline void** getInternalRawArray() { return (void **)_members; }
    inline T*     first() { return _count > 0 ? _members[0] : nullptr; }
    inline T*     last() { return _count > 0 ? _members[_count-1] : nullptr; }

    // inline T*       operator[](uint index) { return _members[index]; }
    // inline const T* operator[](uint index) const { return _members[index]; }

    inline T*  operator [](uint index) const    { return _members[index]; }
    inline T*& operator [](uint index)          { return _members[index]; }

    // void         operator+=(T* value) { append(value); };
    // void         operator+=(T&& value) { append(&value); };
    // void         operator-=(int index) { deleteItem(index); };
    DynaList<T>& operator+=(T* value);
    DynaList<T>& operator+=(T&& value);
    DynaList<T>& operator-=(int index);

    DynaList<T>& operator<<(T* value);
    DynaList<T>& operator<<(T&& value);

    int  firstNullSlot();
    DynaList<T>* setCapacity(uint newCapacity);
    DynaList<T>* setAutoPackTrim(bool pack, bool trim);
    DynaList<T>* setOwnsMembers(bool ownsMembers);
    void adjustSize(int delta);
    void adjustSizeLog(int delta);
    void trim();
    void pack();
    void clear();

    int  indexOf(T* value, uint start, uint step);
    int  indexOf(T** value, uint arraySize, uint start, uint step);

    int  insert(int index, T* value);
    int  insert(int index, T&& value);
    int  insert(int index, T** array, uint length);
    int  insert(int index, DynaList<T>* list);
    int  append(T* value);
    int  append(T&& value);
    int  append(T** array, uint length);
    int  append(DynaList<T>* list);
    T*   set(int index, T* value);
    void move(int index, int destIndex);
    void slide(int frIndex, int toIndex);
    T*   remove(int index);
    DynaList<T>* remove(int frIndex, int toIndex);
    void deleteItem(int index);
    void deleteItems(int frIndex, int toIndex);
    void push(T* value);
    void pushLast(T* value);
    T*   pop();
    T*   popLast();
    DynaListIter<T> begin() const;
    DynaListIter<T> end() const;
};

template <class T> class DynaListIter {
    int                 _curIndex;
    int                 _lastReturned = INVALID_INDEX;
    const DynaList<T>*  _list = nullptr;

public:
    DynaListIter (const DynaList<T>* list, int start)
            : _curIndex(start), _list(list)
    { }

    int  getIndex() { return _curIndex; }
    bool hasNext()  { return _curIndex < _list->_count; }
    bool hasPrev()  { return _curIndex >= 0; }

    bool operator== (const DynaListIter<T>& other) const {
        return _curIndex == other._curIndex;
    }

    bool operator!= (const DynaListIter<T>& other) const {
        return _curIndex != other._curIndex;
    }

    T* operator* () const {
        return _list->get( _curIndex );
    }

    const DynaListIter<T>& operator++ () {
        ++_curIndex;
        return *this;
    }

    const DynaListIter<T>& operator-- () {
        --_curIndex;
        return *this;
    }
};

MAKE_LISTTYPE_DEF(String, String);

#endif //DYNALIST_H
