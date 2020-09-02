//
// Created by Ken Kopelson on 27/12/17.
//

#ifndef ICHARACTERCLASS_H
#define ICHARACTERCLASS_H

#include "../IntWrapper.h"
#include "../DynaHashSetImpl.h"
#include "../String.h"

class CharacterClass {
    DynaHashSet<Char>* _identSet;

public:
    CharacterClass();
    virtual ~CharacterClass();
    bool findIdentChar(char char1);
    bool findIdentChar(Char* char1);
    void addIdentChars(const String& multiChars);
    void clearIdentChars();
    void setIdentChars(const String& multiChars);

    virtual bool isAlphaClass(char ch) = 0;
    virtual bool isDigitClass(char ch) = 0;
    virtual bool isHexClass(char ch) = 0;
    virtual bool isVowelClass(char ch) = 0;
    virtual bool isConsonantClass(char ch) = 0;
    virtual bool isIdentClass(char ch) = 0;
};


#endif //ICHARACTERCLASS_H
