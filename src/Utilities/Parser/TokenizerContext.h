//
// Created by Ken Kopelson on 8/11/17.
//

#ifndef TOKENIZERCONTEXT_H
#define TOKENIZERCONTEXT_H

#include "../IntWrapper.h"
#include "../DynaHashMap.h"
#include "../DynaHashSet.h"
#include "../String.h"
#include "CharacterClass.h"

class TokenizerContext {
    bool _caseSensitive     = false;
    bool _getStrings        = true;
    bool _getCharLits       = false;
    bool _getUnknown        = false;
    bool _getCRLF           = false;
    bool _getSpaces         = false;
    bool _innerApostrophe   = false;
    bool _allowNumCommas    = false;
    bool _allowSingleQuotes = false;

    DynaHashMap<String, Integer>*   _keyWords;
    DynaHashMap<Char,   Integer>*   _singleOps;
    DynaHashSet<Char>*              _multiOpSet;
    DynaHashMap<String, Integer>*   _multiOps;
    CharacterClass*                 _charClass;

    bool _addMultiOpChar(Char* char1);

public:
    TokenizerContext();
    ~TokenizerContext();

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
    void setAllowInnerApostrophe(bool innerApostrophe);
    bool isAllowInnerApostrophe();
    void setCharClass(CharacterClass* charClass);
    CharacterClass* getCharClass();

    void init();
    Integer* findKeyword(String* keyword);

    /**
     * @brief Adds keywords to the Tokenizer context
     * 
     * @param keyword expects a "new" String of the keyword
     * @param code 
     * @return true 
     * @return false 
     */
    bool     addKeyword(String* keyword, int code);
    Integer* findSingleOp(Char* singleOp);

    /**
     * @brief Adds a single-character operator
     * 
     * @param singleOp expects a "new" Char of the operator
     * @param code 
     * @return true 
     * @return false 
     */
    bool     addSingleOp(Char* singleOp, int code);
    Integer* findMultiOp(String* multiOp);

    /**
     * @brief Adds a multi-character operator
     * 
     * @param multiOp expects a "new" String of the operator
     * @param code 
     * @return true 
     * @return false 
     */
    bool     addMultiOp(String* multiOp, int code);
    bool     findIdentChar(Char* char1);

    /**
     * @brief Adds a list of the characters that can make up an identifier, to the already existing list
     * 
     * @param multiChars can be a stack-based String, as each character will be extracted and made "new"
     */
    void     addIdentChars(const String& multiChars);
    void     clearIdentChars();
    void     setIdentChars(const String& multiChars);

    bool     isAlphaClass(char ch);
    bool     isDigitClass(char ch);
    bool     isHexClass(char ch);
    bool     isVowelClass(char ch);
    bool     isConsonantClass(char ch);
    bool     isIdentClass(char ch);

    bool     findMultiOpChar(Char* char1);
};


#endif //TOKENIZERCONTEXT_H
