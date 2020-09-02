//
// Created by Ken Kopelson on 3/11/17.
//

#ifndef ROBOCHEFEMULATOR_PRINTUTILS_H
#define ROBOCHEFEMULATOR_PRINTUTILS_H

#include <iostream>
#include <fstream>
using namespace std;

#include "Utilities/String.h"

class PrintUtils {
    static bool _printToFile;
    static ofstream outFile;

    static void _sprintf(char* buffer, size_t maxLen, String format, va_list args);

public:
    static bool printToConsole;

    static void openOutfile(char* fileName);

    static void closeOutfile();

    static bool isPrintToFile();

    static void print(String text);

    static void println();

    static void println(String text);

    static void printIndent(String text, int indent);

    static void printLnIndent(String text, int indent);

    static void sprintf(char* buffer, size_t maxLen, String format, ...);

    static void printf(String format, ...);
};

#endif //ROBOCHEFEMULATOR_PRINTUTILS_H
