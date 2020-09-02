/*
 * RingBuffer.cpp
 *
 *  Created on: 31/08/2017
 *      Author: ken
 */

#include <cstdio>
#include <string>
#include "ICacheConst.h"
#include "DynaBuffer.h"
#include "Exception.h"

bool DynaBuffer::THROW_EXCEPTIONS = true;

DynaBuffer::DynaBuffer() : _buffer(nullptr), _bufSize(0), _bufEnd(0), _bufPos(0), _isDirty(false) {
}

DynaBuffer::DynaBuffer(uint size, bool zeroOut) : DynaBuffer() {
    setBufferSize(size);
    if (zeroOut) {
      zeroFill();
    }
}

DynaBuffer::~DynaBuffer() {
    delete _buffer;
}

DynaBuffer::DynaBuffer(const DynaBuffer& other) {
    _buffer  = nullptr;
    _bufEnd  = other.getBufEnd();
    _bufPos  = other.getPos();
    _isDirty = false;
    setBufferSize(other.getBufferSize());
    memcpy(_buffer, other._buffer, _bufSize);
}

DynaBuffer* DynaBuffer::copy() {
    return new DynaBuffer(*this);
}

void DynaBuffer::zeroFill(int fromIndex, int toIndex) {
    if (toIndex < 0)
        toIndex = _bufSize - 1;
    if (toIndex >= fromIndex)
        memset(_buffer + fromIndex, 0, (size_t)(toIndex - fromIndex) + 1);
}

void DynaBuffer::clear() {
    if (!isEmpty()) {
        _bufPos = _bufEnd = 0;
        _isDirty = true;
    }
}

void DynaBuffer::reset() {
    _bufPos = 0;
}

bool DynaBuffer::isDirty() {
    return _isDirty;
}

void DynaBuffer::setIsDirty(bool isDirty) {
    _isDirty = isDirty;
}

uint DynaBuffer::getPos() const {
    return _bufPos;
}

bool DynaBuffer::setPos(uint pos) {
    if (pos <= _bufEnd) {
        _bufPos = pos;
        return true;
    }
    return false;
}

uint DynaBuffer::adjustPos(int delta) {
    _bufPos = (delta < 0 && -delta > _bufPos) ? 0 : _bufPos += delta;
    return _bufPos;
}

bool DynaBuffer::insertRegion(int index, int count) {
    int toIndex = index + count;
    int lastIdx = _bufEnd - count;
    if (toIndex < lastIdx) {
        memmove(_buffer + toIndex, _buffer + index, lastIdx - index);
        _isDirty = true;
        return true;
    }
    return false;
}

bool DynaBuffer::deleteRegion(int index, int count) {
    int frIndex = index + count;
    if (frIndex <= _bufEnd) {
        if (frIndex < _bufEnd) {
            memmove(_buffer + index, _buffer + frIndex, _bufEnd - frIndex);
        }
        zeroFill(_bufEnd - frIndex);
        _isDirty = true;
        return true;
    }
    return false;
}

void DynaBuffer::invalidate() {
    _bufPos = 0;
    _bufEnd = 0;
    memset(_buffer, 0, _bufSize);
}

bool DynaBuffer::isEmpty() {
    return _bufEnd == 0;
}

bool DynaBuffer::isFull() {
    return _bufEnd == _bufSize;
}

bool DynaBuffer::hasCapacity() {
    return _bufPos < _bufSize;
}

bool DynaBuffer::hasRemainingCapacity(uint count) {
    return getRemainingCapacity() >= count;
}

uint DynaBuffer::getRemainingCapacity() const {
    return _bufPos < _bufSize ? _bufSize - _bufPos : 0;
}

bool DynaBuffer::hasRemaining(uint count) {
    return getRemaining() >= count;
}

uint DynaBuffer::getRemaining() const {
    return _bufPos < _bufEnd ? _bufEnd - _bufPos : 0;
}

bool DynaBuffer::requiresRemaining(uint count) {
    if (hasRemaining(count))
        return true;
    else if (THROW_EXCEPTIONS)
        throw BufferUnderrunException();
    return false;
}

uint DynaBuffer::getBufEnd() const {
    return _bufEnd;
}

void DynaBuffer::setBufEnd(uint bufEnd) {
    if (bufEnd <= _bufSize)
        _bufEnd = bufEnd;
}

uint DynaBuffer::getBufferSize() const {
    return _bufSize;
}

void DynaBuffer::setBufferSize(uint bufSize) {
    if (bufSize != _bufSize) {
        auto* newBuffer = new uint8_t[ bufSize ];
        if (_buffer != nullptr) {
            uint copyCount = bufSize > _bufSize ? _bufSize : bufSize;
            memcpy(newBuffer, _buffer, copyCount);
            delete _buffer;
        }
        _bufSize = bufSize;
        _buffer  = newBuffer;
        if (_bufEnd > _bufSize)
            invalidate();
    }
}

bool DynaBuffer::hasNextByte() {
    return _bufPos < _bufEnd;
}

