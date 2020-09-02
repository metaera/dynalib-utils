//
// Created by Ken Kopelson on 6/03/18.
//

#ifndef DYNACACHEIMPL_H
#define DYNACACHEIMPL_H

#include "DynaCache.h"
#include "ICachePage.h"
#include "ICacheFile.h"

#define MAKE_CACHETYPE_INSTANCE(C, T) \
    template class DynaCache<C>; \
    typedef DynaCache<C> T##Cache

template<class T>
DynaCache<T>::DynaCache() {
    _pageList = new DynaLinkedList<T>();
    _pageList->setOwnsMembers(true);
    _pageMap  = new DynaHashMap<Index, LinkedEntry<T>>();
    _pageMap->setOwnsMembers(false);    // Map holds references only
}

template<class T>
DynaCache<T>::~DynaCache() {
    // TODO: trying flush here instead of "clear"
    // clear();
    flush();
    delete _pageMap;
    _pageMap  = nullptr;
    delete _pageList;
    _pageList = nullptr;
    delete _listener;
    _listener = nullptr;
}

template<class T>
DynaCache<T>::DynaCache(const DynaCache<T>& other) {
    _autoTouch = other._autoTouch;
    _maxPages  = other._maxPages;
    _pageList  = other._pageList->copy();
    _pageMap   = other._pageMap->copy();
    _listener  = other._listener;
}

template<class T>
DynaCache<T>* DynaCache<T>::copy() {
    return new DynaCache<T>(*this);
}

template<class T>
void DynaCache<T>::setAutoTouch(bool autoTouch) {
    _autoTouch = autoTouch;
}

template<class T>
bool DynaCache<T>::isAutoTouch() {
    return _autoTouch;
}

template<class T>
void DynaCache<T>::resetStats() {
    _pageFlushes      = 0L;
    _pageReplacements = 0L;
    _pageAccesses     = 0L;
}

template<class T>
void DynaCache<T>::clear() {
    flush();
    _pageMap->clear();
    _pageList->clear();
}

template<class T>
bool DynaCache<T>::isEmpty() {
    return _pageList == nullptr || _pageList->size() == 0;
}

template<class T>
bool DynaCache<T>::isFull() {
    return _pageList != nullptr && _pageList->size() >= _maxPages;
}

template<class T>
bool DynaCache<T>::isDirty() {
    if (_pageList != nullptr) {
        for (T* page : *_pageList) {
            if (page->isDirty())
                return true;
        }
    }
    return false;
}

template<class T>
int DynaCache<T>::getDirtyPageCount() {
    int count = 0;
    if (_pageList != nullptr) {
        for (T* page : *_pageList) {
            if (page->isDirty())
                ++count;
        }
    }
    return count;
}

template<class T>
int DynaCache<T>::getMaxPages() {
    return _maxPages;
}

template<class T>
int DynaCache<T>::getPageCount() {
    return _pageList->size();
}

template<class T>
DynaLinkedList<T>* DynaCache<T>::getPageList() {
    return _pageList;
}

template<class T>
void DynaCache<T>::setMaxPages(int maxPages) {
    if (maxPages < _maxPages) {
        for (int i = maxPages; i < _maxPages; ++i)
            removeLastPage();
    }
    _maxPages = maxPages;
}

template<class T>
void DynaCache<T>::setListener(IDynaCacheListener<T>* listener) {
    _listener = listener;
}

template<class T>
void DynaCache<T>::flush() {
    if (_pageList != nullptr && !_pageList->isEmpty()) {
        for (T* page : *_pageList) {
            flushPage(page);
        }
    }
}

template<class T>
void DynaCache<T>::flushPage(T* page) {
    if (_listener != nullptr)
        _listener->onFlush((ICachePage<T>*)page);
}

template<class T>
void DynaCache<T>::touchEntry(LinkedEntry<T>* entry) {
    if (_pageList != nullptr) {
        _pageList->moveToFirst(entry);
        ++_pageAccesses;
    }
}

template<class T>
T* DynaCache<T>::getNewPage() {
    try {
        return new T();
    }
    catch (Exception& e) {
    }
    return nullptr;
}

template<class T>
LinkedEntry<T>* DynaCache<T>::addNewPage(T* page) {
    auto* entry = _pageList->pushEntry(page);
    ++_pageAccesses;
    _pageMap->put(page->getKeyValue(), entry);
    if (_listener != nullptr)
        _listener->onAddNew((ICachePage<T>*)page);
    return entry;
}

template<class T>
T* DynaCache<T>::getPageForRemoval(LinkedEntry<T>* entry) {
    T* page = entry->getObject();
    flushPage(page);
    return page;
}

