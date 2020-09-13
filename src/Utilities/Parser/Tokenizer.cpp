//
// Created by Ken Kopelson on 8/11/17.
//

#include <cmath>

#include "../Utils.h"
#include "Tokenizer.h"
#include "Token.h"
#include "TokenizerContext.h"
#include "TokenizerState.h"
#include "ErrorContext.h"

Tokenizer::Tokenizer(TokenizerContext* context, const String& text) {
    _context        = context;
    _contextCreated = false;
    init();
    if (!text.empty())
        setText(text);
}

Tokenizer::Tokenizer(TokenizerContext* context) {
    _context = context;
    init();
}

Tokenizer::Tokenizer(const String& text) {
    init();
    if (!text.empty())
        setText(text);
}

Tokenizer::Tokenizer() {
    init();
}

Tokenizer::~Tokenizer() {
    delete _errorContext;
    if (_contextCreated) {
        delete _context;
    }
}

void Tokenizer::init() {
    if (_errorContext != nullptr) {
        delete _errorContext;
    }
    _errorContext = new ErrorContext;
    if (_context == nullptr) {
        _context = new TokenizerContext();
        _contextCreated = true;
    }
    _context->init();
}

void Tokenizer::setContext(TokenizerContext* context) {
    if (context != nullptr) {
        if (_contextCreated) {
            delete _context;
            _contextCreated = false;
        }
        _context = context;
    }
}

TokenizerContext* Tokenizer::getContext() {
    return _context;
}

void Tokenizer::setText(const String& text) {
    const_cast<String&>(_text) = text;
    resetScan();
}

void Tokenizer::setText(String& text) {
    const_cast<String&>(_text) = text;
    resetScan();
}

void Tokenizer::setCaseSensitive(bool caseSensitive) {
    _context->setCaseSensitive(caseSensitive);
}

bool Tokenizer::isCaseSensitive() {
    return _context->isCaseSensitive();
}

void Tokenizer::setGetCharLits(bool getCharLits) {
    _context->setGetCharLits(getCharLits);
}

bool Tokenizer::isGetCharLits() {
    return _context->isGetCharLits();
}

void Tokenizer::setGetStrings(bool getStrings) {
    _context->setGetStrings(getStrings);
}

bool Tokenizer::isGetStrings() {
    return _context->isGetStrings();
}

void Tokenizer::setAllowSingleQuotes(bool allowSingleQuotes) {
    _context->setAllowSingleQuotes(allowSingleQuotes);
}

bool Tokenizer::isAllowSingleQuotes() {
    return _context->isAllowSingleQuotes();
}

void Tokenizer::setGetUnknown(bool getUnknown) {
    _context->setGetUnknown(getUnknown);
}

bool Tokenizer::isGetUnknown() {
    return _context->isGetUnknown();
}

void Tokenizer::setGetCRLF(bool getCRLF) {
    _context->setGetCRLF(getCRLF);
}

bool Tokenizer::isGetCRLF() {
    return _context->isGetCRLF();
}

void Tokenizer::setGetSpaces(bool getSpaces) {
    _context->setGetSpaces(getSpaces);
}

bool Tokenizer::isGetSpaces() {
    return _context->isGetSpaces();
}

void Tokenizer::setAllowNumCommas(bool allowNumCommas) {
    _context->setAllowNumCommas(allowNumCommas);
}

bool Tokenizer::isAllowNumCommas() {
    return _context->isAllowNumCommas();
}

void Tokenizer::resetScan() {
    _currPosition   = -1;
    _linePosition   = 0;
    _lineStart      = 0;
    _indentPosition = 1;
    _currChar       = _peekChar = '\0';
    delete _errorContext;
    _errorContext   = new ErrorContext;
    _errorFlag      = false;
    _errorStop      = false;
    _errorCount     = 0;
    _lineNumber     = 1;
    _getNextChar();
}

Integer* Tokenizer::findKeyword(String* keyword) {
    return _context->findKeyword(keyword);
}

Integer* Tokenizer::findKeyword(String keyword) {
    return _context->findKeyword(&keyword);
}

bool Tokenizer::addKeyword(String* keyword, int code) {
    return _context->addKeyword(keyword, code);
}

