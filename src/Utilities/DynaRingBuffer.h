/*
 * RingBuffer.h
 *
 *  Created on: 31/08/2017
 *      Author: ken
 */

#ifndef DYNARINGBUFFER_H_
#define DYNARINGBUFFER_H_

#include "TypeDefs.h"
#include "String.h"

class DynaRingBuffer {
    uint8_t* _buffer = nullptr;
    ulong    _bufSize;
    ulong    _count;
    ulong    _bufHead;
    ulong    _bufTail;

public:
    static bool THROW_EXCEPTIONS;

    explicit DynaRingBuffer(ulong size);
    virtual ~DynaRingBuffer();

    void    reset();
    bool    isEmpty();
    bool    isFull();
    bool    hasNextByte();
    bool    hasByteCount(ulong count);
    ulong   getCount();
    bool    hasFreeCount(ulong count);
    ulong   getFreeCount();
    ulong   getBufferSize();
    uint8_t getNextByte();
    uint8_t popLastByte();
    ushort  getNextUShort();
    ulong   getNextULong();
    ulonglong getNextULongLong();
    float   getNextFloat();
    double  getNextDouble();
    String  getBufferAsString();

    bool    getNextString(char* buf, ulong count, bool addTerm);
    void    getNextString(char* buf, ulong maxCount);
    bool    getNextString(String& str);
    bool    getNextToken(String& str, char delim);
    bool    putByte(uint8_t value);
    bool    putUShort(ushort value);
    bool    putULong(ulong value);
    bool    putULongLong(ulonglong value);
    bool    putFloat(float value);
    bool    putDouble(double value);
    bool    putString(const char* value, bool addTerm = true);
};

#endif /* DYNARINGBUFFER_H_ */
