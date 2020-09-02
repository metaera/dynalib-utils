//
// Created by Ken Kopelson on 18/02/18.
//

#ifndef HR1FIRMWARE_TOKENIZERSTATE_H
#define HR1FIRMWARE_TOKENIZERSTATE_H

class Token;

struct TokenizerState {
    int  currPosition;
    int  linePosition;
    int  lineStart;
    int  indentPosition;
    char currChar;
    char peekChar;
    bool errorFlag;
    int  errorCount;
    bool errorStop;
    int  lineNumber;
    Token* currToken;
};


#endif //HR1FIRMWARE_TOKENIZERSTATE_H
