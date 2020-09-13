//
// Created by Ken Kopelson on 8/11/17.
//

#ifndef TOKENIZER_H
#define TOKENIZER_H


#include "../String.h"
#include "../IntWrapper.h"

class  Token;
class  TokenizerContext;
struct TokenizerState;
struct ErrorContext;

class Tokenizer {
    const String _text;
    int     _currPosition   = -1;
    int     _linePosition   = 0;
    int     _lineStart      = 0;
    int     _indentPosition = 0;
    Token*  _currToken      = nullptr;
    char    _currChar       = '\0';
    char    _peekChar       = '\0';
    bool    _errorFlag      = false;
    int     _errorCount     = 0;
    bool    _errorStop      = false;
    int     _lineNumber     = 0;
    bool    _contextCreated = false;

    TokenizerContext* _context      = nullptr;
    ErrorContext*     _errorContext = nullptr;

public:
    Tokenizer(TokenizerContext* context, const String& text);
    explicit Tokenizer(TokenizerContext* context);
    explicit Tokenizer(const String& text);
    Tokenizer();
    virtual ~Tokenizer();

    void init();
    void setContext(TokenizerContext* context);
    TokenizerContext* getContext();
    void setText(const String& text);
    void setText(String& text);
    void setCaseSensitive(bool caseSensitive);
    bool isCaseSensitive();
    void setGetCharLits(bool getCharLits);
    bool isGetCharLits();
    void setGetStrings(bool getStrings);
    bool isGetStrings();
    void setAllowSingleQuotes(bool allowSingleQuotes);
    bool isAllowSingleQuotes();
    void setGetUnknown(bool getUnknown);
    bool isGetUnknown();
    void setGetCRLF(bool getCRLF);
    bool isGetCRLF();
    void setGetSpaces(bool getSpaces);
    bool isGetSpaces();
    void setAllowNumCommas(bool allowNumCommas);
    bool isAllowNumCommas();

    void resetScan();
    Integer* findKeyword(String* keyword);
    Integer* findKeyword(String keyword);
    bool addKeyword(String* keyword, int code);
    Integer* findSingleOp(Char* singleOp);
    Integer* findSingleOp(Char singleOp);
    bool addSingleOp(Char* singleOp, int code);
    Integer* findMultiOp(String* multiOp);
    Integer* findMultiOp(String multiOp);
    bool addMultiOp(String* multiOp, int code);
    bool findIdentChar(Char* char1);
    bool findIdentChar(Char char1);
    void addIdentChars(const String& multiChars);
    void clearIdentChars();
    void setIdentChars(const String& multiChars);

    bool            hasNextChar(int index = 1);
    char            peekNextChar(int index = 1);
    char            peekCurrChar();
    bool            isEOS();
    bool            fetchToken(Token& token);
    int             getCurrPosition();
    void            setIndentHere();
    void            setIndentPosition(int indentPos);
    int             getIndentPosition();
    bool            hasIndented(int indentPos);
    bool            hasOutdented(int indentPos);
    void            resetIndent();
    Token*          getCurrToken();
    String          getTokenLineSegment();
    bool            isError();
    ErrorContext*   getErrorContext();
    void            setErrorContext(ErrorContext* errorContext);
    String          getErrorLineSegment();
    int             getErrorCount();
    TokenizerState* saveState();
    Token*          restoreState(TokenizerState* state);
    void            deleteState(TokenizerState* state);
    void            restartFromToken(Token& token);
    void            error(String msg);

private:
    bool _isBinaryClass(char ch);
    bool _isDigitClass(char ch);
    bool _isAlphaClass(char ch);
    bool _isHexClass(char ch);
    bool _isIdentClass(char ch);

    bool _findMultiOpChar(Char* char1);
    bool _findMultiOpChar(Char char1);
    void _advanceLine();
    char _getNextChar();
    void _appendCurrChar();
    void _appendNextChar();
    void _appendNextChars(int count);
    void _restartToken();
    bool _getNextToken();
    char _skipSpaces();
    void _skipCRLF();
    bool _skipComment();
    bool _checkForIdent();
    bool _checkForOperator();
    bool _checkForMultiOp();
    bool _checkForCharLiteral();
    bool _checkForQuote();
    bool _completeQuote(char quoteChar);
    void _processEscape();
    char _getEscNumber();
    void _calcDoubleValue();
    bool _checkForNumber();
    void _initNumberScan();
    void _doNumber();
    bool _doInfNan();
    void _doInteger();
    void _doFraction();
    void _doExponent();
    void _doHex();
    void _doBinary();
    void _doOctal();
};


#endif //TOKENIZER_H
