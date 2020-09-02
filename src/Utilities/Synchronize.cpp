#include "Synchronize.h"
#include <assert.h>
#include <iostream>

void RecursiveLock::acquire() {
    thread::id tid = this_thread::get_id();
    if (_owner == tid) { 
        _lockCnt++;
    } else {
        _mtx.lock();
        _owner   = tid;
        _lockCnt = 1;
    }
}

void RecursiveLock::release() {
    assert(_owner == this_thread::get_id());
    _lockCnt--;
    if (_lockCnt == 0) {
        _owner = thread::id();      // Invalid thread id
        _mtx.unlock();
    }
}

Mutex::Mutex() {
    _recursiveLock = new RecursiveLock();
}

Mutex::~Mutex() {
    delete _recursiveLock;
}

void Mutex::lock() {
    _recursiveLock->acquire();
}

void Mutex::unlock() {
    _recursiveLock->release();
}

Lock::Lock(Mutex &mutex) : _mutex(mutex), _locked(true) {
    _mutex.lock();
}

//the destructor
Lock::~Lock() {
    _mutex.unlock();
}

Lock::operator bool() const {
    return _locked;
}

void Lock::setUnlock() {
    _locked = false;        
}
