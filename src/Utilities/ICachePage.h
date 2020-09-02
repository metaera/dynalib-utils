/**
 * @file ICachePage.h
 * @author Ken Kopelson (ken@metaera.com)
 * @brief 
 * @version 0.1
 * @date 2020-08-21
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef ICACHEPAGE_H
#define ICACHEPAGE_H

#include "ICacheConst.h"
#include "DynaBuffer.h"

template <typename T> class ICachePage {
protected:
    bool _isDirty = false;

public:
    ICachePage() = default;
    virtual ~ICachePage() = default;
    virtual void clear() = 0;
    virtual void clearDirty() = 0;
    virtual void clearPageDirty() = 0;
    virtual bool isDirty() = 0;
    virtual void setDirty(uint8_t dirtyBits) = 0;
    virtual void clearDirty(uint8_t dirtyBits) = 0;
    virtual index_t getKeyValue() = 0;
    virtual void setKeyValue(index_t keyValue) = 0;
    virtual void printPageReport(int pageSeqNo) = 0;
    virtual void printPageStatReport(int pageSeqNo) = 0;
};

#endif //ICACHEPAGE_H