Integer* Tokenizer::findSingleOp(Char* singleOp) {
    return _context->findSingleOp(singleOp);
}

Integer* Tokenizer::findSingleOp(Char singleOp) {
    return _context->findSingleOp(&singleOp);
}

bool Tokenizer::addSingleOp(Char* singleOp, int code) {
    return _context->addSingleOp(singleOp, code);
}

Integer* Tokenizer::findMultiOp(String* multiOp) {
    return _context->findMultiOp(multiOp);
}

Integer* Tokenizer::findMultiOp(String multiOp) {
    return _context->findMultiOp(&multiOp);
}

bool Tokenizer::addMultiOp(String* multiOp, int code) {
    return _context->addMultiOp(multiOp, code);
}

bool Tokenizer::findIdentChar(Char* char1) {
    return _context->findIdentChar(char1);
}

bool Tokenizer::findIdentChar(Char char1) {
    return _context->findIdentChar(&char1);
}

void Tokenizer::addIdentChars(const String& multiChars) {
    return _context->addIdentChars(multiChars);
}

void Tokenizer::clearIdentChars() {
    _context->clearIdentChars();
}

void Tokenizer::setIdentChars(const String& multiChars) {
    _context->setIdentChars(multiChars);
}

bool Tokenizer::hasNextChar(int index) {
    return ((_currPosition + index) < _text.length());
}

char Tokenizer::peekNextChar(int index) {
    _peekChar = ( hasNextChar(index) ? _text[_currPosition + index] : (char)'\0' );
    return _peekChar;
}

char Tokenizer::peekCurrChar() {
    return _currChar;
}

bool Tokenizer::isEOS() {
    return _currToken->code == EOS_CODE;
}

bool Tokenizer::fetchToken(Token& token) {
    _currToken = &token;
    while (true) {
        if (!_getNextToken())
            return false;
        if (_currToken->code != EOS_CODE)
            break;
    }
    return true;
}

int Tokenizer::getCurrPosition() {
    return _currPosition;
}

void Tokenizer::setIndentHere() {
    _indentPosition = _currToken->linePosition;
}

void Tokenizer::setIndentPosition(int indentPos) {
    _indentPosition = indentPos;
}

int Tokenizer::getIndentPosition() {
    return _indentPosition;
}

bool Tokenizer::hasIndented(int indentPos) {
    return _currToken->linePosition > indentPos;
}

bool Tokenizer::hasOutdented(int indentPos) {
    return _currToken->linePosition < indentPos;
}

void Tokenizer::resetIndent() {
    _indentPosition = 1;
}

Token* Tokenizer::getCurrToken() {
    return _currToken;
}

String Tokenizer::getTokenLineSegment() {
    return String(_text.substr(_lineStart, _currPosition));
}

bool Tokenizer::isError() {
    return _errorFlag;
}

ErrorContext* Tokenizer::getErrorContext() {
    return _errorContext;
}

void Tokenizer::setErrorContext(ErrorContext* errorContext) {
    _errorContext = errorContext;
}

String Tokenizer::getErrorLineSegment() {
    if (_errorContext != nullptr)
        return _text.substr(_errorContext->errorLineStart, _errorContext->errorEnd);
    else
        return "";
}

int Tokenizer::getErrorCount() {
    return _errorCount;
}

TokenizerState* Tokenizer::saveState() {
    auto* state           = new TokenizerState();
    state->currPosition   = _currPosition;
    state->linePosition   = _linePosition;
    state->lineStart      = _lineStart;
    state->indentPosition = _indentPosition;
    state->currChar       = _currChar;
    state->peekChar       = _peekChar;
    state->errorFlag      = _errorFlag;
    state->errorCount     = _errorCount;
    state->errorStop      = _errorStop;
    state->lineNumber     = _lineNumber;
    state->currToken      = new Token(*_currToken);
    return state;
}

Token* Tokenizer::restoreState(TokenizerState* state) {
    _currPosition   = state->currPosition;
    _linePosition   = state->linePosition;
    _lineStart      = state->lineStart;
    _indentPosition = state->indentPosition;
    _currChar       = state->currChar;
    _peekChar       = state->peekChar;
    _errorFlag      = state->errorFlag;
    _errorCount     = state->errorCount;
    _errorStop      = state->errorStop;
    _lineNumber     = state->lineNumber;
    _currToken->copyValues(*state->currToken);
    deleteState(state);
    return _currToken;
}

