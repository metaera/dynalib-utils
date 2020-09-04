//
// Created by Ken Kopelson on 17/10/17.
//

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>
#include <exception>
#include "String.h"
using namespace std;

class Exception : public exception {
    String _msg;

public:
    Exception() = default;
    explicit Exception(const String& msg) : _msg(msg) {}

    String getMessage() { return _msg; }
};

class MethodNotImplemented : public Exception {
public:
    MethodNotImplemented() : Exception() {};
    explicit MethodNotImplemented(const String& msg) : Exception(msg) {}
};

class StrictViolationException : public Exception {
public:
    StrictViolationException() : Exception() {};
    explicit StrictViolationException(const String& msg) : Exception(msg) {}
};

class FormatConversionException : public Exception {
public:
    FormatConversionException() : Exception() {};
    explicit FormatConversionException(const String& msg) : Exception(msg) {}
};

class BufferOverrunException : public Exception {
public:
    BufferOverrunException() : Exception() {};
    explicit BufferOverrunException(const String& msg) : Exception(msg) {}
};

class BufferUnderrunException : public Exception {
public:
    BufferUnderrunException() : Exception() {};
    explicit BufferUnderrunException(const String& msg) : Exception(msg) {}
};

class SecurityAccessViolation : public Exception {
public:
    SecurityAccessViolation() : Exception() {};
    explicit SecurityAccessViolation(const String& msg) : Exception(msg) {}
};

class EmptyStackException : public Exception {
public:
    EmptyStackException() : Exception() {};
    explicit EmptyStackException(const String& msg) : Exception(msg) {}
};

class MixingPointerAndReferenceKeysException : public Exception {
public:
    MixingPointerAndReferenceKeysException() : Exception() {};
    explicit MixingPointerAndReferenceKeysException(const String& msg) : Exception(msg) {}
};

class ParserException : public Exception {
public:
    ParserException() : Exception() {};
    explicit ParserException(const String& msg) : Exception(msg) {}
};

class FileNotFoundException : public Exception {
public:
    FileNotFoundException() : Exception() {};
    explicit FileNotFoundException(const String& msg) : Exception(msg) {}
};

class DataFileException : public Exception {
public:
    DataFileException() : Exception() {};
    explicit DataFileException(const String& msg) : Exception(msg) {}
};

class IllegalArgumentException : public Exception {
public:
    IllegalArgumentException() : Exception() {};
    explicit IllegalArgumentException(const String& msg) : Exception(msg) {}
};

class InvalidValueException : public Exception {
public:
    InvalidValueException() : Exception() {};
    explicit InvalidValueException(const String& msg) : Exception(msg) {}
};

#endif //EXCEPTION_H
