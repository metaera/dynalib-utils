//
// Created by Ken Kopelson on 6/03/18.
//

#ifndef DYNACACHE_H
#define DYNACACHE_H

#include "DynaLinkedList.h"
#include "IntWrapper.h"
#include "DynaHashMap.h"
#include "IDynaCacheListener.h"

#define MAKE_CACHETYPE_DEF(C, T) \
    typedef DynaCache<C> T##Cache

template <class T> class DynaCache : public ICopyable<DynaCache<T>> {
    bool                                  _autoTouch = false;
protected:
    int                                   _maxPages = 0;
    DynaLinkedList<T>*                    _pageList = nullptr;
    DynaHashMap<Index, LinkedEntry<T>>*   _pageMap = nullptr;
    IDynaCacheListener<T>*                _listener = nullptr;

    long _pageFlushes      = 0;
    long _pageReplacements = 0;
    long _pageAccesses     = 0;

public:
    DynaCache();
    virtual ~DynaCache();
    DynaCache(const DynaCache<T>& other);
    DynaCache<T>* copy() override;

    void setAutoTouch(bool autoTouch);
    bool isAutoTouch();
    void resetStats();
    void clear();
    bool isEmpty();
    bool isFull();
    bool isDirty();
    int  getMaxPages();
    int  getPageCount();
    int  getDirtyPageCount();
    DynaLinkedList<T>* getPageList();
    void setMaxPages(int maxPages);
    virtual void setListener(IDynaCacheListener<T>* listener);
    void flush();
    virtual void flushPage(T* page);
    void touchEntry(LinkedEntry<T>* entry);
    T*   getNewPage();
    LinkedEntry<T>* addNewPage(T* page);
    virtual T*   getPageForRemoval(LinkedEntry<T>* entry);
    LinkedEntry<T>* useLastPage(Index keyValue);
    LinkedEntry<T>* useLastPage(T* newPage);
    void removePage(Index keyValue);
    void removeLastPage();
    virtual void purgePages(double removePercent = 0.80);
    LinkedEntry<T>* findPageEntry(Index keyValue);
    LinkedEntry<T>* getPageEntry(Index keyValue);
    LinkedEntry<T>* putPageEntry(Index keyValue);
    LinkedEntry<T>* putPageEntry(T* page);
    T* findPage(Index keyValue);
    T* getPage(Index keyValue);
    T* putPage(Index keyValue);
    T* putPage(T* page);
    void printStatReport(String& name);
};


#endif //DYNACACHE_H