template<class T>
LinkedEntry<T>* DynaCache<T>::useLastPage(Index keyValue) {
    auto* entry = _pageList->getLast();
    if (entry != nullptr) {
        T* page = getPageForRemoval(entry);
        _pageMap->remove(page->getKeyValue());      // Use 'remove' because the map doesn't own the entry
        page->clear();
        page->setKeyValue(keyValue);
        _pageMap->put(keyValue, entry);
        /**
         * This method reuses the "page" object inside the list Entry, so we don't delete it
         */
        touchEntry(entry);
        ++_pageReplacements;
    }
    return entry;
}

template<class T>
LinkedEntry<T>* DynaCache<T>::useLastPage(T* newPage) {
    auto* entry = _pageList->getLast();
    if (entry != nullptr) {
        T* page = getPageForRemoval(entry);
        _pageMap->remove(page->getKeyValue());
        entry->setObject(newPage);
        _pageMap->put(newPage->getKeyValue(), entry);
        if (_pageList->isOwnsMembers()) {
            delete page;   // The old page is no longer needed and must be deleted
        }
        touchEntry(entry);
        ++_pageReplacements;
    }
    return entry;
}

template<class T>
void DynaCache<T>::removePage(Index keyValue) {
    auto* entry = findPageEntry(keyValue);
    if (entry != nullptr) {
        T* page = getPageForRemoval(entry);
        _pageMap->remove(page->getKeyValue());        
        _pageList->deleteEntry(entry);
    }
}

template<class T>
void DynaCache<T>::removeLastPage() {
    auto* entry = _pageList->getLast();
    if (entry != nullptr) {
        T* page = getPageForRemoval(entry);
        _pageMap->remove(page->getKeyValue());
        _pageList->deleteEntry(entry);  // Will delete the page since the list owns it
    }
}

template<class T>
void DynaCache<T>::purgePages(double removePercent) {
    int pagesToRemove = (int)(((double)_pageList->count()) * removePercent);
    for (int i = 0; i < pagesToRemove; ++i) {
        removeLastPage();
    }
}


template<class T>
LinkedEntry<T>* DynaCache<T>::findPageEntry(Index keyValue) {
    auto* entry = _pageMap->get(keyValue);
    return entry;
}

template<class T>
LinkedEntry<T>* DynaCache<T>::getPageEntry(Index keyValue) {
    auto* entry = findPageEntry(keyValue);
    if (entry != nullptr && _autoTouch)
        touchEntry(entry);
    return entry;
}

template<class T>
LinkedEntry<T>* DynaCache<T>::putPageEntry(Index keyValue) {
    auto* entry = findPageEntry(keyValue);
    if (entry ==  nullptr) {
        if (_pageList->size() >= _maxPages)
            entry = useLastPage(keyValue);
        if (entry == nullptr) {
            T* page = getNewPage();
            page->setKeyValue(keyValue);
            entry = addNewPage(page);
        }
    }
    else
        touchEntry(entry);
    return entry;
}

template<class T>
LinkedEntry<T>* DynaCache<T>::putPageEntry(T* page) {
    auto* entry = findPageEntry(page->getKeyValue());
    if (entry ==  nullptr) {
        if (_pageList->size() >= _maxPages)
            entry = useLastPage(page);
        if (entry == nullptr)
            entry = addNewPage(page);
    }
    else
        touchEntry(entry);
    return entry;
}

template<class T>
T* DynaCache<T>::findPage(Index keyValue) {
    auto* entry = findPageEntry(keyValue);
    return entry != nullptr ? entry->getObject() : nullptr;
}

template<class T>
T* DynaCache<T>::getPage(Index keyValue) {
    auto* entry = getPageEntry(keyValue);
    return entry != nullptr ? entry->getObject() : nullptr;
}

template<class T>
T* DynaCache<T>::putPage(Index keyValue) {
    auto* entry = putPageEntry(keyValue);
    return entry != nullptr ? entry->getObject() : nullptr;
}

template<class T>
T* DynaCache<T>::putPage(T* page) {
    auto* entry = putPageEntry(page);
    return entry != nullptr ? entry->getObject() : nullptr;
}

template<class T>
void DynaCache<T>::printStatReport(String& name) {
    try {
        cout << "\n---------------------- Cache Status -------------------------" << endl;
        if (name != "") {
            cout << "File Name             : " << name << endl;
        }
        cout << "Configured Max. Pages : " << getMaxPages() << endl;
        cout << "Total Pages Loaded    : " << _pageList->size() << endl;
        cout << "Total Dirty Pages     : " << getDirtyPageCount() << endl;
        cout << "\n----------------------- Statistics --------------------------" << endl;
        cout << "Page Flushes          : " << _pageFlushes << endl;
        cout << "Page Accesses         : " << _pageAccesses << endl;
        cout << "Page Replacements     : " << _pageReplacements << endl;
        cout << endl;
    }
    catch (Exception& e) {
    }
}

#endif