uint8_t DynaBuffer::getNextByte() {
    if (_bufPos < _bufEnd) {
        return _buffer[_bufPos++];
    }
    else if (THROW_EXCEPTIONS)
        throw BufferUnderrunException();
    return 0;
}

uint8_t DynaBuffer::popLastByte() {
    if (_bufPos > 0) {
        return _buffer[--_bufPos];
    }
    else if (THROW_EXCEPTIONS)
        throw BufferUnderrunException();
    return 0;
}

short DynaBuffer::getNextShort() {
    return (short)getNextUShort();
}

ushort DynaBuffer::getNextUShort() {
    ushort value = 0;
    if (requiresRemaining(sizeof(ushort))) {
        value = *(ushort*)(_buffer + _bufPos);
        _bufPos += sizeof(ushort);
    }
    return value;
}

int DynaBuffer::getNextInt() {
    return (int)getNextUInt();
}

uint DynaBuffer::getNextUInt() {
    uint value = 0;
    if (requiresRemaining(sizeof(uint))) {
        value = *(uint*)(_buffer + _bufPos);
        _bufPos += sizeof(uint);
    }
    return value;
}

long DynaBuffer::getNextLong() {
    return (long)getNextULong();
}

ulong DynaBuffer::getNextULong() {
    ulong value = 0;
    if (requiresRemaining(sizeof(ulong))) {
        value = *(ulong*)(_buffer + _bufPos);
        _bufPos += sizeof(ulong);
    }
    return value;
}

long long DynaBuffer::getNextLongLong() {
    return (long long)getNextULongLong();
}

ulonglong DynaBuffer::getNextULongLong() {
    ulonglong value = 0;
    if (requiresRemaining(sizeof(ulonglong))) {
        value = *(ulonglong*)(_buffer + _bufPos);
        _bufPos += sizeof(ulonglong);
    }
    return value;
}

index_t DynaBuffer::getNextIndex() {
    index_t value = 0;
    if (requiresRemaining(sizeof(index_t))) {
        value = *(index_t*)(_buffer + _bufPos);
        _bufPos += sizeof(index_t);
    }
    return value;
}

UUID DynaBuffer::getNextUUID() {
    uint64_t msb = 0;
    uint64_t lsb = 0;
    if (requiresRemaining(UUID_SIZE)) {
        msb = *(uint64_t*)(_buffer + _bufPos);
        _bufPos += sizeof(uint64_t);
        lsb = *(uint64_t*)(_buffer + _bufPos);
        _bufPos += sizeof(uint64_t);
    }
    return UUID(msb, lsb);
}

float DynaBuffer::getNextFloat() {
    float value = 0.0;
    if (requiresRemaining(sizeof(float))) {
        value = *(float*)(_buffer + _bufPos);
        _bufPos += sizeof(float);
    }
    return value;
}

double DynaBuffer::getNextDouble() {
    double value = 0.0;
    if (requiresRemaining(sizeof(double))) {
        value = *(double*)(_buffer + _bufPos);
        _bufPos += sizeof(double);
    }
    return value;
}

bool DynaBuffer::getNextString(char* buf, uint count, bool addTerm) {
    if (requiresRemaining(count)) {
        for (int i = 0; i < count; ++i) {
            buf[i] = (char)getNextByte();
        }
        if (addTerm)
            buf[count] = (char)0;      // Terminate the buffer
        return true;
    }
    return false;
}

void DynaBuffer::getNextString(char* buf, uint maxCount) {
    int i = 0;
    for (; i < maxCount; ++i) {
        buf[i] = (char)getNextByte();
        if (buf[i] == 0)
            break;
    }
    if (i == maxCount) {
        buf[maxCount] = (char)0;
    }
}

bool DynaBuffer::getNextString(String& str, uint maxCount) {
    return getNextToken(str, '\0', maxCount);
}

bool DynaBuffer::getNextLine(String& str) {
    return getNextToken(str, '\n');
}

bool DynaBuffer::getNextBuffer(byte* buf, uint offset, uint count) {
    if (hasRemaining(count)) {
        memcpy(buf + offset, _buffer + _bufPos, count);
        _bufPos += count;
        return true;
    }
    return false;
}

bool DynaBuffer::getNextToken(String& str, char delim, uint maxCount) {
    str.clear();
    uint len = maxCount > 0 ? maxCount : _bufEnd - _bufPos;
    for (int i = 0; i < len; ++i) {
        char ch = (char)getNextByte();
        if (ch == delim)
            break;
        str.append(1, ch);
    }
    return str.length() > 0;
}

String DynaBuffer::getBufferAsString() {
    return String((const char*)_buffer, _bufSize);
}


//=========================================================================================
//                                        PUTTING
//=========================================================================================
inline void DynaBuffer::_putByte(uint8_t value) {
    _buffer[_bufPos++] = value;
}

bool DynaBuffer::putByte(uint8_t value) {
    if (_bufPos < _bufSize) {
        _buffer[_bufPos++] = value;
        _isDirty = true;
        if (_bufPos > _bufEnd)
            _bufEnd = _bufPos;
        return true;
    }
    else if (THROW_EXCEPTIONS)
        throw BufferOverrunException();
    return false;
}

