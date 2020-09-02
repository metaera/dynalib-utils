#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "../String.h"
#include "../DynaList.h"
#include "../IntWrapper.h"
#include "../Parser/Tokenizer.h"
#include "../Parser/Token.h"
#include "../Graph/Node.h"

using namespace Graph;

class DynaLogger;

namespace KAML {

    class JSONParser {
        static DynaLogger* logger;
        Tokenizer* _tok;

    public:
        JSONParser();
        virtual ~JSONParser();

        Tokenizer* getTok();

        bool  parseJson(const String& buf, Node* rootNode);
        bool  parseValue(Token& token, Node* parentNode);
        bool  parseArray(Token& token, Node* parentNode);
        bool  parseObject(Token& token, Node* parentNode);
        bool  processTypeObject(Node& objNode);
        bool  parseScalar(Token& token, Node* scalarNode);
    };
}

#endif