void Tokenizer::deleteState(TokenizerState* state) {
    delete state->currToken;
    delete state; 
}

void Tokenizer::restartFromToken(Token& token) {
    _currToken->copyValues(token);
    _restartToken();
}

//===========================================================================================
//     PRIVATE IMPLEMENTATION METHODS
//===========================================================================================
bool Tokenizer::_isBinaryClass(char ch) { return ch == '0' || ch == '1'; }
bool Tokenizer::_isDigitClass(char ch)  { return _context->isDigitClass(ch); }
bool Tokenizer::_isAlphaClass(char ch)  { return _context->isAlphaClass(ch); }
bool Tokenizer::_isHexClass(char ch)    { return _context->isHexClass(ch); }
bool Tokenizer::_isIdentClass(char ch)  { return _context->isIdentClass(ch); }

bool Tokenizer::_findMultiOpChar(Char* char1) {
    return _context->findMultiOpChar(char1);
}

bool Tokenizer::_findMultiOpChar(Char char1) {
    return _context->findMultiOpChar(&char1);
}

void Tokenizer::_advanceLine() {
    ++_lineNumber;
    _linePosition = 0;
    _lineStart    = _currPosition;
}

char Tokenizer::_getNextChar() {
    if (hasNextChar()) {
        ++_currPosition;
        ++_linePosition;
        _currChar = _text[_currPosition];
    }
    else
        _currChar = '\0';
    return _currChar;
}

void Tokenizer::_appendCurrChar() {
    if (_currChar != '\0')
        _currToken->appendChar(_currChar);
}

void Tokenizer::_appendNextChar() {
    _getNextChar();
    if (_currChar != '\0')
        _appendCurrChar();
}

void Tokenizer::_appendNextChars(int count) {
    for (int i = 0; i < count; ++i) {
        _appendNextChar();
    }
}

void Tokenizer::_restartToken() {
    _currPosition = _currToken->tokenPosition;
    _linePosition = _currToken->linePosition;
    _currChar     = _text[_currPosition];
    _currToken->reset();
}

