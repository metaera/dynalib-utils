//
// Created by Ken Kopelson on 4/03/18.
//
#include <cstddef>
#include <unistd.h>
using namespace std;
#include "Exception.h"
#include "PageFile.h"
#include "Synchronize.h"

PageFile::PageFile(const String& fileName) : _file(nullptr) {
    if (!exists(fileName.c_str()))
        throw FileNotFoundException("File not found: " + fileName);
    if (!canAccess(fileName))
        throw DataFileException("File not accessible: " + fileName);
    open(fileName.c_str(), "r+b");
    readFileHeader();
    _isNewFile = false;
}

PageFile::PageFile(const String& fileName, uint userHeaderSize, uint pageSize) : _file(nullptr) {
    try {
        if (exists(fileName)) {
            if (!canAccess(fileName))
                throw DataFileException("File not accessible: " + fileName);
            if (!open(fileName, "r+b")) {
                throw DataFileException("File could not be opened in directory: " + fileName);
            }
            readFileHeader();
            #ifdef USE_LONG
                bool usingLong = true;
            #else
                bool usingLong = false;
            #endif
            if (_header.usesLongIndices != usingLong) {
                throw DataFileException("Pages files are not compatible index size format: " + fileName);
            }
            _isNewFile = false;
            if (_header.userHeaderSize != userHeaderSize || _header.pageSize != pageSize) {
                throw DataFileException("File header size and/or page size does not match: " + fileName);
            }
        }
        else {
            _header.versionMaj     = VERSION_MAJ;
            _header.versionMin     = VERSION_MIN;
            #ifdef USE_LONG
                _header.usesLongIndices = true;
            #else
                _header.usesLongIndices = false;
            #endif
            _header.userHeaderSize = userHeaderSize;
            _header.pageSize       = pageSize;
            _header.deletedCount   = 0;
            _header.firstDeleted   = -1;
            _header.lastDeleted    = -1;
            _userHeaderBuf.setBufferSize(_header.userHeaderSize);
            _userHeaderBuf.invalidate();    // Zero-out the buffer
            if (!open(fileName, "w+b")) {
                perror("");
                auto err = errno;
                throw DataFileException("File could not be created in directory: " + fileName + ", errno: " + errno);
            }
            writeFileHeader();
            _isNewFile = true;
        }
    }
    catch (Exception& e) {
        auto msg = e.getMessage();
    }
}

PageFile::~PageFile() {
    close();
}

bool PageFile::exists(const String& fileName) {
    return access(fileName.c_str(), F_OK) == 0;
}

bool PageFile::canAccess(const String& fileName) {
    return access(fileName.c_str(), R_OK | W_OK) == 0;
}


bool PageFile::deleteFile(const String& fileName) {
    return remove(fileName.c_str()) == 0;
}

bool PageFile::renameFile(const String& oldName, const String& newName) {
    return rename(oldName.c_str(), newName.c_str()) == 0;
}

bool PageFile::isOpen() {
    return _file != nullptr;
}

bool PageFile::isNewFile() {
    return _isNewFile;
}

bool PageFile::open(const String& fileName, const char* mode) {
    close();
    _file = fopen(fileName.c_str(), mode);
    return isOpen();
}

void PageFile::close() {
    if (isOpen()) {
        fclose(_file);
        _file      = nullptr;
        _isNewFile = false;
    }
}


void PageFile::resizeFile(index_t pageCount) {
    index_t curPageCount = getPageCount();
    if (pageCount > curPageCount) {
        fflush(_file);
        index_t newLength = sizeof(FileHeader) + _header.userHeaderSize + (pageCount * _header.pageSize);
        fseek(_file, newLength, SEEK_SET);
        fflush(_file);
        clearPages(curPageCount, pageCount - 1);
    }
    else if (pageCount < curPageCount) {
        fflush(_file);
        long long newLength = sizeof(FileHeader) + _header.userHeaderSize + (pageCount * _header.pageSize);
        ftruncate(fileno(_file), newLength);
    }
}

FileHeader& PageFile::getHeader() {
    return _header;
}

DynaBuffer* PageFile::getNewPageBuffer() {
    auto* buffer = new DynaBuffer(_header.pageSize, true);
    _pageHeaderRef = (PageHeader*)buffer->getInternalTypedArray();
    _pageHeaderRef->deletedFlag = false;
    _pageHeaderRef->nextDeleted = INVALID_INDEX;
    buffer->setBufEnd(0);
    return buffer;
}

DynaBuffer* PageFile::getPageBufferCopy(DynaBuffer* buffer) {
    return buffer->copy();
}

index_t PageFile::getPageHeaderOffset(index_t pageIndex) {
    return sizeof(FileHeader) + _header.userHeaderSize + (pageIndex * _header.pageSize);
}

void PageFile::rewind() {
    ::rewind(_file);
}

