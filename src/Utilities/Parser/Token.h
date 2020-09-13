//
// Created by Ken Kopelson on 8/11/17.
//

#ifndef TOKEN_H
#define TOKEN_H

#include "../String.h"

#define TYPE_INVALID            0x0000
#define TOK_TYPE_SPECIAL        0x0001
#define TOK_TYPE_OPER           0x0002
#define TOK_TYPE_KEYWORD        0x0004
#define TOK_TYPE_IDENT          0x0008
#define TOK_TYPE_STRING         0x0010
#define TOK_TYPE_STRING_LIT     0x0020
#define TOK_TYPE_CHAR_LIT       0x0040
#define TOK_TYPE_INT_LIT        0x0100
#define TOK_TYPE_FLOAT_LIT      0x0200

#define TOK_CLASS_ANY           0x0FFF
#define TOK_CLASS_NONTERM       0x0007
#define TOK_CLASS_TEXT_LIT      0x00F0
#define TOK_CLASS_NUMLIT        0x0F00
#define TOK_CLASS_LITERAL       0x0FF0
#define TOK_CLASS_TERM          0x0FF8

#define INVALID_CODE            0
#define SPACE_CODE              1
#define EOL_CODE                2
#define EOS_CODE                3

#define BASE_USER_CODE          1000

class Token {
    String  _buffer;

public:
    int         type           = TYPE_INVALID;
    int         code           = INVALID_CODE;
    long        specialBits    = 0L;
    long double doubleValue    = 0.0;
    bool        boolValue      = false;
    time_t      timeValue      = 0;
    int         lineNumber     = 0;
    int         linePosition   = 0;
    int         tokenPosition  = 0;
    int         intRadix       = 10;
    int         fracDigits     = 0;
    long long   intPortion     = 0;
    long        fracPortion    = 0;
    long        expPortion     = 0;
    bool        negFound       = false;
    bool        intFound       = false;
    bool        fracFound      = false;
    bool        infNanFound    = false;
    bool        expFound       = false;
    bool        expSign        = false;

    Token() = default;
    Token(const Token& token);

    void    copyValues(Token& token);
    void    reset();
    String& getBuffer();
    void    appendChar(char ch);
    ulong   length();

    bool    isAnySpecial(long bits);
    bool    isAllSpecial(long bits);
    long    setSpecial(long bits);
    long    clearSpecial(long bits);
};


#endif //TOKEN_H
