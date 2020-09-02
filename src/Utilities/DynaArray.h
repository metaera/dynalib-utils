//
// Created by Ken Kopelson on 14/10/17.
//

#ifndef DYNAARRAY_H
#define DYNAARRAY_H

#include <exception>
#include <type_traits>
#include <cstring>
#include "TypeDefs.h"
#include "ICopyable.h"

using namespace std;

#define AUTO_PACK       0x01
#define PACK_NEEDED     0x02
#define AUTO_TRIM       0x04
#define END_OF_LIST     -1
#define INVALID_INDEX   -1
#define FIRST_FREE      -2

#define MAKE_ARRAYTYPE_DEF(C, T) \
    typedef DynaArray<C> T##Array

template <class T> class DynaArrayIter;

template <class T> class DynaArray : ICopyable< DynaArray<T> > {

    friend class DynaArrayIter<T>;

    T*      _members  = nullptr;
    uint    _capacity = 0;
    uint    _count    = 0;
    uint8_t _flags    = AUTO_PACK | AUTO_TRIM;

    void _insertArray(T *array, int arrayOffset, int toIndex, int count);
    void _swap(int index1, int index2);
    DynaArray<T>* _copyRange(int frIndex, int toIndex);
    void _deleteExcessCapacity();
    void _deleteRange(int frIndex, int toIndex);
    void _clearRange(int frIndex, int toIndex);
    void _deleteOrClear(int frIndex, int toIndex);
    bool _setAutoPack(bool isPack);
    bool _setAutoTrim(bool isTrim);

protected:
    inline void setFlags(uint8_t flags)    { _flags |= flags; }
    inline void setAllFlags(uint8_t flags) { _flags = flags; }
    inline void clearFlags(uint8_t flags)  { _flags &= ~flags; }
    inline void clearAllFlags()         { _flags = (uint8_t)0x00; }

public:
    //========================================================================================
    //                                   Instance Methods
    //========================================================================================
    DynaArray() = default;
    explicit DynaArray(uint initCapacity);
    explicit DynaArray(T* array, int count);
    virtual ~DynaArray();

    DynaArray(const DynaArray<T>& other);
    DynaArray<T>* copy() override;

    inline uint  getCapacity() { return _capacity; }
    inline uint  getCount() { return _count; }
    inline uint  count() { return _count; }
    inline bool  isFull() { return _count == _capacity; }
    inline bool  isEmpty() { return _count == 0; }
    inline void  trim() { adjustSize(0); }
    inline T     get(uint index) const { return _members[index]; }
    inline T*    getInternalTypedArray() { return _members; }
    inline void* getInternalRawArray() { return _members; }

//    T       operator[](uint index) { return _members[index]; }
//    const T operator[](uint index) const { return _members[index]; }
//    T       operator[](uint index) const { return _members[index]; }
    T  operator [](uint index) const    { return _members[index]; }
    T& operator [](uint index)          { return _members[index]; }

    DynaArray<T>& operator= (const DynaArray<T>& array);
 

    DynaArray<T>& operator+=(T value);
    DynaArray<T>& operator+=(T& value);
    DynaArray<T>& operator-=(int index);

    DynaArray<T>& operator<<(T value);
    DynaArray<T>& operator<<(T& value);

    void setCapacity(uint newCapacity);
    void determineTheCount(int invalidValue);
    void setCount(uint count);
    void setAutoPackTrim(bool pack, bool trim);
    void adjustSize(int delta);
    void adjustSizeLog(int delta);
    void pack();
    void clear();
    int  indexOf(T value, uint start, uint step);
    int  indexOf(T* value, uint arraySize, uint start, uint step);
    int  indexOf(T value);
    int  indexOf(T* value, uint arraySize);
    int  revIndexOf(T value, uint start, uint step);
    int  revIndexOf(T* value, uint arraySize, uint start, uint step);
    int  revIndexOf(T value);
    int  revIndexOf(T* value, uint arraySize);

    int  insert(int index, T value);
    int  insert(int index, T* value);
    int  insert(int index, T* values, uint length);
    int  insert(int index, uint8_t* values, uint offset, uint length);
    int  insert(int index, DynaArray<T>* array);
    int  append(T value);
    int  append(T* values, uint length);
    int  append(uint8_t* values, uint offset, uint length);
    int  append(DynaArray<T>* array);
    T    setValue(int index, T value);
    void move(int index, int destIndex);
    void slide(int frIndex, int toIndex);
    T    remove(int index);
    DynaArray<T>* remove(int frIndex, int toIndex);
    void deleteItem(int index);
    void deleteItems(int frIndex, int toIndex);
    void push(T value);
    T    pop();
    T    popLast();
    DynaArrayIter<T> begin() const;
    DynaArrayIter<T> end() const;
};

template <class T> class DynaArrayIter {
    int           _curIndex;
    int           _lastReturned = INVALID_INDEX;
    const DynaArray<T>* _array;

public:
    DynaArrayIter (const DynaArray<T>* array, int start)
            : _curIndex(start), _array(array)
    { }

    int  getIndex() { return _curIndex; }
    bool hasNext()  { return _curIndex < _array->_count; }
    bool hasPrev()  { return _curIndex >= 0; }

    bool operator== (const DynaArrayIter<T>& other) const {
        return _curIndex == other._curIndex;
    }

    bool operator!= (const DynaArrayIter<T>& other) const {
        return _curIndex != other._curIndex;
    }

    T operator* () const {
        return _array->get( _curIndex );
    }

    const DynaArrayIter<T>& operator++ () {
        ++_curIndex;
        return *this;
    }

    const DynaArrayIter<T>& operator-- () {
        --_curIndex;
        return *this;
    }
};

MAKE_ARRAYTYPE_DEF(char, Char);
MAKE_ARRAYTYPE_DEF(int, Int);
MAKE_ARRAYTYPE_DEF(long, Long);

#endif //DYNAARRAY_H
