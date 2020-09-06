//
// Created by Ken Kopelson on 3/11/17.
//

#include <cstdio>
#include <cstdarg>
#include "PrintUtils.h"

ofstream PrintUtils::outFile;
bool PrintUtils::_printToFile = false;
bool PrintUtils::printToConsole = true;

void PrintUtils::openOutfile(char* fileName) {
    closeOutfile();
    outFile.open(fileName);
    _printToFile = true;
}

void PrintUtils::closeOutfile() {
    if (_printToFile) {
        outFile.close();
        _printToFile = false;
    }
}

bool PrintUtils::isPrintToFile() {
    return _printToFile;
}



void PrintUtils::print(const String text) {
    if (printToConsole)
        cout << text;
    if (_printToFile)
        outFile << text;
}

void PrintUtils::println() {
    if (printToConsole)
        cout << endl;
    if (_printToFile)
        outFile << endl;
}

void PrintUtils::println(const String text) {
    if (printToConsole)
        cout << text << endl;
    if (_printToFile)
        outFile << text << endl;
}

void PrintUtils::printIndent(const String text, int indent) {
    for (int i = 0; i < indent; ++i)
        print((String)"  ");
    print(text);
}

void PrintUtils::printLnIndent(const String text, int indent) {
    for (int i = 0; i < indent; ++i)
        print((String)"  ");
    println(text);
}

void PrintUtils::_sprintf(char* buffer, size_t maxLen, const String format, va_list args) {
    const char* fmt = format.c_str();
    vsnprintf(buffer, maxLen, fmt, args);
}


void PrintUtils::sprintf(char* buffer, size_t maxLen, const String format, ...) {
    va_list args;
    va_start(args, format);
    _sprintf(buffer, maxLen, format, args);
    va_end(args);
}

void PrintUtils::printf(const String format, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, format);
    _sprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    cout << buffer;
}
