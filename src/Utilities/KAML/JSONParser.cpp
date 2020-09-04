#include <cmath>
#include "../DynaListImpl.h"
#include "../Parser/Token.h"
#include "../Parser/Tokenizer.h"
#include "../Parser/ErrorContext.h"
#include "../DynaLogger.h"

#include "JSONParser.h"
#include "../TimeUtils.h"

#define COMMA_CODE      BASE_USER_CODE + 1
#define COLON_CODE      BASE_USER_CODE + 2
#define SEMICOLON_CODE  BASE_USER_CODE + 3
#define LBRACK_CODE     BASE_USER_CODE + 4
#define RBRACK_CODE     BASE_USER_CODE + 5
#define LBRACE_CODE     BASE_USER_CODE + 6
#define RBRACE_CODE     BASE_USER_CODE + 7
#define BACKSLASH_CODE  BASE_USER_CODE + 8
#define TRUE_CODE       BASE_USER_CODE + 9
#define FALSE_CODE      BASE_USER_CODE + 10
#define NULL_CODE       BASE_USER_CODE + 11

#define TYPE_TAG        "type"
#define VALUE_TAG       "value"

#define SEQ_TYPE        "seq"
#define PROPER_TYPE     "proper"
#define TIME_TYPE       "time"
#define TIMESTAMP_TYPE  "ts"
#define DATE_TYPE       "date"
#define FLOAT_TYPE      "float"

using namespace KAML;

DynaLogger* JSONParser::logger = DynaLogger::getLogger("JSON-Pars");

namespace KAML {
    JSONParser::JSONParser() {
        _tok = new Tokenizer();
        _tok->setGetStrings(true);
        _tok->setAllowSingleQuotes(false);
        _tok->setGetUnknown(false);
        _tok->setGetCharLits(false);
        char ops[] = {',', ':', ';', '[', ']', '{', '}', '\\'};
        int  index = COMMA_CODE;
        for (char ch : ops) {
            _tok->addSingleOp(new Char(ch), index++);
        }
        _tok->addKeyword(new String("true"),    TRUE_CODE);
        _tok->addKeyword(new String("false"),   FALSE_CODE);
        _tok->addKeyword(new String("null"),    NULL_CODE);
    }

    JSONParser::~JSONParser() {
        delete _tok;
    }

    Tokenizer* JSONParser::getTok() {
        return _tok;
    }

    bool JSONParser::parseJson(const String& buf, Node* rootNode) {
        _tok->setText(buf);
        auto token = Token();
        try {
            _tok->fetchToken(token);
            while (true) {
                if (token.type == TOK_TYPE_SPECIAL && token.code == EOS_CODE) {
                    break;
                }
                if (!parseValue(token, rootNode)) {
                    _tok->error("Parsing terminated prematurely");
                    return false;
                }
            }
        }
        catch (Exception e) {
            logger->error("JSON file error at line %d: %s", token.lineNumber, e.getMessage().c_str());
            return false;
        }
        return true;
    }

    bool JSONParser::parseValue(Token& token, Node* parentNode) {
        bool  result = false;

        if (token.type == TOK_TYPE_OPER) {
            if (token.code == LBRACK_CODE) {
                result = parseArray(token, parentNode);
                return result;
            }
            else if (token.code == LBRACE_CODE) {
                result = parseObject(token, parentNode);
                if (result) {
                    processTypeObject(*parentNode);
                }
                return result;
            }
            // Fall through to the parseScalar below...
        }
        result = parseScalar(token, parentNode);
        return result;
    }

    bool JSONParser::parseArray(Token& token, Node* parentNode) {
        bool result = false;
        Node* valueNode = nullptr;

        if (token.type == TOK_TYPE_OPER && token.code == LBRACK_CODE) {
            _tok->fetchToken(token);
            while (true) {
                if (token.type == TOK_TYPE_SPECIAL && token.code == EOS_CODE) {
                    break;
                }
                if (token.type == TOK_TYPE_OPER) {
                    switch (token.code) {
                        case RBRACK_CODE: {
                            _tok->fetchToken(token);
                            return true;
                        }
                        case COMMA_CODE: {
                            _tok->fetchToken(token);
                            break;
                        }
                        default:
                            break;
                    }
                }
                valueNode = parentNode->addUntypedNode();
                result = parseValue(token, valueNode);
                if (!result) {
                    break;
                }
            }
        }
        return result;
    }

