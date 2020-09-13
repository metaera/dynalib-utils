//
// Created by Ken Kopelson on 4/03/18.
//

#ifndef DYNALINKEDLISTIMPL_H
#define DYNALINKEDLISTIMPL_H

#include "DynaLinkedList.h"

#define MAKE_LINKEDLISTTYPE_INSTANCE(C, T) \
    template class LinkedEntry<C>; \
    typedef LinkedEntry<C> T##LinkedEntry; \
    template class DynaLinkedList<C>; \
    typedef DynaLinkedList<C> T##LinkedList

//===========================================================================
//                         LinkedEntry Methods
//===========================================================================
template <class T> LinkedEntry<T>::LinkedEntry(T* object, bool ownsObject) :
        _owner(nullptr), _prev(nullptr), _next(nullptr), _ownsObject(ownsObject), _object(object){}

template <class T> LinkedEntry<T>::~LinkedEntry() {
    if (_object != nullptr && _ownsObject) {
        delete _object;
        _object = nullptr;
    }
}

template <class T> LinkedEntry<T>::LinkedEntry(const LinkedEntry<T>& other) {
    _owner      = nullptr;
    _prev       = nullptr;
    _next       = nullptr;
    _object     = other._object;
    _ownsObject = other._ownsObject;
}

template <class T> LinkedEntry<T>* LinkedEntry<T>::copy() {
    return new LinkedEntry<T>(*this);
}


template <class T> DynaLinkedList<T>* LinkedEntry<T>::getOwner() {
    return _owner;
}

template <class T> LinkedEntry<T>* LinkedEntry<T>::getPrevious() {
    return _prev;
}

template <class T> LinkedEntry<T>* LinkedEntry<T>::getNext() {
    return _next;
}

template <class T> bool LinkedEntry<T>::isOwnsObject() {
    return _ownsObject;
}

template <class T> T* LinkedEntry<T>::getObject() const {
    return _object;
}

template <class T> LinkedEntry<T>* LinkedEntry<T>::setOwner(DynaLinkedList<T>* owner) {
    _owner = owner;
    return this;
}

template <class T> LinkedEntry<T>* LinkedEntry<T>::setNext(LinkedEntry<T>* next) {
    _next = next;
    return this;
}

template <class T> LinkedEntry<T>* LinkedEntry<T>::setPrevious(LinkedEntry<T>* previous) {
    _prev = previous;
    return this;
}

template <class T> LinkedEntry<T>* LinkedEntry<T>::setObject(T* object) {
    _object = object;
    return this;
}

//=========================================================================================
//                                  Miscellaneous Methods
//=========================================================================================
template <class T> DynaLinkedList<T>::DynaLinkedList() :
        _first(nullptr), _last(nullptr), _ownsMembers(true), _entryCount(0){}

template<class T>
DynaLinkedList<T>::~DynaLinkedList() {
    clear();
}

template<class T>
DynaLinkedList<T>::DynaLinkedList(const DynaLinkedList<T>& other) :
    _ownsMembers(other._ownsMembers), _entryCount(other._entryCount) {
    auto it = LinkedEntryIter<T>(const_cast<DynaLinkedList<T>*>(&other), _first);
    for (LinkedEntry<T>* entry : it) {
        pushEntry(entry->copy());
    }
}

template<class T>
DynaLinkedList<T>* DynaLinkedList<T>::copy() {
    return new DynaLinkedList<T>(*this);
}

template <class T> void DynaLinkedList<T>::clear() {
    auto it = LinkedEntryIter<T>(this, _first);
    while (it.hasNext()) {
        it.remove();
    }
    _first      = nullptr;
    _last       = nullptr;
    _entryCount = 0;
}

template<class T>
int DynaLinkedList<T>::size() {
    return _entryCount;
}


template<class T>
LinkedEntry<T>* DynaLinkedList<T>::getFirst() const {
    return _first;
}

template<class T>
LinkedEntry<T>* DynaLinkedList<T>::getLast() const {
    return _last;
}

template<class T>
bool DynaLinkedList<T>::isFirst(LinkedEntry<T>* entry) {
    return entry != nullptr && entry == _first;
}

template<class T>
bool DynaLinkedList<T>::isLast(LinkedEntry<T>* entry) {
    return entry != nullptr && entry == _last;
}

template<class T>
bool DynaLinkedList<T>::isOwnsMembers() {
    return _ownsMembers;
}

template<class T>
void DynaLinkedList<T>::setOwnsMembers(bool ownsMembers) {
    _ownsMembers = ownsMembers;
}

