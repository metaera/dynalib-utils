//
// Created by Ken Kopelson on 27/04/18.
//

#ifndef HR1FIRMWARE_ICACHEFILE_H
#define HR1FIRMWARE_ICACHEFILE_H

#include "TypeDefs.h"
#include "ICacheConst.h"

template <class T> class ICacheFile {
public:
    ICacheFile() = default;
    virtual ~ICacheFile() = default;
    virtual void setFileName(const String& name) = 0;
    virtual String getFileName() = 0;
    virtual void init() = 0;
    virtual bool isOpen() = 0;
    virtual void clearCache() = 0;

    virtual void open() = 0;
    virtual void close() = 0;
    virtual bool isNewFile() = 0;
    virtual void resizeFile(index_t pageCount) = 0;
    virtual int  getMaxCachePages() = 0;
    virtual void purgePages(double removePercent) = 0;
    virtual index_t getPageCount() = 0;
    virtual index_t getUsedPages() = 0;
    virtual bool isUserHeaderDirty() = 0;
    virtual void setMaxCachePages(int maxPages) = 0;
    virtual void setUsedPages(index_t usedPaged) = 0;
    virtual void flushPage(T* page) = 0;
    virtual void flush() = 0;
    virtual void getUserHeader() = 0;
    virtual void writeUserHeader() = 0;
    virtual void printFileReport(bool longRep) = 0;
    virtual void printStatReport() = 0;
};

#endif //HR1FIRMWARE_ICACHEFILE_H
