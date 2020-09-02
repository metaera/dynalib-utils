#ifndef DYNAWAITQUEUE_H
#define DYNAWAITQUEUE_H

#include <semaphore.h>
#include "BitManip.h"
#include "CheckForError.h"
#include "Exception.h"
#include <atomic>

template <class T> class DynaWaitQueue {
protected:
    bool         _rethrow  = false;
    atomic<bool> _shutdown = false;
    atomic<bool> _complete = true;
    sem_t        _full;
    sem_t        _empty;
    sem_t        _mutex;
    atomic<int>  _maxSize   = NO_MAX_SIZE;
    DynaList<T>* _queueList = nullptr;

public:
    static int NO_MAX_SIZE;

    explicit DynaWaitQueue(int maxSize);
    virtual ~DynaWaitQueue();

    bool     add(T* obj, long msecs);
    T*       next(long msecs);
};

#endif