template<class T>
LinkedEntry<T>* DynaLinkedList<T>::get(int index) {
    auto it = LinkedEntryIter<T>(this, _first);
    int idx = 0;
    for (LinkedEntry<T>* entry : it) {
        if (idx == index)
            return entry;
        ++idx;
    }
    return nullptr;
}

template<class T>
uint DynaLinkedList<T>::getCount() {
    return _entryCount;    
}

template<class T>
uint DynaLinkedList<T>::count() {
    return _entryCount;
}

//=========================================================================================
//                                   Find/Locate Methods
//=========================================================================================
template<class T>
int DynaLinkedList<T>::indexOfEntry(LinkedEntry<T>* entry) {
    auto it = LinkedEntryIter<T>(this, _first);
    int index = 0;
    for (LinkedEntry<T>* e : it) {
        if (e == entry)
            return index;
        ++index;
    }
    return INVALID_INDEX;
}

template<class T>
LinkedEntry<T>* DynaLinkedList<T>::findEntry(T* object) {
    auto it = LinkedEntryIter<T>(this, _first);
    for (LinkedEntry<T>* entry : it) {
        if (entry->getObject() == object)
            return entry;
    }
    return nullptr;
}

template<class T>
int DynaLinkedList<T>::indexOf(T* object) {
    auto it = LinkedIter<T>(this, _first);
    int index = 0;
    for (T* obj : it) {
        if (obj == object)
            return index;
        ++index;
    }
    return INVALID_INDEX;
}

template<class T>
T* DynaLinkedList<T>::find(T* object) {
    auto it = LinkedIter<T>(this, _first);
    for (T* obj : it) {
        if (obj == object)
            return obj;
    }
    return nullptr;
}

//=========================================================================================
//                                 Entry-Oriented Methods
//=========================================================================================
template<class T>
LinkedEntry<T>* DynaLinkedList<T>::insertEntry(LinkedEntry<T>* entry, LinkedEntry<T>* destLink, bool after) {
    if (entry != nullptr) {
        if (entry->getOwner() == nullptr) {
            if (after) {
                if (destLink == nullptr)
                    destLink = _last;
                if (_first == nullptr)
                    _first = entry;
                if (_last == nullptr || destLink == _last)
                    _last = entry;
                if (destLink != nullptr) {
                    entry->setPrevious(destLink);
                    entry->setNext(destLink->getNext());
                    if (destLink->getNext() != nullptr)
                        destLink->getNext()->setPrevious(entry);
                    destLink->setNext(entry);
                }
            }
            else {
                if (destLink == nullptr)
                    destLink = _first;
                if (_first == nullptr || destLink == _first)
                    _first = entry;
                if (_last == nullptr)
                    _last = entry;
                if (destLink != nullptr) {
                    entry->setPrevious(destLink->getPrevious());
                    entry->setNext(destLink);
                    if (destLink->getPrevious() != nullptr)
                        destLink->getPrevious()->setNext(entry);
                    destLink->setPrevious(entry);
                }
            }
            entry->setOwner(this);
            ++_entryCount;
        }
        else
            entry = nullptr;
    }
    return entry;
}

template<class T>
LinkedEntry<T>* DynaLinkedList<T>::insertEntry(T* object, LinkedEntry<T>* destLink, bool after) {
    if (object != nullptr) {
        auto* entry = new LinkedEntry<T>(object, _ownsMembers);
        insertEntry(entry, destLink, after);
        return entry;
    }
    return nullptr;
}

template<class T>
LinkedEntry<T>* DynaLinkedList<T>::appendEntry(LinkedEntry<T>* entry) {
    return insertEntry(entry, _last, true);
}

template<class T>
LinkedEntry<T>* DynaLinkedList<T>::appendEntry(T* object) {
    return insertEntry(object, _last, true);
}

template<class T>
LinkedEntry<T>* DynaLinkedList<T>::pushEntry(LinkedEntry<T>* entry) {
    return insertEntry(entry, _first, false);
}

template<class T>
LinkedEntry<T>* DynaLinkedList<T>::pushEntry(T* object) {
    return insertEntry(object, _first, false);
}

template <class T> LinkedEntry<T>* DynaLinkedList<T>::removeEntry(LinkedEntry<T>* entry) {
    if (entry != nullptr) {
        if (entry->getOwner() == this) {
            if (entry->getPrevious() != nullptr)
                entry->getPrevious()->setNext(entry->getNext());
            if (entry->getNext() != nullptr)
                entry->getNext()->setPrevious(entry->getPrevious());
            if (_first == entry)
                _first = entry->getNext();
            if (_last == entry)
                _last = entry->getPrevious();
            entry->setNext(nullptr);
            entry->setPrevious(nullptr);
            entry->setOwner(nullptr);
            --_entryCount;
        }
        else
            entry = nullptr;
    }
    return entry;
}

