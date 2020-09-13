//
// Created by Ken Kopelson on 8/11/17.
//

#include "TokenizerContext.h"
#include "ASCIICharClass.h"
#include "../Utils.h"

TokenizerContext::TokenizerContext() {
    _keyWords   = new DynaHashMap<String, Integer>();
    _singleOps  = new DynaHashMap<Char, Integer>();
    _multiOpSet = new DynaHashSet<Char>();
    _multiOps   = new DynaHashMap<String, Integer>();
    _charClass  = new ASCIICharClass();
    init();
}

TokenizerContext::~TokenizerContext() {
    init();
    delete _keyWords;
    delete _singleOps;
    delete _multiOpSet;
    delete _multiOps;
    delete _charClass;
}

void TokenizerContext::setCaseSensitive(bool caseSensitive) {
    _caseSensitive = caseSensitive;
}

bool TokenizerContext::isCaseSensitive() {
    return _caseSensitive;
}

void TokenizerContext::setGetCharLits(bool getCharLits) {
    _getCharLits = getCharLits;
}

bool TokenizerContext::isGetCharLits() {
    return _getCharLits;
}

void TokenizerContext::setGetStrings(bool getStrings) {
    _getStrings = getStrings;
}

bool TokenizerContext::isGetStrings() {
    return _getStrings;
}

void TokenizerContext::setAllowSingleQuotes(bool allowSingleQuotes) {
    _allowSingleQuotes = allowSingleQuotes;
}

bool TokenizerContext::isAllowSingleQuotes() {
    return _allowSingleQuotes;
}

void TokenizerContext::setGetUnknown(bool getUnknown) {
    _getUnknown = getUnknown;
}

bool TokenizerContext::isGetUnknown() {
    return _getUnknown;
}

void TokenizerContext::setGetCRLF(bool getCRLF) {
    _getCRLF = getCRLF;
}

bool TokenizerContext::isGetCRLF() {
    return _getCRLF;
}

void TokenizerContext::setGetSpaces(bool getSpaces) {
    _getSpaces = getSpaces;
}

bool TokenizerContext::isGetSpaces() {
    return _getSpaces;
}

void TokenizerContext::setAllowNumCommas(bool allowNumCommas) {
    _allowNumCommas = allowNumCommas;
}

bool TokenizerContext::isAllowNumCommas() {
    return _allowNumCommas;
}

void TokenizerContext::setAllowInnerApostrophe(bool innerApostrophe) {
    _innerApostrophe = innerApostrophe;
}


bool TokenizerContext::isAllowInnerApostrophe() {
    return _innerApostrophe;    
}

void TokenizerContext::setCharClass(CharacterClass* charClass) {
    _charClass = charClass;
}

CharacterClass* TokenizerContext::getCharClass() {
    return _charClass;
}

void TokenizerContext::init() {
    _keyWords->clear();
    _singleOps->clear();
    _multiOpSet->clear();
    _multiOps->clear();
    _charClass->clearIdentChars();
}

Integer* TokenizerContext::findKeyword(String* keyword) {
    String temp = keyword->tolower();
    return _keyWords->get(_caseSensitive ? keyword : &temp);
}

bool TokenizerContext::addKeyword(String* keyword, int code) {
    if (!_caseSensitive) {
        keyword->tolower_inplace();
    }
    if (!_keyWords->containsKey(keyword)) {
        _keyWords->put(keyword, new Integer(code));
        return true;
    }
    delete keyword;     // Already in map, so deletee the passed in keyword
    return false;
}

Integer* TokenizerContext::findSingleOp(Char* singleOp) {
    return _singleOps->get(singleOp);
}

bool TokenizerContext::addSingleOp(Char* singleOp, int code) {
    if (!_singleOps->containsKey(singleOp)) {
        _singleOps->put(singleOp, new Integer(code));
        return true;
    }
    delete singleOp;
    return false;
}

Integer* TokenizerContext::findMultiOp(String* multiOp) {
    return _multiOps->get(multiOp);
}

bool TokenizerContext::addMultiOp(String* multiOp, int code) {
    if (!_multiOps->containsKey(multiOp)) {
        ulong len = multiOp->length();
        for (int i = 0; i < len; ++i) {
            auto* ch = new Char((*multiOp)[i]);
            if (!_addMultiOpChar(ch))
                delete ch;
        }
        _multiOps->put(multiOp, new Integer(code));
        return true;
    }
    delete multiOp;
    return false;
}

bool TokenizerContext::findIdentChar(Char* char1) {
    return _charClass->findIdentChar(char1);
}

void TokenizerContext::addIdentChars(const String& multiChars) {
    _charClass->addIdentChars(multiChars);
}

void TokenizerContext::clearIdentChars() {
    _charClass->clearIdentChars();
}

void TokenizerContext::setIdentChars(const String& multiChars) {
    _charClass->setIdentChars(multiChars);
}

bool TokenizerContext::isAlphaClass(char ch) {
    return _charClass->isAlphaClass(ch);
}

bool TokenizerContext::isDigitClass(char ch) {
    return _charClass->isDigitClass(ch);
}

bool TokenizerContext::isHexClass(char ch) {
    return _charClass->isHexClass(ch);
}

bool TokenizerContext::isVowelClass(char ch) {
    return _charClass->isVowelClass(ch);
}

bool TokenizerContext::isConsonantClass(char ch) {
    return _charClass->isConsonantClass(ch);
}

bool TokenizerContext::isIdentClass(char ch) {
    return _charClass->isIdentClass(ch);
}

bool TokenizerContext::findMultiOpChar(Char* char1) {
    return _multiOpSet->contains(char1);
}

bool TokenizerContext::_addMultiOpChar(Char* char1) {
    if (!_multiOpSet->contains(char1)) {
        _multiOpSet->add(char1);
        return true;
    }
    return false;
}
