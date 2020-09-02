//
// Created by Ken Kopelson on 15/10/17.
//

#ifndef CHECKFORERROR_H
#define CHECKFORERROR_H

#include <string>
#include <iostream>
#include <stdexcept>

using namespace std;

/**
 * Define "NDEBUG" to disable all assertions
 */

class CheckForError {
public:
    static bool isValidArg(bool expResult, const std::string &msg = "???");
    static bool isNotNull(void* obj, const std::string &msg = "???");
    static bool isInBounds(int index, int maxIndex, const std::string &msg = "???");

    // Exceptions
    static bool assertValidArg(bool expResult, const std::string &msg = "???");
    static bool assertNotNull(void* obj, const std::string &msg = "???");
    static bool assertInBounds(int index, int maxIndex, const std::string &msg = "???");
};


#endif //CHECKFORERROR_H