bool PageFile::readFileHeader() {
    if (isOpen()) {
        ::rewind(_file);
        size_t count = fread(&_header, 1, sizeof(FileHeader), _file);
        if (count == sizeof(FileHeader)) {
            _userHeaderBuf.setBufferSize(_header.userHeaderSize);
            bool result = true;
            if (_header.userHeaderSize > 0)
                result = readUserHeader(false);
            return result;
        }
        else
            throw DataFileException("Invalid file header size...file corrupted");
    }
    return false;
}

bool PageFile::writeFileHeader() {
    if (isOpen()) {
        ::rewind(_file);
        size_t count = fwrite(&_header, 1, sizeof(FileHeader), _file);
        if (count == sizeof(FileHeader)) {
            if (_header.userHeaderSize > 0)
                writeUserHeader(false);
            else
                fflush(_file);
            return true;
        }
        else
            throw DataFileException("Error writing file header...");
    }
    return false;
}

index_t PageFile::seekToUserHeader() {
    index_t offset = getUserHeaderOffset();
    fseek(_file, offset, SEEK_SET);
    return offset;
}

bool PageFile::readUserHeader(bool doSeek) {
    if (isOpen()) {
        if (doSeek)
            seekToUserHeader();
        if (_header.userHeaderSize > 0) {
            size_t count = fread(_userHeaderBuf.getInternalTypedArray(), 1, _header.userHeaderSize, _file);
            if (count != _header.userHeaderSize)
                throw DataFileException("Invalid user header size...file corrupted");
        }
        return true;
    }
    return false;
}

bool PageFile::writeUserHeader(bool doSeek) {
    if (isOpen()) {
        if (doSeek)
            seekToUserHeader();
        if (_header.userHeaderSize > 0) {
            size_t count = fwrite(_userHeaderBuf.getInternalTypedArray(), 1, _header.userHeaderSize, _file);
            if (count != _header.userHeaderSize)
                throw DataFileException("Error writing user header...");
            fflush(_file);
        }
        return true;
    }
    return false;
}

index_t PageFile::seekToPageHeader(index_t pageIndex, int adjust = 0) {
    index_t offset = getPageHeaderOffset(pageIndex) + adjust;
    fseek(_file, offset, SEEK_SET);
    return offset;
}

bool PageFile::readPageHeader(index_t pageIndex) {
    if (isOpen()) {
        seekToPageHeader(pageIndex);
        size_t result = fread(&_pageHeader, 1, sizeof(PageHeader), _file);
        if (result == sizeof(PageHeader)) {
            if (_pageHeader.deletedFlag)
                throw DataFileException("Page " + to_string(pageIndex) + " is marked deleted...");
            return true;
        }
    }
    return false;
}

bool PageFile::writePageHeader(index_t pageIndex) {
    if (isOpen()) {
        if (_pageHeader.deletedFlag)
            throw DataFileException("Page " + to_string(pageIndex) + " is deleted...cannot write header...");
        _pageHeader.deletedFlag = false;
        _pageHeader.nextDeleted = -1;
        seekToPageHeader(pageIndex);
        size_t result = fwrite(&_pageHeader, 1, sizeof(PageHeader), _file);
        fflush(_file);
        return result == sizeof(PageHeader);
    }
    return false;
}

uint8_t* PageFile::getUserHeaderPointer() {
    if (_header.userHeaderSize > 0)
        return _userHeaderBuf.getInternalTypedArray();
    return nullptr;
}

void PageFile::deletePage(index_t pageIndex, DynaBuffer* buffer) {
    _pageHeaderRef = (PageHeader*)buffer->getInternalTypedArray();
    if (_pageHeaderRef->deletedFlag)
        throw DataFileException("Page " + to_string(pageIndex) + " is already deleted...");

    _pageHeaderRef->deletedFlag = true;
    _pageHeaderRef->nextDeleted = _header.lastDeleted;
    writePage(pageIndex, buffer, 0, sizeof(PageHeader));

    if (_header.firstDeleted == -1)
        _header.firstDeleted = pageIndex;
    _header.lastDeleted = pageIndex;
    ++_header.deletedCount;
    writeFileHeader();
}

void PageFile::deletePage(index_t pageIndex) {
    if (readPageHeader(pageIndex)) {
        if (_pageHeader.deletedFlag)
            throw DataFileException("Page " + to_string(pageIndex) + " is already deleted...");
        _pageHeader.deletedFlag = true;
        _pageHeader.nextDeleted = _header.lastDeleted;
        seekToPageHeader(pageIndex);
        size_t result = fwrite(&_pageHeader, 1, sizeof(PageHeader), _file);
        fflush(_file);
        if (result == sizeof(PageHeader)) {
            if (_header.firstDeleted == -1)
                _header.firstDeleted = pageIndex;
            _header.lastDeleted = pageIndex;
            ++_header.deletedCount;
            writeFileHeader();
        }
    }
}

