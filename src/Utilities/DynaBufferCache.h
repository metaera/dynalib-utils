//
// Created by Ken Kopelson on 26/04/18.
//

#ifndef DYNABUFFERCACHE_H
#define DYNABUFFERCACHE_H

#include "PageFile.h"
#include "DynaCache.h"
//#include "Utilities/IDynaBufferCacheListener.h"

#define MAKE_BUFFERCACHETYPE_DEF(C, T) \
    typedef DynaBufferCache<C> T##BufferCache

template <class T> class DynaBufferCache : public DynaCache<T> {
    PageFile* _file = nullptr;

public:
    explicit DynaBufferCache(PageFile* file);

    void setListener(IDynaCacheListener<T>* listener) override;
    void flushPage(T* page) override;
    void flushAndInvalidate(T* page);
    T*   getPageForRemoval(LinkedEntry<T>* entry) override;
    LinkedEntry<T>* useLastPage(Index keyValue, DynaBuffer* buffer);
    LinkedEntry<T>* putPageEntry(Index keyValue, DynaBuffer* buffer);
    T*   putPage(Index keyValue, DynaBuffer* buffer);
};


#endif //DYNABUFFERCACHE_H
