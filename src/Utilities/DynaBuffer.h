/*
 * RingBuffer.h
 *
 *  Created on: 31/08/2017
 *      Author: ken
 */

#ifndef DYNABUFFER_H_
#define DYNABUFFER_H_

#include "ICacheConst.h"
#include "TypeDefs.h"
#include "String.h"
#include "UUID.h"

class DynaBuffer : ICopyable<DynaBuffer> {
    uint8_t* _buffer = nullptr;
    uint     _bufSize;
    uint     _bufEnd;
    uint     _bufPos;
    bool     _isDirty;

public:
    static bool THROW_EXCEPTIONS;

    DynaBuffer();
    explicit DynaBuffer(uint size, bool zeroOut=false);
    virtual ~DynaBuffer();

    DynaBuffer(const DynaBuffer& other);
    DynaBuffer* copy() override;

    uint8_t* getInternalTypedArray() { return _buffer; }
    void*    getInternalRawArray()   { return _buffer; }
    uint8_t* getInternalTypedArrayAtPos(ulong pos) { return _buffer + pos; }

    void    reset();
    void    clear();
    void    zeroFill(int fromIndex = 0, int toIndex = -1);
    bool    isDirty();
    void    setIsDirty(bool isDirty);
    uint    getPos() const;
    bool    setPos(uint pos);
    uint    adjustPos(int delta);
    bool    insertRegion(int index, int count);
    bool    deleteRegion(int index, int count);

    void    invalidate();
    bool    isEmpty();
    bool    isFull();
    bool    hasCapacity();
    bool    hasRemainingCapacity(uint count);
    uint    getRemainingCapacity() const;
    bool    hasRemaining(uint count);
    uint    getRemaining() const;
    bool    requiresRemaining(uint count);

    uint    getBufEnd() const;
    void    setBufEnd(uint count);
    uint    getBufferSize() const;
    void    setBufferSize(uint bufSize);

    bool    hasNextByte();
    uint8_t getNextByte();
    uint8_t popLastByte();
    short   getNextShort();
    ushort  getNextUShort();
    int     getNextInt();
    uint    getNextUInt();
    long    getNextLong();
    ulong   getNextULong();
    long long getNextLongLong();
    ulonglong getNextULongLong();
    index_t getNextIndex();
    UUID    getNextUUID();
    float   getNextFloat();
    double  getNextDouble();
    long double getNextLongDouble();
    bool    getNextString(char* buf, uint count, bool addTerm);
    void    getNextString(char* buf, uint maxCount);
    bool    getNextString(String& str, uint maxCount = 0);
    bool    getNextLine(String& str);
    bool    getNextBuffer(byte* buf, uint offset, uint count);
    bool    getNextToken(String& str, char delim, uint maxCount = 0);
    String  getBufferAsString();

    void    _putByte(uint8_t value);
    bool    putByte(uint8_t value);
    bool    putShort(short value);
    bool    putUShort(ushort value);
    bool    putInt(int value);
    bool    putUInt(uint value);
    bool    putLong(long value);
    bool    putULong(ulong value);
    bool    putLongLong(long long value);
    bool    putULongLong(ulonglong value);
    bool    putIndex(index_t value);
    bool    putUUID(UUID* value);
    bool    putFloat(float value);
    bool    putDouble(double value);
    bool    putLongDouble(long double value);
    bool    putString(const char* value, uint maxCount = 0, bool addTerm = true);
    bool    putString(const String& strValue, uint maxCount = 0, bool addTerm = true);
    bool    putBuffer(uint8_t* buf, uint offset, uint count);
};

#endif /* DYNABUFFER_H_ */
