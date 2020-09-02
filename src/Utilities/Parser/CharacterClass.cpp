//
// Created by Ken Kopelson on 27/12/17.
//


#include "CharacterClass.h"
#include "../IntWrapper.h"

MAKE_SETTYPE_INSTANCE(Char, Char);

CharacterClass::CharacterClass() {
    _identSet = new DynaHashSet<Char>();
}

CharacterClass::~CharacterClass() {
    delete _identSet;
}

bool CharacterClass::findIdentChar(char char1) {
    auto char2 = Char(char1);
    return _identSet->contains(&char2);
}

bool CharacterClass::findIdentChar(Char* char1) {
    return _identSet->contains(char1);
}

void CharacterClass::addIdentChars(const String& multiChars) {
    ulong len = multiChars.length();
    for (ulong i = 0; i < len; ++i) {
        Char* ch = new Char(multiChars[i]);
        if (!_identSet->contains(ch)) {
            _identSet->add(ch);
        }
        else {
            delete ch;
        }
    }
}

void CharacterClass::clearIdentChars() {
    _identSet->clear();
}

void CharacterClass::setIdentChars(const String& multiChars) {
    clearIdentChars();
    addIdentChars(multiChars);
}

