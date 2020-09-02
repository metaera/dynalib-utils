//
// Created by Ken Kopelson on 8/11/17.
//

#ifndef ROBOCHEFEMULATOR_ERRORCONTEXT_H
#define ROBOCHEFEMULATOR_ERRORCONTEXT_H


#include "../String.h"

struct ErrorContext {
    String  errorMsg;
    int     errorLine      = 0;
    int     errorLineStart = 0;
    int     errorStart     = 0;
    int     errorEnd       = 0;
};


#endif //ROBOCHEFEMULATOR_ERRORCONTEXT_H
