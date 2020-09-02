/*
 * RingBuffer.cpp
 *
 *  Created on: 31/08/2017
 *      Author: ken
 */

#include <cstdio>
#include <string>
#include <stdio.h>
#include <string.h>

#include "DynaRingBuffer.h"
#include "Exception.h"

bool DynaRingBuffer::THROW_EXCEPTIONS = true;

DynaRingBuffer::DynaRingBuffer(ulong size) {
    _buffer = new uint8_t[size];
    _bufSize  = size;
    reset();
}

DynaRingBuffer::~DynaRingBuffer() {
    delete[] _buffer;
}

void DynaRingBuffer::reset() {
    _bufHead = _bufTail = _count = 0;
    memset(_buffer, 0, _bufSize);
}

bool DynaRingBuffer::isEmpty() {
    return _count == 0;
}

bool DynaRingBuffer::isFull() {
    return _count == _bufSize;
}

bool DynaRingBuffer::hasNextByte() {
    return !isEmpty();
}

bool DynaRingBuffer::hasByteCount(ulong count) {
    return getCount() >= count;
}

ulong DynaRingBuffer::getCount() {
    return _count;
}

bool DynaRingBuffer::hasFreeCount(ulong count) {
    return getFreeCount() >= count;
}

ulong DynaRingBuffer::getFreeCount() {
    return _bufSize - getCount();
}

ulong DynaRingBuffer::getBufferSize() {
    return _bufSize;
}

uint8_t DynaRingBuffer::getNextByte() {
    if (!isEmpty()) {
        if (_bufTail == _bufSize)
            _bufTail = 0;
        uint8_t value = _buffer[_bufTail++];
        --_count;
        return value;
    }
    else if (THROW_EXCEPTIONS)
        throw BufferUnderrunException();
    return 0;
}

uint8_t DynaRingBuffer::popLastByte() {
    if (!isEmpty()) {
        if (_bufHead == 0)
            _bufHead = _bufSize;
        uint8_t value = _buffer[--_bufHead];
        --_count;
        return value;
    }
    else if (THROW_EXCEPTIONS)
        throw BufferUnderrunException();
    return 0;
}

ushort DynaRingBuffer::getNextUShort() {
    ushort value = 0;
    value |= (ushort)getNextByte() << 8;
    value |= (ushort)getNextByte();
    return value;
}

ulong DynaRingBuffer::getNextULong() {
    ulong value = 0;
    value |= (ulong)getNextByte() << 24;
    value |= (ulong)getNextByte() << 16;
    value |= (ulong)getNextByte() << 8;
    value |= (ulong)getNextByte();
    return value;
}

ulonglong DynaRingBuffer::getNextULongLong() {
    ulonglong value = 0;
    value |= (ulonglong)getNextByte() << 56;
    value |= (ulonglong)getNextByte() << 48;
    value |= (ulonglong)getNextByte() << 40;
    value |= (ulonglong)getNextByte() << 32;
    value |= (ulonglong)getNextByte() << 24;
    value |= (ulonglong)getNextByte() << 16;
    value |= (ulonglong)getNextByte() << 8;
    value |= (ulonglong)getNextByte();
    return value;
}

float DynaRingBuffer::getNextFloat() {
    ulong value = getNextULong();
    return static_cast<float>(value);
}

double DynaRingBuffer::getNextDouble() {
    ulonglong value = getNextULongLong();
    return static_cast<double>(value);
}

bool DynaRingBuffer::getNextString(char* buf, ulong count, bool addTerm) {
    if (hasByteCount(count)) {
        for (int i = 0; i < count; ++i) {
            buf[i] = (char)getNextByte();
        }
        if (addTerm)
            buf[count] = (char)0;      // Terminate the buffer
        return true;
    }
    return false;
}

void DynaRingBuffer::getNextString(char* buf, ulong maxCount) {
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

bool DynaRingBuffer::getNextString(String& str) {
    str.clear();
    ulong len = _count;
    for (int i = 0; i < len; ++i) {
        str.append(1, (char)getNextByte());
    }
    return true;
}

bool DynaRingBuffer::getNextToken(String& str, char delim) {
    str.clear();
    ulong len = _count;
    for (int i = 0; i < len; ++i) {
        char ch = (char)getNextByte();
        if (ch == delim)
            break;
        str.append(1, ch);
    }
    return str.length() > 0;
}

String DynaRingBuffer::getBufferAsString() {
    String str;
    ulong len = _count;
    for (int i = 0; i < len; ++i) {
        str.append(1, (char)getNextByte());
    }
    return String(str);
}

bool DynaRingBuffer::putByte(uint8_t value) {
    if (!isFull()) {
        if (_bufHead == _bufSize)
            _bufHead = 0;
        _buffer[_bufHead++] = value;
        ++_count;
        return true;
    }
    else if (THROW_EXCEPTIONS)
        throw BufferOverrunException();
    return false;
}

bool DynaRingBuffer::putUShort(ushort value) {
    if (putByte((uint8_t)((value >> 8) & 0xFF)))
        if (putByte((uint8_t)(value & 0xFF)))
            return true;
    return false;
}

bool DynaRingBuffer::putULong(ulong value) {
    if (putByte((uint8_t)((value >> 24) & 0xFF)))
        if (putByte((uint8_t)((value >> 16) & 0xFF)))
            if (putByte((uint8_t)((value >> 8) & 0xFF)))
                if (putByte((uint8_t)(value & 0xFF)))
                    return true;
    return false;
}

bool DynaRingBuffer::putULongLong(ulonglong value) {
    if (putByte((uint8_t)((value >> 56) & 0xFF)))
        if (putByte((uint8_t)((value >> 48) & 0xFF)))
            if (putByte((uint8_t)((value >> 40) & 0xFF)))
                if (putByte((uint8_t)((value >> 32) & 0xFF)))
                    if (putByte((uint8_t)((value >> 24) & 0xFF)))
                        if (putByte((uint8_t)((value >> 16) & 0xFF)))
                            if (putByte((uint8_t)((value >> 8) & 0xFF)))
                                if (putByte((uint8_t)(value & 0xFF)))
                                    return true;
    return false;
}

bool DynaRingBuffer::putFloat(float value) {
    return putULong(static_cast<ulong>(value));
}

bool DynaRingBuffer::putDouble(double value) {
    return putULong(static_cast<ulonglong>(value));
}

bool DynaRingBuffer::putString(const char* value, bool addTerm) {
    ulong len = strlen(value);
    if (hasFreeCount(len + (addTerm ? 1 : 0))) {
        for (int i = 0; i < len; ++i) {
            putByte((uint8_t)value[i]);
        }
        if (addTerm)
            putByte((uint8_t)0);
        return true;
    }
    else if (THROW_EXCEPTIONS)
        throw BufferOverrunException();
    return false;
}