template <class T> bool DynaLinkedList<T>::deleteEntry(LinkedEntry<T>* entry) {
    if (entry != nullptr)
        entry = removeEntry(entry);
    if (entry != nullptr) {
        delete entry;
        return true;
    }
    return false;
}

template<class T>
LinkedEntry<T>* DynaLinkedList<T>::popEntry() {
    return removeEntry(_first);
}

template<class T>
LinkedEntry<T>* DynaLinkedList<T>::popLastEntry() {
    return removeEntry(_last);
}

template<class T>
bool DynaLinkedList<T>::move(LinkedEntry<T>* entry, LinkedEntry<T>* destEntry, bool after) {
    if (entry != nullptr && entry != destEntry) {
        if (entry->getOwner() == this)
            removeEntry(entry);
        else if (entry->getOwner() != nullptr)
            entry->getOwner()->removeEntry(entry);
        if (destEntry == nullptr || destEntry->getOwner() == this) {
            insertEntry(entry, destEntry, after);
            return true;
        }
        else if (destEntry->getOwner() != nullptr) {
            destEntry->getOwner()->insertEntry(entry, destEntry, after);
            return true;
        }
    }
    return false;
}

template<class T>
bool DynaLinkedList<T>::moveToFirst(LinkedEntry<T>* entry) {
    return move(entry, _first, false);
}

template<class T>
bool DynaLinkedList<T>::moveToLast(LinkedEntry<T>* entry) {
    return move(entry, _last, true);
}

template<class T>
DynaLinkedList<T>* DynaLinkedList<T>::splitList(LinkedEntry<T>* atEntry) {
    if (atEntry != nullptr && atEntry->getOwner() == this) {
        auto* newList = new DynaLinkedList<T>();
        newList->_first = atEntry;
        newList->_last  = _last;
        if (atEntry->getPrevious() != nullptr) {
            atEntry->getPrevious()->setNext(nullptr);
            _last = atEntry->getPrevious();
            atEntry->setPrevious(nullptr);
        }
        else {
            _first = _last = nullptr;
        }
        auto it = LinkedEntryIter<T>(newList, _first);
        int count = 0;
        for (LinkedEntry<T>* entry : it) {
            entry->setOwner(newList);
            ++count;
        }
        _entryCount -= count;
        newList->_entryCount = count;
        return newList;
    }
    return nullptr;
}

template<class T>
DynaLinkedList<T>* DynaLinkedList<T>::appendList(DynaLinkedList<T>* list) {
    if (list != this && list->_first != nullptr) {
        if (_last != nullptr) {
            _last->setNext(list->_first);
            if (list->_first != nullptr)
                list->_first->setPrevious(_last);
            _last = list->_last;
        }
        else {
            _first = list->_first;
            _last  = list->_last;
        }
        auto it = LinkedEntryIter<T>(list, list->_first);
        int count = 0;
        for (LinkedEntry<T>* entry : it) {
            entry->setOwner(this);
            ++count;
        }
        _entryCount += count;
        list->_first = list->_last = nullptr;
        list->_entryCount = 0;
        return this;
    }
    return nullptr;
}

//=========================================================================================
//                                Object-Oriented Methods
//=========================================================================================
template<class T>
T* DynaLinkedList<T>::insert(T* object, LinkedEntry<T>* destLink, bool after) {
    if (object != nullptr) {
        auto* entry = new LinkedEntry<T>(object, _ownsMembers);
        insertEntry(entry, destLink, after);
    }
    return object;
}

template<class T>
T* DynaLinkedList<T>::append(T* object) {
    return insert(object, _last, true);
}

template<class T>
T* DynaLinkedList<T>::push(T* object) {
    return insert(object, _first, false);
}

template <class T> T* DynaLinkedList<T>::remove(LinkedEntry<T>* entry) {
    if (entry != nullptr)
        entry = removeEntry(entry);
    if (entry != nullptr) {
        T* object = entry->getObject();
        entry->setObject(nullptr);
        delete entry;
        return object;
    }
    return nullptr;
}

template<class T>
T* DynaLinkedList<T>::pop() {
    return remove(_first);
}

template<class T>
T* DynaLinkedList<T>::popLast() {
    return remove(_last);
}

template <class T> LinkedIter<T> DynaLinkedList<T>::begin () {
    return LinkedIter<T>( this, _first );
}

template <class T> LinkedIter<T> DynaLinkedList<T>::end () {
    return LinkedIter<T>( this, nullptr );
}


//=========================================================================================
//                                Linked Iterator Methods
//=========================================================================================
template <class T> LinkedIter<T>::LinkedIter(DynaLinkedList<T>* list)
        : _list(list), _current(list->getFirst()) {}