void PageFile::_clearPage(index_t pageIndex, DynaBuffer* buffer) {
    buffer->invalidate();
    _pageHeaderRef = (PageHeader*)buffer->getInternalRawArray();
    _pageHeaderRef->deletedFlag = false;
    _pageHeaderRef->nextDeleted = -1;
    writePage(pageIndex, buffer);
}

void PageFile::clearPage(index_t pageIndex, DynaBuffer* buffer) {
    _pageHeaderRef = (PageHeader*)buffer->getInternalRawArray();
    if (_pageHeaderRef->deletedFlag)
        throw DataFileException("Page " + to_string(pageIndex) + " is marked deleted...");
    _clearPage(pageIndex, buffer);
}

void PageFile::clearPages(index_t frPageIndex, index_t toPageIndex) {
    DynaBuffer* buffer = getNewPageBuffer();
    for (long i = frPageIndex; i <= toPageIndex; ++i) {
        clearPage(i, buffer);
    }
    finally(){
        /**
         * The buffer from getNewPageBuffer is "new'd" so we need to delete it here
         */
        delete buffer;
    }
    end_finally
}

bool PageFile::readPage(index_t pageIndex, DynaBuffer* buffer) {
    seekToPageHeader(pageIndex);
    size_t count = fread(buffer->getInternalTypedArray(), 1, _header.pageSize, _file);
    buffer->setPos(0);
    buffer->setBufEnd((uint)count);
    if (count == _header.pageSize) {
        // Relies on setPos(0) above to position at the start of the buffer
        _pageHeaderRef = (PageHeader*)buffer->getInternalTypedArray();
        if (_pageHeaderRef->deletedFlag)
            throw DataFileException("Page " + to_string(pageIndex) + " is marked deleted...");
        return true;
    }
    return false;
}

bool PageFile::readNextPage(DynaBuffer* buffer) {
    size_t count = fread(buffer->getInternalTypedArray(), 1, _header.pageSize, _file);
    buffer->setPos(0);
    buffer->setBufEnd((uint)count);
    if (count == _header.pageSize) {
        // Relies on setPos(0) above to position at the start of the buffer
        _pageHeaderRef = (PageHeader*)buffer->getInternalTypedArray();
        if (_pageHeaderRef->deletedFlag)
            throw DataFileException("Page is marked deleted...");
        return true;
    }
    return false;

}

/**
 * @brief writes a portion of the passed in buffer to the corresponding location in the page on disk.
 * 
 * @param pageIndex 
 * @param buffer - a DynaBuffer holding a complete page from the file
 * @param bufOffset - starting location in buffer (and the same position on the underlying page)
 * @param length - the number of bytes to write
 */
void PageFile::writePage(index_t pageIndex, DynaBuffer* buffer, int bufOffset, size_t length) {
    seekToPageHeader(pageIndex, bufOffset);
    auto count = fwrite(buffer->getInternalTypedArray() + bufOffset, 1, length, _file);
    fflush(_file);
}

void PageFile::writePage(index_t pageIndex, DynaBuffer* buffer) {
    seekToPageHeader(pageIndex);
    auto count = fwrite(buffer->getInternalTypedArray(), 1, _header.pageSize, _file);
    fflush(_file);
}

void PageFile::flushPage(index_t pageIndex, DynaBuffer* buffer) {
    seekToPageHeader(pageIndex);
    fwrite(buffer->getInternalTypedArray(), 1, _header.pageSize, _file);
    fflush(_file);
    _clearPage(pageIndex, buffer);
}

void PageFile::flushNextPage(DynaBuffer* buffer) {
    fwrite(buffer->getInternalTypedArray(), 1, _header.pageSize, _file);
    fflush(_file);
}


index_t PageFile::getPageCount() {
    fseek(_file, 0, SEEK_END);
    long fileSize = ftell(_file);
    return (index_t)(fileSize - sizeof(FileHeader) - _header.userHeaderSize) / _header.pageSize;
}

index_t PageFile::getValidPageCount() {
    return getPageCount() - _header.deletedCount;
}

index_t PageFile::getFreePage(DynaBuffer* buffer) {
    index_t pageIndex = INVALID_INDEX;
    if (_header.lastDeleted >= 0) {
        pageIndex = _header.lastDeleted;
        seekToPageHeader(pageIndex);
        size_t result = fread(buffer->getInternalTypedArray(), 1, _header.pageSize, _file);
        if (result == _header.pageSize) {
            _pageHeaderRef = (PageHeader*)buffer->getInternalTypedArray();
            if (!_pageHeaderRef->deletedFlag)
                throw DataFileException("Page " + to_string(pageIndex) + " not valid deleted page...");
            _header.lastDeleted = _pageHeaderRef->nextDeleted;
            --_header.deletedCount;
            writeFileHeader();
            _clearPage(pageIndex, buffer);  // Will clear the deleted status of the page
        }
    }
    else {
        pageIndex = getPageCount();
        resizeFile(pageIndex + 1);
        readPage(pageIndex, buffer);
    }
    return pageIndex;
}

