//
// Created by Ken Kopelson on 26/10/17.
//

#ifndef INTWRAPPER_H
#define INTWRAPPER_H

#include "IndexConfig.h"
#include "IHashable.h"
#include "ICopyable.h"
#include "HashCoder.h"

template <class T> class IntWrapper : IHashable<T>, ICopyable<IntWrapper<T>> {
    T _value;

public:
    typedef T valueType;
    IntWrapper() : _value() {}
    IntWrapper(T v) : _value(v) {}
    operator T() const { return _value; }
    operator T() { return _value; }
    IntWrapper<T>* copy() override {
        return new IntWrapper<T>(_value);
    }

    T getValue() { return _value; }

    int hashCode() const override {
        auto code = HashCoder();
        code.add(_value);
        return code.getCode();
    }
    bool operator== (const T &other) const override {
        return _value == other;
    }

    // modifiers
    IntWrapper& operator++ ()     { ++_value; return *this; }
    IntWrapper& operator-- ()     { --_value; return *this; }
    IntWrapper  operator++ (int)  { return IntWrapper(_value++); }
    IntWrapper  operator-- (int)  { return IntWrapper(_value--); }

    // Assignment
    IntWrapper& operator=  (T v)  { _value = v;   return *this; }
    IntWrapper& operator+= (T v)  { _value += v;  return *this; }
    IntWrapper& operator-= (T v)  { _value -= v;  return *this; }
    IntWrapper& operator*= (T v)  { _value *= v;  return *this; }
    IntWrapper& operator/= (T v)  { _value /= v;  return *this; }
    IntWrapper& operator%= (T v)  { _value %= v;  return *this; }
    IntWrapper& operator&= (T v)  { _value &= v;  return *this; }
    IntWrapper& operator|= (T v)  { _value |= v;  return *this; }
    IntWrapper& operator^= (T v)  { _value ^= v;  return *this; }
    IntWrapper& operator<<= (T v) { _value <<= v; return *this; }
    IntWrapper& operator>>= (T v) { _value >>= v; return *this; }

    // accessors
    IntWrapper operator+ () const {return IntWrapper(+_value); }
    IntWrapper operator- () const {return IntWrapper(-_value); }
    IntWrapper operator! () const {return IntWrapper(!_value); }
    IntWrapper operator~ () const {return IntWrapper(~_value); }

    // friends
    friend IntWrapper operator+ (IntWrapper iw, IntWrapper v) { return IntWrapper(iw._value + v._value); }
    friend IntWrapper operator+ (IntWrapper iw, T v) { return IntWrapper(iw._value + v); }
    friend IntWrapper operator+ (T v, IntWrapper iw) { return IntWrapper(v + iw._value); }

    friend IntWrapper operator- (IntWrapper iw, IntWrapper v) { return IntWrapper(iw._value - v._value); }
    friend IntWrapper operator- (IntWrapper iw, T v) { return IntWrapper(iw._value - v); }
    friend IntWrapper operator- (T v, IntWrapper iw) { return IntWrapper(v - iw._value); }

    friend IntWrapper operator* (IntWrapper iw, IntWrapper v) { return IntWrapper(iw._value * v._value); }
    friend IntWrapper operator* (IntWrapper iw, T v) { return IntWrapper(iw._value * v); }
    friend IntWrapper operator* (T v, IntWrapper iw) { return IntWrapper(v * iw._value); }

    friend IntWrapper operator/ (IntWrapper iw, IntWrapper v) { return IntWrapper(iw._value / v._value); }
    friend IntWrapper operator/ (IntWrapper iw, T v) { return IntWrapper(iw._value / v); }
    friend IntWrapper operator/ (T v, IntWrapper iw) { return IntWrapper(v / iw._value); }

    friend IntWrapper operator% (IntWrapper iw, IntWrapper v) { return IntWrapper(iw._value % v._value); }
    friend IntWrapper operator% (IntWrapper iw, T v) { return IntWrapper(iw._value % v); }
    friend IntWrapper operator% (T v, IntWrapper iw) { return IntWrapper(v % iw._value); }

    friend IntWrapper operator& (IntWrapper iw, IntWrapper v) { return IntWrapper(iw._value & v._value); }
    friend IntWrapper operator& (IntWrapper iw, T v) { return IntWrapper(iw._value & v); }
    friend IntWrapper operator& (T v, IntWrapper iw) { return IntWrapper(v & iw._value); }

    friend IntWrapper operator| (IntWrapper iw, IntWrapper v) { return IntWrapper(iw._value | v._value); }
    friend IntWrapper operator| (IntWrapper iw, T v) { return IntWrapper(iw._value | v); }
    friend IntWrapper operator| (T v, IntWrapper iw) { return IntWrapper(v | iw._value); }

    friend IntWrapper operator^ (IntWrapper iw, IntWrapper v) { return IntWrapper(iw._value ^ v._value); }
    friend IntWrapper operator^ (IntWrapper iw, T v) { return IntWrapper(iw._value ^ v); }
    friend IntWrapper operator^ (T v, IntWrapper iw) { return IntWrapper(v ^ iw._value); }

    friend IntWrapper operator<< (IntWrapper iw, IntWrapper v) { return IntWrapper(iw._value << v._value); }
    friend IntWrapper operator<< (IntWrapper iw, T v) { return IntWrapper(iw._value << v); }
    friend IntWrapper operator<< (T v, IntWrapper iw) { return IntWrapper(v << iw._value); }

    friend IntWrapper operator>> (IntWrapper iw, IntWrapper v) { return IntWrapper(iw._value >> v._value); }
    friend IntWrapper operator>> (IntWrapper iw, T v) { return IntWrapper(iw._value >> v); }
    friend IntWrapper operator>> (T v, IntWrapper iw) { return IntWrapper(v >> iw._value); }

    friend bool operator== (IntWrapper iw, IntWrapper v) { return iw._value == v._value; }
    friend bool operator== (IntWrapper iw, T v) { return iw._value == v; }
    friend bool operator== (T v, IntWrapper iw) { return v == iw._value; }

    friend bool operator!= (IntWrapper iw, IntWrapper v) { return iw._value != v._value; }
    friend bool operator!= (IntWrapper iw, T v) { return iw._value != v; }
    friend bool operator!= (T v, IntWrapper iw) { return v != iw._value; }

    friend bool operator< (IntWrapper iw, IntWrapper v) { return iw._value < v._value; }
    friend bool operator< (IntWrapper iw, T v) { return iw._value < v; }
    friend bool operator< (T v, IntWrapper iw) { return v < iw._value; }

    friend bool operator<= (IntWrapper iw, IntWrapper v) { return iw._value <= v._value; }
    friend bool operator<= (IntWrapper iw, T v) { return iw._value <= v; }
    friend bool operator<= (T v, IntWrapper iw) { return v <= iw._value; }

    friend bool operator> (IntWrapper iw, IntWrapper v) { return iw._value > v._value; }
    friend bool operator> (IntWrapper iw, T v) { return iw._value > v; }
    friend bool operator> (T v, IntWrapper iw) { return v > iw._value; }

    friend bool operator>= (IntWrapper iw, IntWrapper v) { return iw._value >= v._value; }
    friend bool operator>= (IntWrapper iw, T v) { return iw._value >= v; }
    friend bool operator>= (T v, IntWrapper iw) { return v >= iw._value; }

};

typedef IntWrapper<char>               Char;
typedef IntWrapper<unsigned char>      UChar;
typedef IntWrapper<short>              Short;
typedef IntWrapper<unsigned short>     UShort;
typedef IntWrapper<int>                Integer;
typedef IntWrapper<unsigned int>       UInteger;
typedef IntWrapper<long>               Long;
typedef IntWrapper<unsigned long>      ULong;
typedef IntWrapper<long long>          LongLong;
typedef IntWrapper<unsigned long long> ULongLong;
typedef IntWrapper<float>              Float;
typedef IntWrapper<double>             Double;

#ifdef USE_LONG
typedef IntWrapper<int64_t> Index;
#else
typedef IntWrapper<int32_t> Index;
#endif

#endif //INTWRAPPER_H
