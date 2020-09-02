//
// Created by Ken Kopelson on 15/10/17.
//

#include <cstdio>
#include "CheckForError.h"

bool CheckForError::isValidArg(bool expResult, const std::string &msg) {
    if (expResult)
        return true;
    cout << "Illegal Argument: " << msg;
    return false;
}

bool CheckForError::isNotNull(void* obj, const std::string &msg) {
    if (obj != nullptr)
        return true;
    cout << "Null Pointer: " << msg;
    return false;
}

bool CheckForError::isInBounds(int index, int maxIndex, const std::string &msg) {
    if (index >= 0 && index <= maxIndex)
        return true;
    cout << "Index out of bounds: " << msg << ", Index: " << index << ", Max: " << maxIndex;
    return false;
}

// Exceptions
bool CheckForError::assertValidArg(bool expResult, const std::string &msg) {
    if (expResult)
        return true;
    throw std::invalid_argument("Invalid Argument: " + msg);
}

bool CheckForError::assertNotNull(void* obj, const std::string &msg) {
    if (obj != nullptr)
        return true;
    throw std::invalid_argument("Null Pointer: " + msg);
}

bool CheckForError::assertInBounds(int index, int maxIndex, const std::string &msg) {
    if (index >= 0 && index <= maxIndex)
        return true;
    throw std::range_error("Index out of bounds: " + msg);
}
