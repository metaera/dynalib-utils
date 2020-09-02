#ifndef DYNAWAITQUEUEIMPL_H
#define DYNAWAITQUEUEIMPL_H

#include <iostream>
#include <type_traits>

#include "BitManip.h"
#include "DynaWaitQueue.h"
#include "CheckForError.h"
#include "Exception.h"

#define MAKE_WAITQUEUETYPE_INSTANCE(C, T) \
    template class DynaWaitQueue<C>; \
    typedef DynaWaitQueue<C> T##WaitQueue


template <class T> DynaWaitQueue<T>::DynaWaitQueue(int maxSize) {
    sem_init(&_empty, 0, maxSize);  // MAX buffers are empty to begin with...
    sem_init(&_full,  0, 0);        // ... and 0 are full
    sem_init(&_mutex, 0, 1);        // mutex = 1 since it a lock
    _queueList = new DynaList<T>();
}

template <class T> bool DynaWaitQueue<T>::add(T* obj, long msecs) {
    sem_wait(&_empty);
    sem_wait(&_mutex);  // scope of lock reduced to prevent deadlock
    _queueList->push(obj);
    sem_post(&_mutex);
    sem_post(&_full);
    return true;
}

template <class T> T* DynaWaitQueue::next(long msecs) {
    sem_wait(&_full);
    sem_wait(&_mutex);  // scope of lock reduced to prevent deadlock
    T* obj = _queueList->popLast();
    sem_post(&_mutex);
    sem_post(&_empty);
    return obj;
}




#endif