template <class T> LinkedIter<T>::LinkedIter(DynaLinkedList<T>* list, LinkedEntry<T>* start)
        : _current(start), _list(list) {}

template <class T> bool LinkedIter<T>::hasNext()     {
    return _current == nullptr ? _list->getFirst() != nullptr  : _current->getNext() != nullptr;
}

template <class T> bool LinkedIter<T>::hasPrevious() {
    return _current == nullptr ? _list->getLast() != nullptr : _current->getPrevious() != nullptr;
}

template <class T> T* LinkedIter<T>::next() {
    _current = _current == nullptr ? _list->getFirst() : _current->getNext();
    return _current != nullptr ? _current->getObject() : nullptr;
}

template <class T> T* LinkedIter<T>::previous() {
    _current = _current == nullptr ? _list->getLast() : _current->getPrevious();
    return _current != nullptr ? _current->getObject() : nullptr;
}

template <class T> void LinkedIter<T>::remove() {
    if (_current != nullptr) {
        LinkedEntry<T>* next = _current->getNext();
        if (next == nullptr)
            next = _current->getPrevious();
        _list->deleteEntry(_current);
        _current = next;
    }
}

template <class T> bool LinkedIter<T>::operator== (const LinkedIter<T>& other) const {
    return _current == other._current;
}

template <class T> bool LinkedIter<T>::operator!= (const LinkedIter<T>& other) const {
    return _current != other._current;
}

template <class T> T* LinkedIter<T>::operator* () const {
    return _current->getObject();
}

template <class T> const LinkedIter<T>& LinkedIter<T>::operator++ () {
    next();
    return *this;
}

template <class T> const LinkedIter<T>& LinkedIter<T>::operator-- () {
    previous();
    return *this;
}

template <class T> LinkedIter<T> LinkedIter<T>::begin() {
    return LinkedIter<T>( _list, _list->getFirst() );
}

template <class T> LinkedIter<T> LinkedIter<T>::end() {
    return LinkedIter<T>( _list, nullptr );
}


//=========================================================================================
//                              Linked Entry Iterator Methods
//=========================================================================================
template <class T> LinkedEntryIter<T>::LinkedEntryIter(DynaLinkedList<T>* list)
        : _list(list), _current(list->getFirst()) {}

template <class T> LinkedEntryIter<T>::LinkedEntryIter(DynaLinkedList<T>* list, LinkedEntry<T>* start)
        : _current(start),_list(list) {}

template <class T> bool LinkedEntryIter<T>::hasNext() {
    return _current == nullptr ? _list->getFirst() != nullptr  :
            ( _current->getNext() != nullptr || _list->getLast() != nullptr );
}

template <class T> bool LinkedEntryIter<T>::hasPrevious() {
    return _current == nullptr ? _list->getLast() != nullptr :
            ( _current->getPrevious() != nullptr || _list->getFirst() != nullptr );
}

template <class T> LinkedEntry<T>* LinkedEntryIter<T>::next() {
    _current = _current == nullptr ? _list->getFirst() :
                ( _current->getNext() != nullptr ? _current->getNext() : _list->getLast() );
    return _current;
}

template <class T> LinkedEntry<T>* LinkedEntryIter<T>::previous() {
    _current = _current == nullptr ? _list->getLast() :
                ( _current->getPrevious() != nullptr ? _current->getPrevious() : _list->getFirst() );
    return _current;
}

template <class T> void LinkedEntryIter<T>::remove() {
    if (_current != nullptr) {
        LinkedEntry<T>* next = _current->getNext();
        if (next == nullptr)
            next = _current->getPrevious();
        _list->deleteEntry(_current);
        _current = next;
    }
}

template <class T> bool LinkedEntryIter<T>::operator== (const LinkedEntryIter<T>& other) const {
    return _current == other._current;
}

template <class T> bool LinkedEntryIter<T>::operator!= (const LinkedEntryIter<T>& other) const {
    return _current != other._current;
}

template <class T> LinkedEntry<T>* LinkedEntryIter<T>::operator* () const {
    return _current;
}

template <class T> const LinkedEntryIter<T>& LinkedEntryIter<T>::operator++ () {
    next();
    return *this;
}

template <class T> const LinkedEntryIter<T>& LinkedEntryIter<T>::operator-- () {
    previous();
    return *this;
}

template <class T> LinkedEntryIter<T> LinkedEntryIter<T>::begin() {
    return LinkedEntryIter<T>( _list, _list->getFirst() );
}

template <class T> LinkedEntryIter<T> LinkedEntryIter<T>::end() {
    return LinkedEntryIter<T>( _list, nullptr );
}

#endif //DYNALINKEDLISTIMPL_H
