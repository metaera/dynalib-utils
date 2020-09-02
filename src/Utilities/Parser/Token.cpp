//
// Created by Ken Kopelson on 8/11/17.
//

#include "Token.h"

Token::Token(const Token& token) {
    copyValues(const_cast<Token&>(token));
}

void Token::copyValues(Token& token) {
    _buffer       = token.getBuffer();
    type          = token.type;
    code          = token.code;
    specialBits   = token.specialBits;
    doubleValue   = token.doubleValue;
    boolValue     = token.boolValue;
    lineNumber    = token.lineNumber;
    linePosition  = token.linePosition;
    tokenPosition = token.tokenPosition;
    intRadix      = token.intRadix;
    fracDigits    = token.fracDigits;
    intPortion    = token.intPortion;
    fracPortion   = token.fracPortion;
    expPortion    = token.expPortion;
    negFound      = token.negFound;
    intFound      = token.intFound;
    fracFound     = token.fracFound;
    infNanFound   = token.infNanFound;
    expFound      = token.expFound;
    expSign       = token.expSign;
}

String& Token::getBuffer() {
    return _buffer;
}

void Token::reset() {
    _buffer.clear();
    type         = TYPE_INVALID;
    code         = INVALID_CODE;
    specialBits  = 0L;
}

void Token::appendChar(char ch) {
    _buffer.append(1, ch);
}

ulong Token::length() {
    return _buffer.length();
}

bool Token::isAnySpecial(long bits) { return (specialBits & bits) != 0; }
bool Token::isAllSpecial(long bits) { return (specialBits & bits) == bits; }
long Token::setSpecial(long bits)   { return specialBits |= bits; }
long Token::clearSpecial(long bits) { return specialBits &= bits; }

