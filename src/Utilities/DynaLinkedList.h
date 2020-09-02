/**
 * @file DynaLinkedList.h
 * @author Ken Kopelson (ken@metaera.com)
 * @brief 
 * @version 0.1
 * @date 2020-01-10
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef DYNALINKEDLIST_H
#define DYNALINKEDLIST_H

#include <typeinfo>
#include <iostream>
#include "TypeDefs.h"
#include "Exception.h"
#include "IComparable.h"
#include "ICopyable.h"

#define INVALID_INDEX   -1

#define MAKE_LINKEDLISTTYPE_DEF(C, T) \
    typedef LinkedEntry<C> T##LinkedEntry; \
    typedef DynaLinkedList<C> T##LinkedList

template <class T> class DynaLinkedList;
template <class T> class LinkedIter;
template <class T> class LinkedEntryIter;

template <class T> class LinkedEntry : IComparable< LinkedEntry<T> >, ICopyable< LinkedEntry<T> > {
    DynaLinkedList<T>* _owner;
    LinkedEntry<T>*    _prev;
    LinkedEntry<T>*    _next;
    bool               _ownsObject;
    T*                 _object;
public:
    explicit LinkedEntry(T* object, bool ownsObject);
    virtual ~LinkedEntry();
    LinkedEntry(const LinkedEntry<T>& other);
    LinkedEntry<T>* copy() override;

    DynaLinkedList<T>* getOwner();
    LinkedEntry<T>*    getPrevious();
    LinkedEntry<T>*    getNext();
    bool               isOwnsObject();
    T*                 getObject() const;
    LinkedEntry<T>*    setOwner(DynaLinkedList<T>* owner);
    LinkedEntry<T>*    setNext(LinkedEntry<T>* next);
    LinkedEntry<T>*    setPrevious(LinkedEntry<T>* previous);
    LinkedEntry<T>*    setObject(T* object);

    bool operator== (const LinkedEntry<T> &other) const override {
        return _object == other.getObject();
    }

};

template <class T> class DynaLinkedList : public ICopyable< DynaLinkedList<T> >{
    friend class LinkedIter<T>;
    friend class LinkedEntryIter<T>;

    LinkedEntry<T>* _first;
    LinkedEntry<T>* _last;
    bool            _ownsMembers;
    uint            _entryCount;

public:
    DynaLinkedList();
    virtual ~DynaLinkedList();
    DynaLinkedList(const DynaLinkedList<T>& other);
    DynaLinkedList<T>* copy() override;

    void clear();
    int  size();
    inline bool     isEmpty() { return _entryCount == 0; }
    LinkedEntry<T>* getFirst() const;
    LinkedEntry<T>* getLast() const;
    bool            isFirst(LinkedEntry<T>* entry);
    bool            isLast(LinkedEntry<T>* entry);
    bool            isOwnsMembers();
    void            setOwnsMembers(bool ownsMembers);
    LinkedEntry<T>* get(int index);
    uint            getCount();
    uint            count();
    int             indexOfEntry(LinkedEntry<T>* entry);
    LinkedEntry<T>* findEntry(T* object);
    int             indexOf(T* object);
    T*              find(T* object);
    LinkedEntry<T>* insertEntry(LinkedEntry<T>* entry, LinkedEntry<T>* destLink, bool after);
    LinkedEntry<T>* insertEntry(T* object, LinkedEntry<T>* destLink, bool after);
    LinkedEntry<T>* appendEntry(LinkedEntry<T>* entry);
    LinkedEntry<T>* appendEntry(T* object);
    LinkedEntry<T>* pushEntry(LinkedEntry<T>* entry);
    LinkedEntry<T>* pushEntry(T* object);
    LinkedEntry<T>* removeEntry(LinkedEntry<T>* entry);
    bool            deleteEntry(LinkedEntry<T>* entry);
    LinkedEntry<T>* popEntry();
    LinkedEntry<T>* popLastEntry();

    bool               move(LinkedEntry<T>* entry, LinkedEntry<T>* destEntry, bool after);
    bool               moveToFirst(LinkedEntry<T>* entry);
    bool               moveToLast(LinkedEntry<T>* entry);
    DynaLinkedList<T>* splitList(LinkedEntry<T>* atEntry);
    DynaLinkedList<T>* appendList(DynaLinkedList<T>* list);

    T* insert(T* object, LinkedEntry<T>* destLink, bool after);
    T* append(T* object);
    T* push(T* object);
    T* remove(LinkedEntry<T>* entry);
    T* pop();
    T* popLast();
    LinkedIter<T> begin();
    LinkedIter<T> end();
};

template <class T> class LinkedIter {
    LinkedEntry<T>*     _current;
    DynaLinkedList<T>*  _list;

public:
    explicit LinkedIter(DynaLinkedList<T>* list);
    explicit LinkedIter(DynaLinkedList<T>* list, LinkedEntry<T>* start);

    bool hasNext();
    bool hasPrevious();
    T* next();
    T* previous();
    void remove();
    bool operator== (const LinkedIter<T>& other) const;
    bool operator!= (const LinkedIter<T>& other) const;
    T* operator* () const;
    const LinkedIter<T>& operator++ ();
    const LinkedIter<T>& operator-- ();
    LinkedIter<T> begin();
    LinkedIter<T> end();
};

template <class T> class LinkedEntryIter {
    LinkedEntry<T>*     _current;
    DynaLinkedList<T>*  _list;

public:
    explicit LinkedEntryIter(DynaLinkedList<T>* list);
    explicit LinkedEntryIter(DynaLinkedList<T>* list, LinkedEntry<T>* start);

    bool hasNext();
    bool hasPrevious();
    LinkedEntry<T>* next();
    LinkedEntry<T>* previous();
    void remove();
    bool operator== (const LinkedEntryIter<T>& other) const;
    bool operator!= (const LinkedEntryIter<T>& other) const;
    LinkedEntry<T>* operator* () const;
    const LinkedEntryIter<T>& operator++ ();
    const LinkedEntryIter<T>& operator-- ();
    LinkedEntryIter<T> begin();
    LinkedEntryIter<T> end();
};

#endif //DYNALINKEDLIST_H