    bool JSONParser::parseObject(Token& token, Node* parentNode) {
        bool  result = false;
        Node* valueNode = nullptr;

        if (token.type == TOK_TYPE_OPER && token.code == LBRACE_CODE) {
            _tok->fetchToken(token);
            while (true) {
                if (token.type == TOK_TYPE_SPECIAL && token.code == EOS_CODE) {
                    break;
                }
                if (token.type == TOK_TYPE_OPER) {
                    switch (token.code) {
                        case RBRACE_CODE: {
                            _tok->fetchToken(token);
                            return true;
                        }
                        case COMMA_CODE: {
                            _tok->fetchToken(token);
                            break;
                        }
                        default:
                            return false;
                    }
                }
                if (token.type == TOK_TYPE_STRING) {
                    auto key = token.getBuffer();
                    _tok->fetchToken(token);
                    if (token.type == TOK_TYPE_OPER && token.code == COLON_CODE) {
                        _tok->fetchToken(token);
                        valueNode = parentNode->addUntypedNode(key);
                    }
                    else {
                        break;
                    }
                    result = parseValue(token, valueNode);
                    if (!result) {
                        break;
                    }
                }
                else {
                    break;
                }
            }
        }
        return result;
    }

    bool JSONParser::processTypeObject(Node& objNode) {
        bool result = false;
        if (objNode.isMap() && objNode.value().mapValue->count() == 2) {
            Node* type  = objNode(TYPE_TAG);
            if (type != nullptr) {
                Node* value = objNode(VALUE_TAG);
                if (value != nullptr) {
                    String typeStr = objNode[TYPE_TAG].asString();

                    if (typeStr == SEQ_TYPE) {
                        auto valStr = objNode[VALUE_TAG].asString();
                        objNode.setScalar(valStr.c_str(), ScalarType::SEQ);
                        result = true;
                    }
                    else if (typeStr == PROPER_TYPE) {
                        auto valStr = objNode[VALUE_TAG].asString();
                        objNode.setScalar(valStr.c_str(), ScalarType::PROPER);
                        result = true;
                    }
                    else if (typeStr == TIME_TYPE) {
                        String valStr = objNode[VALUE_TAG].asString();
                        auto tval = TimeUtils::parseLocalTime(valStr.c_str());
                        objNode.setScalar(tval, ScalarType::TIME);
                        result = true;
                    }
                    else if (typeStr == TIMESTAMP_TYPE) {
                        String valStr = objNode[VALUE_TAG].asString();
                        auto tval = TimeUtils::parseTimestamp(valStr.c_str());
                        objNode.setScalar(tval, ScalarType::TIMESTAMP);
                        result = true;
                    }
                    else if (typeStr == DATE_TYPE) {
                        String valStr = objNode[VALUE_TAG].asString();
                        auto tval = TimeUtils::parseLocalDate(valStr.c_str());
                        objNode.setScalar(tval, ScalarType::DATE);
                        result = true;
                    }
                    else if (typeStr == FLOAT_TYPE) {
                        String valStr = objNode[VALUE_TAG].asString().tolower();
                        double fval   = 0.0;
                        if (valStr == "inf") {
                            fval = INFINITY;
                        }
                        else if (valStr == "-inf") {
                            fval = -INFINITY;
                        }
                        else if (valStr == "nan") {
                            fval = NAN;
                        }
                        objNode.setScalar(fval);
                        return true;
                    }
                }
            }
        }
        return result;
    }

    bool JSONParser::parseScalar(Token& token, Node* scalarNode) {
        bool result = false;
        switch (token.type) {
            case TOK_TYPE_FLOAT_LIT: {
                scalarNode->setScalar(token.doubleValue);
                _tok->fetchToken(token);
                result = true;
                break;
            }
            case TOK_TYPE_INT_LIT: {
                scalarNode->setScalar((long long)token.intPortion);
                _tok->fetchToken(token);
                result = true;
                break;
            }
            case TOK_TYPE_KEYWORD: {
                if (token.code == TRUE_CODE) {
                    token.boolValue = true;
                    scalarNode->setScalar(token.boolValue);
                }
                else if (token.code == FALSE_CODE) {
                    token.boolValue = false;
                    scalarNode->setScalar(token.boolValue);
                }
                else if (token.code == NULL_CODE) {
                    scalarNode->setUntypedNode();
                }
                _tok->fetchToken(token);
                result = true;
                break;
            }
            case TOK_TYPE_STRING: {
                scalarNode->setScalar(token.getBuffer().c_str());
                _tok->fetchToken(token);
                result = true;
                break;
            }
        }            
        return result;
    }
}