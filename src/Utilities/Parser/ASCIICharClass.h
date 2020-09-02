//
// Created by Ken Kopelson on 27/12/17.
//

#ifndef ASCIICHARCLASS_H
#define ASCIICHARCLASS_H

#include "CharacterClass.h"

class ASCIICharClass : public CharacterClass {
public:
    bool isAlphaClass(char ch)     override { return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'); }
    bool isDigitClass(char ch)     override { return (ch >= '0' && ch <= '9'); }
    bool isHexClass(char ch)       override { return isDigitClass(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'); }
    bool isVowelClass(char ch)     override { return String("aeiouAEIOU").find(ch) != string::npos; }
    bool isConsonantClass(char ch) override { return isAlphaClass(ch) && !isVowelClass(ch); }
    bool isIdentClass(char ch)     override { return isAlphaClass(ch) || isDigitClass(ch) || findIdentChar(ch); }
};

#endif //ASCIICHARCLASS_H
