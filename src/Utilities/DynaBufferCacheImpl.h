//
// Created by Ken Kopelson on 26/04/18.
//
#ifndef DYNABUFFERCACHEIMPL_H
#define DYNABUFFERCACHEIMPL_H

#include "DynaBufferCache.h"
#include "DynaCache.h"
#include "IDynaBufferCacheListener.h"

#define MAKE_BUFFERCACHETYPE_INSTANCE(C, T) \
    template class DynaBufferCache<C>; \
    typedef DynaBufferCache<C> T##BufferCache

template<class T>
DynaBufferCache<T>::DynaBufferCache(PageFile* file) : DynaCache<T>() {
    _file = file;
}

template<class T>
void DynaBufferCache<T>::setListener(IDynaCacheListener<T>* listener) {
    DynaCache<T>::setListener(listener);
//    throw IllegalArgumentException("Requires 'IDynaBufferCacheListener'");
}

//template<class T>
//void DynaBufferCache<T>::setListener(IDynaBufferCacheListener<T>* listener) {
//    DynaCache<T>::setListener(listener);
//}

template<class T>
void DynaBufferCache<T>::flushPage(T* page) {
    DynaBuffer* buffer = page->getBuffer();
    if (buffer != nullptr) {
        if (DynaCache<T>::_listener != nullptr)
            DynaCache<T>::_listener->onFlush(page);
        if (buffer->isDirty()) {
            _file->writePage(page->getPageIndex(), buffer);
            buffer->setIsDirty(false);
            ++DynaCache<T>::_pageFlushes;
        }
        if (DynaCache<T>::_listener != nullptr) {
            ((IDynaBufferCacheListener<T>*)DynaCache<T>::_listener)->postFlush(page);
        }
    }
}

template<class T>
void DynaBufferCache<T>::flushAndInvalidate(T* page) {
    flushPage(page);
    DynaBuffer* buffer = page->getBuffer();
    if (buffer != nullptr)
        buffer->invalidate();
}

template<class T>
T* DynaBufferCache<T>::getPageForRemoval(LinkedEntry<T>* entry) {
    T* page = entry->getObject();
    flushAndInvalidate(page);
    return page;
}

template<class T>
LinkedEntry<T>* DynaBufferCache<T>::useLastPage(Index keyValue, DynaBuffer* buffer) {
    LinkedEntry<T>* entry = DynaCache<T>::_pageList->getLast();
    if (entry != nullptr) {
        T* page = getPageForRemoval(entry);
        DynaCache<T>::_pageMap->remove(page->getKeyValue());
        page->clear();
        page->setKeyValue(keyValue);
        page->setBuffer(buffer);
        DynaCache<T>::_pageMap->put(keyValue, entry);
        DynaCache<T>::touchEntry(entry);
        ++DynaCache<T>::_pageReplacements;
    }
    return entry;
}

template<class T>
LinkedEntry<T>* DynaBufferCache<T>::putPageEntry(Index keyValue, DynaBuffer* buffer) {
    LinkedEntry<T>* entry = DynaCache<T>::findPageEntry(keyValue);
    if (entry == nullptr) {
        if (DynaCache<T>::_pageList->size() >= DynaCache<T>::_maxPages)
            entry = useLastPage(keyValue, buffer);
        if (entry == nullptr) {
            T* page = DynaCache<T>::getNewPage();
            page->setKeyValue(keyValue);
            page->setBuffer(buffer);
            entry = DynaCache<T>::addNewPage(page);
        }
    }
    else
        DynaCache<T>::touchEntry(entry);
    return entry;
}

template<class T>
T* DynaBufferCache<T>::putPage(Index keyValue, DynaBuffer* buffer) {
    LinkedEntry<T>* entry = putPageEntry(keyValue, buffer);
    return entry != nullptr ? entry->getObject() : nullptr;
}

// template<class T>
// void DynaBufferCache<T>::purgePages(double removePercent = 0.80) {
//     int pagesToRemove = (int)(((double)_pageList->count()) * removePercent);
//     for (int i = 0; i < pagesToRemove; ++i) {
//         removeLastPage();
//     }
// }


#endif