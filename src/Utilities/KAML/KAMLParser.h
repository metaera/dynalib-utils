#ifndef KAML_PARSER_H
#define KAML_PARSER_H

#include "../String.h"
#include "../DynaList.h"
#include "../IntWrapper.h"
#include "../Parser/Tokenizer.h"
#include "../Parser/Token.h"
#include "../Graph/Node.h"

using namespace Graph;

enum class CastType { NO_CAST, STR_CAST, SEQ_CAST, PROPER_CAST, INT_CAST, FLOAT_CAST, BOOL_CAST, TIME_CAST, TS_CAST, DATE_CAST };

class DynaLogger;

namespace KAML {

    class KAMLParser {
        static DynaLogger* logger;
        Tokenizer* _tok      = nullptr;
        CastType   _currCast = CastType::NO_CAST;
        bool       _unwind   = false;

    public:
        KAMLParser();
        virtual ~KAMLParser();

        Tokenizer* getTok();

        bool  parseKaml(const String& buf, Node* rootNode);
        bool  parseBlock(Token& token, Node* parentNode, int parentIndent);
        bool  parseTypeCast(Token& token);
        void  applyTypeCast(Node* valueNode);
        bool  parseBlockValue(Token& token, Node* parentNode);
        bool  parseFlowValue(Token& token, Node* parentNode);
        bool  parseFlowList(Token& token, Node* parentNode);
        bool  parseFlowMap(Token& token, Node* parentNode);
        bool  parseScalar(Token& token, Node* scalarNode);
        bool  parseUnquotedString(Token& token, Node* scalarNode);
        bool  parseFormattedString(Token& token, Node* scalarNode);
    };
}

#endif