//
// Created by Ken Kopelson on 26/03/18.
//

#ifndef IDYNACACHELISTENER_H
#define IDYNACACHELISTENER_H

#include "ICachePage.h"
#include "ICacheFile.h"

template <class T> class IDynaCacheListener {
protected:
    ICacheFile<T>* _parent = nullptr;

public:
    explicit IDynaCacheListener(ICacheFile<T>* parent) : _parent(parent) {};
    virtual ~IDynaCacheListener() = default;
    virtual void onAddNew(ICachePage<T>* page) = 0;
    virtual void onFlush(ICachePage<T>* page) = 0;
};

#endif //IDYNACACHELISTENER_H
