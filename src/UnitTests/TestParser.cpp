//
// Created by Ken Kopelson on 18/02/18.
//
#include <iostream>
using namespace std;

#include "catch.hpp"
#include "../Utilities/Parser/Tokenizer.h"
#include "../Utilities/Parser/Token.h"
#include "../Utilities/Parser/ErrorContext.h"

SCENARIO("Parser operates properly") {
    GIVEN("a newly allocated Tokenizer and Token") {
        Tokenizer* tok = new Tokenizer("(a*(b+9)'str\"ing''s'/'\u00FF'century.brat<=0x56,0.34e45|.23,034~0b011010)");
        tok->setGetStrings(true);
        tok->setGetUnknown(true);
        char ops[] = {'*', '/', '-', '+', '.', '$', '(', ')'};
        int  index = 100;
        for (char ch : ops) {
            tok->addSingleOp(new Char(ch), index++);
        }
        tok->addMultiOp(new String("<="), index++);
        tok->addKeyword(new String("brat"), 1000);
        auto token = Token();

        WHEN("we parse the given string") {
            THEN("we get the correct tokens") {
                while (tok->fetchToken(token)) {
//                    cout << "Token: '" << token.getBuffer() << "', type: " << token.type << ", code: " << token.code << endl;
//                    if (token.type == TOK_TYPE_INT_LIT) {
//                        cout << "   Value: " << token.intPortion << endl;
//                    }
//                    else if (token.type == TOK_TYPE_FLOAT_LIT) {
//                        cout << "   Value: " << token.doubleValue << endl;
//                    }
//                    if (tok->isError()) {
//                        cout << "   ERROR: " << tok->getErrorContext()->errorMsg << endl;
//                        break;
//                    }
                }
            }
        }
    }
}

