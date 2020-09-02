//
// Created by Ken Kopelson on 4/03/18.
//

#ifndef PAGEFILE_H
#define PAGEFILE_H

#include <cstdio>
#include "String.h"
#include "DynaBuffer.h"
#include "ICacheConst.h"

using namespace std;

#define VERSION_MAJ 1
#define VERSION_MIN 0

#pragma pack(push, 1)
struct FileHeader {
    int16_t   versionMaj;
    int16_t   versionMin;
    bool      usesLongIndices;
    uint32_t  userHeaderSize;
    uint32_t  pageSize;
    index_t   deletedCount;
    index_t   firstDeleted;
    index_t   lastDeleted;
};

struct PageHeader {
    bool      deletedFlag;
    index_t   nextDeleted;
};
#pragma pack(pop)

class PageFile {
    FILE*       _file = nullptr;
    FileHeader  _header;
    DynaBuffer  _userHeaderBuf;
    PageHeader  _pageHeader;
    PageHeader* _pageHeaderRef = nullptr;
    bool        _isNewFile = false;

public:
    explicit PageFile(const String& fileName);
    explicit PageFile(const String& fileName, uint userHeaderSize, uint pageSize);
    virtual ~PageFile();

    static bool exists(const String& fileName);
    static bool canAccess(const String& fileName);
    static bool deleteFile(const String& fileName);
    static bool renameFile(const String& oldName, const String& newName);

    bool    isOpen();
    bool    isNewFile();
    bool    open(const String& fileName, const char* mode);
    void    close();
    short   getVersionMaj()       { return _header.versionMaj; }
    short   getVersionMin()       { return _header.versionMin; }
    uint    getUserHeaderSize()   { return _header.userHeaderSize; }
    index_t getPageSize()         { return _header.pageSize; }
    index_t getDeletedPageCount() { return _header.deletedCount; }

    void resizeFile(index_t pageCount);

    FileHeader& getHeader();
    DynaBuffer* getNewPageBuffer();
    DynaBuffer* getPageBufferCopy(DynaBuffer* buffer);

    index_t getUserHeaderOffset() { return sizeof(FileHeader); }
    index_t getPageHeaderOffset(index_t pageIndex);

    void    rewind();
    bool    readFileHeader();
    bool    writeFileHeader();
    index_t seekToUserHeader();
    bool    readUserHeader(bool doSeek = true);
    bool    writeUserHeader(bool doSeek = true);
    index_t seekToPageHeader(index_t pageIndex, int adjust);
    bool    readPageHeader(index_t pageIndex);
    bool    writePageHeader(index_t pageIndex);

    uint8_t* getUserHeaderPointer();
    void deletePage(index_t pageIndex, DynaBuffer* buffer);
    void deletePage(index_t pageIndex);
    void _clearPage(index_t pageIndex, DynaBuffer* buffer);
    void clearPage(index_t pageIndex, DynaBuffer* buffer);
    void clearPages(index_t frPageIndex, index_t toPageIndex);
    bool readPage(index_t pageIndex, DynaBuffer* buffer);
    bool readNextPage(DynaBuffer* buffer);
    void writePage(index_t pageIndex, DynaBuffer* buffer, int bufOffset, size_t length);
    void writePage(index_t pageIndex, DynaBuffer* buffer);
    void flushPage(index_t pageIndex, DynaBuffer* buffer);
    void flushNextPage(DynaBuffer* buffer);
    index_t getPageCount();
    index_t getValidPageCount();
    index_t getFreePage(DynaBuffer* buffer);
};


#endif //DATAFILE_H
