/**
 * @file IBufferCachePage.h
 * @author Ken Kopelson (ken@metaera.com)
 * @brief 
 * @version 0.1
 * @date 2020-09-19
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef IBUFFERCACHEPAGE_H
#define IBUFFERCACHEPAGE_H

#include "DynaBuffer.h"
#include "ICacheConst.h"
#include "ICachePage.h"

template <typename T> class IBufferCachePage : public ICachePage<T> {
protected:
    bool _isDirty = false;

public:
    IBufferCachePage() = default;
    virtual ~IBufferCachePage() = default;
    virtual bool        isFull() = 0;
    virtual index_t     getFileOffset() = 0;
    virtual DynaBuffer* getBuffer() = 0;
    virtual index_t     getPageIndex() = 0;
    virtual void        setFileOffset(index_t fileOffset) = 0;
    virtual void        setBuffer(DynaBuffer* buffer) = 0;
};

#endif //IBUFFERCACHEPAGE_H