bool DynaBuffer::putShort(short value) {
    return putUShort((ushort)value);
}

bool DynaBuffer::putUShort(ushort value) {
    if (hasRemainingCapacity(sizeof(ushort))) {
        *(ushort*)(_buffer + _bufPos) = value;
        _bufPos += sizeof(ushort);
        _isDirty = true;
        if (_bufPos > _bufEnd)
            _bufEnd = _bufPos;
        return true;
    }
    else if (THROW_EXCEPTIONS)
        throw BufferOverrunException();
    return false;
}

bool DynaBuffer::putInt(int value) {
    return putUInt((uint)value);
}

bool DynaBuffer::putUInt(uint value) {
    if (hasRemainingCapacity(sizeof(uint))) {
        *(uint*)(_buffer + _bufPos) = value;
        _bufPos += sizeof(uint);
        _isDirty = true;
        if (_bufPos > _bufEnd)
            _bufEnd = _bufPos;
        return true;
    }
    else if (THROW_EXCEPTIONS)
        throw BufferOverrunException();
    return false;
}

bool DynaBuffer::putLong(long value) {
    return putULong((ulong)value);
}

bool DynaBuffer::putULong(ulong value) {
    if (hasRemainingCapacity(sizeof(ulong))) {
        *(ulong*)(_buffer + _bufPos) = value;
        _bufPos += sizeof(ulong);
        _isDirty = true;
        if (_bufPos > _bufEnd)
            _bufEnd = _bufPos;
        return true;
    }
    else if (THROW_EXCEPTIONS)
        throw BufferOverrunException();
    return false;
}

bool DynaBuffer::putLongLong(long long value) {
    return putULongLong((ulonglong)value);
}

bool DynaBuffer::putULongLong(ulonglong value) {
    if (hasRemainingCapacity(sizeof(ulonglong))) {
        *(ulonglong*)(_buffer + _bufPos) = value;
        _bufPos += sizeof(ulonglong);
        _isDirty = true;
        if (_bufPos > _bufEnd)
            _bufEnd = _bufPos;
        return true;
    }
    else if (THROW_EXCEPTIONS)
        throw BufferOverrunException();
    return false;
}

bool DynaBuffer::putIndex(index_t value) {
    if (hasRemainingCapacity(sizeof(ulong))) {
        *(index_t*)(_buffer + _bufPos) = value;
        _bufPos += sizeof(index_t);
        _isDirty = true;
        if (_bufPos > _bufEnd)
            _bufEnd = _bufPos;
        return true;
    }
    else if (THROW_EXCEPTIONS)
        throw BufferOverrunException();
    return false;
}

bool DynaBuffer::putUUID(UUID* value) {
    putLongLong(value->getMostSignificantBits());
    putLongLong(value->getLeastSignificantBits());
    return false;
}

bool DynaBuffer::putFloat(float value) {
    if (hasRemainingCapacity(sizeof(float))) {
        *(float*)(_buffer + _bufPos) = value;
        _bufPos += sizeof(float);
        _isDirty = true;
        if (_bufPos > _bufEnd)
            _bufEnd = _bufPos;
        return true;
    }
    else if (THROW_EXCEPTIONS)
        throw BufferOverrunException();
    return false;
}

bool DynaBuffer::putDouble(double value) {
    if (hasRemainingCapacity(sizeof(double))) {
        *(double*)(_buffer + _bufPos) = value;
        _bufPos += sizeof(double);
        _isDirty = true;
        if (_bufPos > _bufEnd)
            _bufEnd = _bufPos;
        return true;
    }
    else if (THROW_EXCEPTIONS)
        throw BufferOverrunException();
    return false;
}

bool DynaBuffer::putString(const char* value, uint maxCount, bool addTerm) {
    size_t len = strlen(value);
    if (maxCount > 0 && len >= maxCount) {
        if (len > maxCount) {
            len = maxCount;
        }
    }
    if (hasRemainingCapacity((uint)(len + (addTerm ? 1 : 0)))) {
        memcpy(_buffer + _bufPos, value, len);
        _bufPos += len;
        if (addTerm)
            _putByte((uint8_t)0);
        _isDirty = true;
        if (_bufPos > _bufEnd)
            _bufEnd = _bufPos;
        return true;
    }
    else if (THROW_EXCEPTIONS)
        throw BufferOverrunException();
    return false;
}

bool DynaBuffer::putString(const String& strValue, uint maxCount, bool addTerm) {
    return putString(strValue.c_str(), maxCount, addTerm);
}

bool DynaBuffer::putBuffer(uint8_t* buf, uint offset, uint count) {
    if (hasRemainingCapacity(count)) {
        memcpy(_buffer + _bufPos, buf + offset, count);
        _bufPos += count;
        _isDirty = true;
        if (_bufPos > _bufEnd)
            _bufEnd = _bufPos;
        return true;
    }
    else if (THROW_EXCEPTIONS)
        throw BufferOverrunException();
    return false;
}

