//
// Created by Ken Kopelson on 26/10/17.
//

#ifndef STRINGWRAPPER_H
#define STRINGWRAPPER_H


#include "Utilities/IHashable.h"
#include "Utilities/HashCoder.h"

template <class T> class StringWrapper : IHashable<T> {
    T _value;

public:
    typedef T valueType;
    StringWrapper() : _value() {}
    StringWrapper(T v) : _value(v) {}
    operator T() const { return _value; }

    T getValue() { return _value; }

    int hashCode() const override {
        auto code = HashCoder();
        code.add(_value);
        return code.getCode();
    }
    bool operator== (const T &other) override {
        return _value == other;
    }
    bool operator!= (const T &other) override {
        return !(_value == other);
    }

    // modifiers
    StringWrapper& operator=  (T v)  { _value = v;   return *this; }
    StringWrapper& operator+= (T v)  { _value += v;  return *this; }

    // accessors
    StringWrapper operator[] (size_t pos) {return StringWrapper(_value); }
    StringWrapper operator[] (size_t pos) const {return StringWrapper(_value); }

    // friends
    friend StringWrapper operator+ (StringWrapper iw, StringWrapper v) { return StringWrapper(iw._value + v._value); }
    friend StringWrapper operator+ (StringWrapper iw, T v) { return StringWrapper(iw._value + v); }
    friend StringWrapper operator+ (T v, StringWrapper iw) { return StringWrapper(v + iw._value); }

    friend StringWrapper operator<< (StringWrapper iw, StringWrapper v) { return StringWrapper(iw._value << v._value); }
    friend StringWrapper operator<< (StringWrapper iw, T v) { return StringWrapper(iw._value << v); }
    friend StringWrapper operator<< (T v, StringWrapper iw) { return StringWrapper(v << iw._value); }

    friend StringWrapper operator>> (StringWrapper iw, StringWrapper v) { return StringWrapper(iw._value >> v._value); }
    friend StringWrapper operator>> (StringWrapper iw, T v) { return StringWrapper(iw._value >> v); }
    friend StringWrapper operator>> (T v, StringWrapper iw) { return StringWrapper(v >> iw._value); }

    friend bool operator== (StringWrapper iw, StringWrapper v) { return iw._value == v._value; }
    friend bool operator== (StringWrapper iw, T v) { return iw._value == v; }
    friend bool operator== (T v, StringWrapper iw) { return v == iw._value; }

    friend bool operator!= (StringWrapper iw, StringWrapper v) { return !(iw._value == v._value); }
    friend bool operator!= (StringWrapper iw, T v) { return !(iw._value == v); }
    friend bool operator!= (T v, StringWrapper iw) { return !(v == iw._value); }

    friend bool operator< (StringWrapper iw, StringWrapper v) { return iw._value < v._value; }
    friend bool operator< (StringWrapper iw, T v) { return iw._value < v; }
    friend bool operator< (T v, StringWrapper iw) { return v < iw._value; }

    friend bool operator<= (StringWrapper iw, StringWrapper v) { return iw._value <= v._value; }
    friend bool operator<= (StringWrapper iw, T v) { return iw._value <= v; }
    friend bool operator<= (T v, StringWrapper iw) { return v <= iw._value; }

    friend bool operator> (StringWrapper iw, StringWrapper v) { return iw._value > v._value; }
    friend bool operator> (StringWrapper iw, T v) { return iw._value > v; }
    friend bool operator> (T v, StringWrapper iw) { return v > iw._value; }

    friend bool operator>= (StringWrapper iw, StringWrapper v) { return iw._value >= v._value; }
    friend bool operator>= (StringWrapper iw, T v) { return iw._value >= v; }
    friend bool operator>= (T v, StringWrapper iw) { return v >= iw._value; }
};

#endif //STRINGWRAPPER_H
