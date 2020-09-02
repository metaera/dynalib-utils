#include <atomic>
#include <thread>
#include <mutex>

using namespace std;


/**
 * @brief This macro provides 'Java-like' synchronized blocks in C++
 * 
 */
#define synchronized(M)  for(Lock M##_lock = M; M##_lock; M##_lock.setUnlock())


/**
 * @brief These macros provide 'Java-style' synchronization for methods in C++
 * 
 */
#define synch(M)  { Lock M##_lock = M; for (; M##_lock; M##_lock.setUnlock())
#define synch_end  }


class RecursiveLock {
private:
    int                _lockCnt;
    atomic<thread::id> _owner;  
    mutex              _mtx;

public:
    void acquire();
    void release();

};

class Mutex {
private:
    RecursiveLock* _recursiveLock;

public:
    //the default constructor
    Mutex();
    ~Mutex();
    void lock();
    void unlock();
};

class Lock {
private:
    Mutex& _mutex;
    bool   _locked;

public:
    Lock(Mutex &mutex);
    ~Lock();
    operator bool () const;
    void setUnlock();
};

template <typename F>
struct FinalAction {
    FinalAction(F f) : _clean(f), _enabled(true) {}
    ~FinalAction() { if (_enabled) _clean(); }
    void disable() { _enabled = false; };
private:
    F _clean;
    bool _enabled;
};

template <typename F>
FinalAction<F> _finally(F f) {
    return FinalAction<F>(f);
}

#define finally() auto _final_obj=_finally([&]
#define finally1(M) auto M##_final_obj=_finally([M]
#define finally2(M,M2) auto M##_final_obj=_finally([M,M2]
#define finally3(M,M2,M3) auto M##_final_obj=_finally([M,M2,M3]
#define finally4(M,M2,M3,M4) auto M##_final_obj=_finally([M,M2,M3,M4]
#define finally5(M,M2,M3,M4,M5) auto M##_final_obj=_finally([M,M2,M3,M4,M5]
#define end_finally );