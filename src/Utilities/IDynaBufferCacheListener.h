//
// Created by Ken Kopelson on 26/04/18.
//

#ifndef IDYNABUFFERCACHELISTENER_H
#define IDYNABUFFERCACHELISTENER_H

#include "IDynaCacheListener.h"
#include "ICachePage.h"
#include "ICacheFile.h"

template <class T> class IDynaBufferCacheListener : public IDynaCacheListener<T> {
public:
    explicit IDynaBufferCacheListener(ICacheFile<T>* parent) : IDynaCacheListener<T>(parent) {};
    virtual ~IDynaBufferCacheListener() = default;
    virtual void postFlush(ICachePage<T>* page) = 0;
};

#endif //IDYNABUFFERCACHELISTENER_H