bool Tokenizer::_getNextToken() {
    _currToken->reset();
    _errorFlag = false;
    while (true) {
        if (!isGetCRLF()) {
            _skipCRLF();
        }
        else if (!isGetSpaces()) {
            _skipSpaces();
        }
        if (!_skipComment()) {
            break;
        }
    }

    _currToken->lineNumber    = _lineNumber;
    _currToken->linePosition  = _linePosition;
    _currToken->tokenPosition = _currPosition;

    if (_currChar == '\0' || _errorStop) {
        _currToken->type = TOK_TYPE_SPECIAL;
        _currToken->code = EOS_CODE;        // End of String
        return false;
    }
    else if (_currChar == '\n' || _currChar == '\r') {
        _currToken->type = TOK_TYPE_SPECIAL;
        _currToken->code = EOL_CODE;        // End of Line
        if (_currChar == '\n') {
            _advanceLine();
        }
        _appendCurrChar();
        _getNextChar();
    }
    else if (_currChar == ' ' || _currChar == '\t') {
        _currToken->type = TOK_TYPE_SPECIAL;
        _currToken->code = SPACE_CODE;
        _currChar        = ' ';     // Replace possible 'tab' char with a space
        _appendCurrChar();
        _getNextChar();
    }
    else {
        if (!_checkForMultiOp()) {
            if (!_checkForNumber()) {
                if (!_checkForIdent()) {
                    if (!_checkForOperator()) {
                        if (!_checkForCharLiteral()) {
                            if (!_checkForQuote()) {
                                if (_context->isGetUnknown()) {
                                    _appendCurrChar();
                                    _getNextChar();
                                    _currToken->type = TOK_TYPE_CHAR_LIT;
                                } else {
                                    error("Invalid Syntax");
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}

char Tokenizer::_skipSpaces() {
    while (_currChar == ' ' || _currChar == '\t')
        _getNextChar();
    return _currChar;
}

void Tokenizer::_skipCRLF() {
    char ch;
    while ( ( ( ch = _skipSpaces() ) == '\n' ) || ( ch == '\r' ) ) {
        if ( ch == '\n' )
            _advanceLine();
        _getNextChar();
    }
}

bool Tokenizer::_skipComment() {
    int commentLevel = 0;
    bool result = false;
    while ( true ) {
        if ( _currChar == '/' ) {
            peekNextChar();
            if ( _peekChar == '*' ) {
                ++commentLevel;
                _getNextChar();
                result = true;
                while ( commentLevel > 0 ) {
                    _getNextChar();
                    switch ( _currChar ) {
                        case '/':
                            if ( peekNextChar() == '*' ) {
                                _getNextChar();
                                ++commentLevel;
                            }
                            continue;
                        case '*':
                            if ( peekNextChar() == '/' ) {
                                _getNextChar();
                                --commentLevel;
                            }
                            continue;
                        case '\0':
                            error( "EOF encountered in comment block" );
                            return false;
                        default:
                            if ( _currChar == '\n' )
                                _advanceLine();
                            break;
                    }
                }
                _getNextChar();
            }
            else if ( _peekChar == '/' ) {
                _getNextChar();
                result = true;
                while ( true ) {
                    _getNextChar();
                    switch ( _currChar ) {
                        case '\n':
                            _advanceLine();
                            break;
                        case '\0':
                            return false;
                        default:
                            continue;
                    }
                    break;
                }
                if (!isGetCRLF()) {
                    _getNextChar();
                }
            }
            else
                break;
        }
        else
            break;
    }
    return result;
}

bool Tokenizer::_checkForIdent() {
    if ( _isAlphaClass( _currChar ) || _currChar == '_' ) {
        _appendCurrChar();
        _getNextChar();
        while (_isIdentClass( _currChar )) {
            _appendCurrChar();
            _getNextChar();
        }

        Integer* keywordCode = findKeyword(&(_currToken->getBuffer()));
        if ( keywordCode != nullptr ) {
            _currToken->type = TOK_TYPE_KEYWORD;
            _currToken->code = *keywordCode;
        }
        else {
            _currToken->type = TOK_TYPE_IDENT;
            _currToken->code = INVALID_CODE;
        }
        return true;
    }
    return false;
}

bool Tokenizer::_checkForOperator() {
    Integer* code = findSingleOp( Char(_currChar) );
    if ( code != nullptr ) {
        _appendCurrChar();
        _currToken->type = TOK_TYPE_OPER;
        _currToken->code = *code;
        _getNextChar();
        return true;
    }
    return false;
}

bool Tokenizer::_checkForMultiOp() {
    peekNextChar();
    if ( _findMultiOpChar( Char(_currChar) ) && _findMultiOpChar( Char(_peekChar) ) ) {
        _appendCurrChar();
        _appendNextChar();
        while ( _findMultiOpChar( Char(peekNextChar()) ) )
            _appendNextChar();
        Integer* code = findMultiOp(&(_currToken->getBuffer()));
        if ( code != nullptr ) {
            _currToken->type = TOK_TYPE_OPER;
            _currToken->code = *code;
            _getNextChar();
            return true;
        }
        else
            _restartToken();
    }
    return false;
}

bool Tokenizer::_checkForCharLiteral() {
    if ( _context->isGetCharLits() && _currChar == '\'' ) {
        _getNextChar();
        if ( _currChar == '\'' )
            error( "Invalid character constant" );
        else {
            if ( _currChar == '\\' ) {
                _getNextChar();
                _processEscape();
            }
            _appendCurrChar();
            peekNextChar();
            if ( _peekChar != '\'' )
                _completeQuote( '\'' );
            else {
                _getNextChar();  // Get the single quote...
                _currToken->type = TOK_TYPE_CHAR_LIT;
                _getNextChar();
            }
            return true;
        }
    }
    return false;
}

bool Tokenizer::_checkForQuote() {
    if ( _context->isGetStrings() ) {
        switch ( _currChar ) {
            case '\'':
                if (!isAllowSingleQuotes()) {
                    break;
                }
                // Fall through here...
            case '"':
                return _completeQuote( _currChar );
            default:
                break;
        }
    }
    return false;
}

bool Tokenizer::_completeQuote( char quoteChar ) {
    bool more = true;
    while ( more ) {
        _getNextChar();
        switch ( _currChar ) {
            case '\\':
                _getNextChar();
                switch ( _currChar ) {
                    case ' ':
                        _skipSpaces();
                        if (_currChar != '\n') {
                            continue;
                        }
                        // Fall through here to the next case...
                    case '\n':
                        _skipCRLF();
                        _appendCurrChar();
                        if ( _currChar == '\n' )
                            _advanceLine();
                        continue;
                    default:
                        _processEscape();
                        _appendCurrChar();
                        break;
                }
                break;
            case '"':
                if ( quoteChar == '"' )
                    more = false;
                else
                    _appendCurrChar();
                break;
            case '\'':
                if ( quoteChar == '\'' )
                    more = false;
                else
                    _appendCurrChar();
                break;
            case '\0':
                error( "Unexpected EOF encountered" );
                more = false;
                break;
            default:
                _appendCurrChar();
                if ( _currChar == '\n' )
                    _advanceLine();
                break;
        }
    }
    _currToken->type = quoteChar == '"' ? TOK_TYPE_STRING : TOK_TYPE_STRING_LIT;
    _getNextChar();
    return true;
}

void Tokenizer::_processEscape() {
    switch(_currChar) {
        case 'n':   _currChar = '\n';   // Newline
            break;
        case 'b':   _currChar = '\b';   // Backspace
            break;
        case 't':   _currChar = '\t';   // Horizontal tab
            break;
        case 'v':   _currChar = '\v';   // Vertical tab
            break;
        case 'f':   _currChar = '\f';   // Form feed
            break;
        case 'r':   _currChar = '\r';   // Carriage return
            break;
        case 'a':   _currChar = '\a';   // Alert
            break;
        case '\'':  _currChar = '\'';   // Single quote
            break;
        case '\\':  _currChar = '\\';   // Backslash
            break;
        default:    _currChar = _getEscNumber();
            break;
    }
}

char Tokenizer::_getEscNumber() {
    int value = 0;
    if (toUpper(_currChar) == 'U') {
        int digits = 0;
        while (_isHexClass(peekNextChar()) && digits < 4) {
            _getNextChar();
            ++digits;
            char tmpChar = toUpper(_currChar);
            int  tmpVal  = 0;
            if (tmpChar >= 'A')
                tmpVal = tmpChar - 'A' + 10;
            else
                tmpVal = tmpChar - '0';
            value = (value << 4) + tmpVal;
            if (value > 0xFFFF)
                error("Illegal Unicode constant value... max. is 0xFFFF");
        }
        if (digits != 4)
            error("Illegal Unicode constant... must have 4 hexidecimal digits");
    }
    else if (_currChar == '0') {
        if (toUpper(peekNextChar()) == 'X') {
            _getNextChar();
            if (!_isHexClass(peekNextChar()))
                error("Illegal hexidecimal constant");
            while (_isHexClass(peekNextChar())) {
                _getNextChar();
                char tmpChar = toUpper(_currChar);
                int tmpVal = 0;
                if (tmpChar >= 'A')
                    tmpVal = tmpChar - 'A' + 10;
                else
                    tmpVal = tmpChar - '0';
                value = (value << 4) + tmpVal;
                if (value > 0xFF)
                    error("Illegal hexidecimal constant value... max. is 0xFF");
            }
        }
        else {
            while (_isDigitClass(peekNextChar())) {
                _getNextChar();
                int tmpVal = _currChar - '0';
                if (tmpVal > 7)
                    error("Illegal octal constant");
                value = (value << 3) + tmpVal;
                if (value > 0xFF)
                    error("Illegal octal constant value... max. is 0377");
            }
        }
    }
    else if (_isDigitClass(_currChar)) {
        value = _currChar - '0';
        while (_isDigitClass(peekNextChar())) {
            _getNextChar();
            value = (value * 10) + (_currChar - '0');
            if (value > 0xFF)
                error("Illegal decimal constant value... max. is 255");
        }
    }
    else
        error("Missing escape value");
    return (char)value;
}

void Tokenizer::_calcDoubleValue() {
    if ( _currToken->type == TOK_TYPE_FLOAT_LIT ) {
        _currToken->doubleValue = _currToken->intPortion + ( _currToken->fracPortion / powl( 10, _currToken->fracDigits ) );
        _currToken->doubleValue = _currToken->doubleValue * powl( 10, _currToken->expPortion );
    }
}

bool Tokenizer::_checkForNumber() {
    _currToken->intFound    = false;
    _currToken->fracFound   = false;
    _currToken->infNanFound = false;
    if ( _currChar == '0' ) {
        char peek = peekNextChar();
        if (peek == '.') {
            _doNumber();
            _calcDoubleValue();
        }
        else {
            _appendCurrChar();
            _initNumberScan();
            switch ( peek ) {
                case 'X':
                case 'x':
                    _doHex();
                    break;
                case 'B':
                case 'b':
                    _doBinary();
                    break;
                default:
                    _doOctal();
                    break;
            }
        }
    }
    else {
        _doNumber();
        if (!_currToken->infNanFound) {
            _calcDoubleValue();
        }
    }
    return _currToken->intFound || _currToken->fracFound || _currToken->infNanFound;
}

void Tokenizer::_initNumberScan() {
    _currToken->doubleValue = 0.0;
    _currToken->intRadix    = 10;
    _currToken->fracDigits  = 0;
    _currToken->intPortion  = 0;
    _currToken->fracPortion = 0;
    _currToken->expPortion  = 0;
    _currToken->negFound    = false;
    _currToken->infNanFound = false;
    _currToken->expFound    = false;
    _currToken->expSign     = false;
}

void Tokenizer::_doNumber() {
    if (!_doInfNan()) {
        _doInteger();
        _doFraction();
        if (_currToken->intFound || _currToken->fracFound)
            _doExponent();
    }
}

/**
 * @brief check for Infinity (+/-) and NaN float values
 * 
 * @return true 
 * @return false 
 */
bool Tokenizer::_doInfNan() {
    bool negFound = false;
    int  start    = 0;
    const char* infLit = (const char*)"inf";
    const char* nanLit = (const char*)"nan";
    if ( _currChar == '-') {
        negFound = true;
        start    = 1;
    }
    // Check for Inf
    bool result = true;
    for (int i = 0; i < 3; ++i) {
        char ch = toLower(peekNextChar(start + i));
        if (ch != infLit[i]) {
            result = false;
            break;
        }
    }
    if (result) {
        _appendCurrChar();
        _appendNextChars(2 + (negFound ? 1 : 0));
        _getNextChar();
        _currToken->negFound    = negFound;
        _currToken->infNanFound = true;
        _currToken->type = TOK_TYPE_FLOAT_LIT;
        _currToken->doubleValue = negFound ? -INFINITY : INFINITY;
    }
    else if (!result && !negFound) {
        result = true;
        for ( int i = 0; i < 3; ++i) {
            char ch = toLower(peekNextChar(i));
            if (ch != nanLit[i]) {
                result = false;
                break;
            }
        }
        if (result) {
            _appendCurrChar();
            _appendNextChars(2);
            _getNextChar();
            _currToken->infNanFound = true;
            _currToken->type = TOK_TYPE_FLOAT_LIT;
            _currToken->doubleValue = NAN;
        }
    }
    return result;
}

void Tokenizer::_doInteger() {
    bool negFound = false;
    if ( _currChar == '-') {
        if (_isDigitClass(peekNextChar())) {
            negFound = true;
            _appendCurrChar();
            _getNextChar();
        }
    }
    if ( _isDigitClass( _currChar )) {
        _initNumberScan();
        _appendCurrChar();
        _currToken->negFound   = negFound;
        _currToken->intFound   = true;
        _currToken->type       = TOK_TYPE_INT_LIT;
        _currToken->intPortion = _currChar - '0';

        int  digitCount = 0;
        bool first      = true;
        while (_isDigitClass(_getNextChar())) {
            _appendCurrChar();
            _currToken->intPortion = ( _currToken->intPortion * 10 ) + ( _currChar - '0' );
            if (_context->isAllowNumCommas()) {
                ++digitCount;
                if ( peekNextChar() == ',' ) {
                    if ( digitCount == 3 || ( first && digitCount < 3 ) ) {
                        _getNextChar();
                        digitCount = 0;
                        first = false;
                    }
                    else
                        error( "Invalid comma ',' embedded in number" );
                }
            }
        }
        if (negFound) {
            _currToken->intPortion = -(_currToken->intPortion);
        }
    }
}

void Tokenizer::_doFraction() {
    if (_currChar == '.') {
        if (!_currToken->intFound)
            _initNumberScan();
        if (_isDigitClass(peekNextChar())) {
            _appendCurrChar();
            _currToken->fracFound = true;
            _currToken->type      = TOK_TYPE_FLOAT_LIT;

            while (_isDigitClass(_getNextChar())) {
                _appendCurrChar();
                _currToken->fracPortion = (_currToken->fracPortion * 10) + (_currChar - '0');
                ++(_currToken->fracDigits);
            }
        }
        else if (_currToken->intFound) {
            _appendCurrChar();      // Append the decimal '.'
            _getNextChar();
            _currToken->type = TOK_TYPE_FLOAT_LIT;
        }
    }
}

void Tokenizer::_doExponent() {
    switch(_currChar) {
        case 'E':
        case 'e':
            _appendCurrChar();
            _currToken->expFound = true;
            _currToken->type     = TOK_TYPE_FLOAT_LIT;
            if (peekNextChar() == '-') {
                _currToken->expSign = true;
                _appendNextChar();
            }
            while (_isDigitClass(_getNextChar())) {
                _appendCurrChar();
                _currToken->expPortion = (_currToken->expPortion * 10) + (_currChar - '0');
            }
            if (_currToken->expSign)
                _currToken->expPortion = -(_currToken->expPortion);
            break;
    }
}

void Tokenizer::_doHex() {
    _getNextChar();
    _appendCurrChar();
    _currToken->intRadix = 16;
    _currToken->intFound = true;
    _currToken->type     = TOK_TYPE_INT_LIT;

    if (!_isHexClass(peekNextChar()))
        error("Illegal hex constant");

    while (_isHexClass(_getNextChar())) {
        _appendCurrChar();
        char tmpChar = toUpper(_currChar);
        int tmpVal = 0;
        if (tmpChar >= 'A')
            tmpVal = tmpChar - 'A' + 10;
        else
            tmpVal = tmpChar - '0';
        _currToken->intPortion = (_currToken->intPortion << 4) + tmpVal;
    }
}

void Tokenizer::_doBinary() {
    _getNextChar();
    _appendCurrChar();
    _currToken->intRadix = 2;
    _currToken->intFound = true;
    _currToken->type     = TOK_TYPE_INT_LIT;

    if (!_isBinaryClass(_getNextChar()))
        error("Illegal binary constant");

    while (_isBinaryClass(_getNextChar())) {
        _appendCurrChar();
        char tmpChar = toUpper(_currChar);
        _currToken->intPortion = (_currToken->intPortion << 1) + tmpChar - '0';
    }
}

void Tokenizer::_doOctal() {
    _currToken->intRadix = 8;
    _currToken->intFound = true;
    _currToken->type     = TOK_TYPE_INT_LIT;
    while (_isDigitClass(_getNextChar())) {
        _appendCurrChar();
        int tmpVal = _currChar - '0';
        if (tmpVal > 7)
            error("Illegal octal constant");
        _currToken->intPortion = (_currToken->intPortion << 3) + tmpVal;
    }
}

void Tokenizer::error(String msg) {
    if (!_errorFlag) {
        if (_errorContext == nullptr)
            _errorContext = new ErrorContext;
        _errorContext->errorMsg       = msg;
        _errorContext->errorLine      = _currToken->lineNumber;
        _errorContext->errorLineStart = _lineStart;
        _errorContext->errorStart     = _currToken->tokenPosition;
        _errorContext->errorEnd       = _currPosition;
        _errorFlag                    = true;
        ++_errorCount;
    }
}