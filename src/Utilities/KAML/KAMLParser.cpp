#include "../DynaListImpl.h"
#include "../Parser/Token.h"
#include "../Parser/Tokenizer.h"
#include "../Parser/ErrorContext.h"
#include "../DynaLogger.h"

#include "KAMLParser.h"
#include "../TimeUtils.h"

// #define DEBUG

#define PERIOD_CODE     BASE_USER_CODE + 1
#define COMMA_CODE      BASE_USER_CODE + 2
#define DASH_CODE       BASE_USER_CODE + 3
#define COLON_CODE      BASE_USER_CODE + 4
#define SEMICOLON_CODE  BASE_USER_CODE + 5
#define LBRACK_CODE     BASE_USER_CODE + 6
#define RBRACK_CODE     BASE_USER_CODE + 7
#define LBRACE_CODE     BASE_USER_CODE + 8
#define RBRACE_CODE     BASE_USER_CODE + 9
#define AT_CODE         BASE_USER_CODE + 10
#define VBAR_CODE       BASE_USER_CODE + 11
#define BACKSLASH_CODE  BASE_USER_CODE + 12
#define LT_CODE         BASE_USER_CODE + 13
#define GT_CODE         BASE_USER_CODE + 14
#define HASH_CODE       BASE_USER_CODE + 15
#define BREAK_CODE      BASE_USER_CODE + 16
#define TRUE_CODE       BASE_USER_CODE + 17
#define FALSE_CODE      BASE_USER_CODE + 18
#define NULL_CODE       BASE_USER_CODE + 19
#define STR_CODE        BASE_USER_CODE + 20
#define SEQ_CODE        BASE_USER_CODE + 21
#define PROPER_CODE     BASE_USER_CODE + 22
#define INT_CODE        BASE_USER_CODE + 23
#define FLOAT_CODE      BASE_USER_CODE + 24
#define BOOL_CODE       BASE_USER_CODE + 25
#define TIME_CODE       BASE_USER_CODE + 26
#define TIMESTAMP_CODE  BASE_USER_CODE + 27
#define DATE_CODE       BASE_USER_CODE + 28

using namespace KAML;

DynaLogger* KAMLParser::logger = DynaLogger::getLogger("KAML-Pars");

namespace KAML {

    KAMLParser::KAMLParser() {
        _tok = new Tokenizer();
        _tok->setGetStrings(true);
        _tok->setAllowSingleQuotes(true);
        _tok->setGetCRLF(false);
        _tok->setGetSpaces(false);
        _tok->setGetUnknown(false);
        _tok->setGetCharLits(false);
        _tok->addIdentChars(String("_"));
        char ops[] = {'.', ',', '-', ':', ';', '[', ']', '{', '}', '@', '|', '\\', '<', '>', '#'};
        int  index = PERIOD_CODE;
        for (char ch : ops) {
            _tok->addSingleOp(new Char(ch), index++);
        }
        _tok->addMultiOp(new String("---"), BREAK_CODE);

        _tok->addKeyword(new String("true"),    TRUE_CODE);
        _tok->addKeyword(new String("false"),   FALSE_CODE);
        _tok->addKeyword(new String("null"),    NULL_CODE);
        _tok->addKeyword(new String("str"),     STR_CODE);
        _tok->addKeyword(new String("seq"),     SEQ_CODE);
        _tok->addKeyword(new String("proper"),  PROPER_CODE);
        _tok->addKeyword(new String("int"),     INT_CODE);
        _tok->addKeyword(new String("float"),   FLOAT_CODE);
        _tok->addKeyword(new String("bool"),    BOOL_CODE);
        _tok->addKeyword(new String("time"),    TIME_CODE);
        _tok->addKeyword(new String("ts"),      TIMESTAMP_CODE);
        _tok->addKeyword(new String("date"),    DATE_CODE);
    }

    KAMLParser::~KAMLParser() {
        delete _tok;
    }

    Tokenizer* KAMLParser::getTok() {
        return _tok;
    }

    bool KAMLParser::parseKaml(const String& buf, Node* rootNode) {
        _tok->setText(buf);
        _tok->setGetCRLF(false);
        _tok->setGetSpaces(false);
        _tok->setGetUnknown(false);
        auto    token   = Token();
        Node*   docNode = nullptr;
        _unwind         = false;
        try {
            _tok->fetchToken(token);
            while (true) {
                _unwind = false;
                int indentPos = token.linePosition;
                // _tok->resetIndent();
                if (token.type == TOK_TYPE_SPECIAL && token.code == EOS_CODE) {
                    break;
                }
                else if (token.type == TOK_TYPE_OPER) {
                    if (token.code == BREAK_CODE) {
                        docNode = rootNode->addUntypedNode();
                        _tok->fetchToken(token);
                        continue;
                    }
                }
                if (docNode == nullptr) {
                    // If no docNode has been added with "---", add a default one
                    docNode = rootNode->addUntypedNode();
                }
                if (!parseBlock(token, docNode, indentPos, 0)) {
                    _tok->error("Parsing terminated prematurely");
                    return false;
                }
            }
        }
        catch (Exception e) {
            logger->error("KAML file error at line %d: %s", token.lineNumber, e.getMessage().c_str());
            return false;
        }
        return true;
    }


/**
 * test: "string"
 * test:    - "string"
 *          - "string"
 * test:
 *      -   key: 1
 *          key2: 2
 *          key3: 3
 *      -   key: 1
 *          key2: 2
 * 
 * test:
 *      // This is indented, so begins a new list as the map value
 *      -
 *          // This is indented, so begins a new map
 *          key: 1
 *          key2: 2
 *      -
 *          key: 1
 *          key2: 2
 * 
 * test:
 *      key: 1
 *      key2: 2
 * ---
 * - A
 * - B
 * -    - 1
 *      - 2
 * - C
 * 
 * // The above parses as ["A", "B", [1, 2], "C"]
 * 
 */


    bool KAMLParser::parseBlock(Token& token, Node* parentNode, int parentIndent, int level) {
        auto indentPos = parentIndent;
        #ifdef DEBUG
        cout << "Enter block at line: " << token.lineNumber << ", indent: " << indentPos << ", level: " << level << endl;
        #endif
        while (!_unwind) {
            if (token.lineNumber == 31) {
                int i = 0;
            }
            if (token.type == TOK_TYPE_SPECIAL && token.code == EOS_CODE) {
                break;
            }
            if (_tok->hasOutdented(indentPos)) {
                #ifdef DEBUG
                cout << "Outdented from: " << indentPos << endl;
                #endif
                break;
            }
            bool parseNotFound = false;
            if (token.type == TOK_TYPE_OPER) {
                if (token.code == BREAK_CODE) {
                    _unwind = true;
                    break;
                }
                if (token.code == DASH_CODE) {
                    #ifdef DEBUG
                    cout << "Dash code at: " << token.linePosition << endl;
                    #endif
                    if (_tok->hasIndented(indentPos)) {
                        auto savePos = token.linePosition;
                        #ifdef DEBUG
                        cout << "Indented from: " << indentPos << endl;
                        #endif
                        Node* subNode = nullptr;
                        if (parentNode->isList()) {
                            Node* temp = parentNode->list().last();
                            if (temp != nullptr && temp->isList()) {
                                subNode = temp;
                            }
                            else {
                                subNode = parentNode;
                            }
                        }
                        else if (parentNode->isUntyped()) {
                            parentNode->setList();
                            subNode = parentNode;
                            indentPos = savePos;
                        }
                        if (subNode == nullptr) {
                            _tok->error("Invalid syntax - invalid parent for subnodes");
                            return false;
                        }
                        /**
                         * This call to parseBlock is for the purpose of starting a new list node, or the
                         * continuation of a parent list by adding sub-lists to it.  When this call executes
                         * the called function will go to the parseBlock below, since the indent level will be zero.
                         */
                        #ifdef DEBUG
                        cout << endl << "  Calling PB1: " << savePos << endl;
                        #endif
                        if (!parseBlock(token, subNode, savePos, level + 1)) {
                            if (_tok->isError()) {
                                return false;
                            }
                        }
                        #ifdef DEBUG
                        cout << "  Returned from PB1, indent: " << indentPos << ", save: " << savePos << endl;
                        cout << "  Returned to Level: " << level << endl;
                        #endif
                        if (level == 1) {
                            break;      // Return to level 0
                        }
                        continue;
                    }
                    else {
                        _tok->fetchToken(token);
                        /**
                         * This parseBlock will actually add new entries to the list node (parentNode), ultimately
                         * calling parseBlockValue.
                         */
                        #ifdef DEBUG
                        cout << endl << "    Calling PB2: " << indentPos << endl;
                        #endif
                        if (!parseBlock(token, parentNode->addUntypedNode(), indentPos, level)) {
                            if (_tok->isError()) {
                                return false;
                            }
                        }
                        #ifdef DEBUG
                        cout << "    Returned from PB2" << endl;
                        #endif
                        continue;
                    }
                }
                else if (token.code == HASH_CODE) {
                    auto* state = _tok->saveState();
                    int index = 0;
                    _tok->fetchToken(token);
                    if (token.type == TOK_TYPE_INT_LIT) {
                        index = token.intPortion;
                        _tok->fetchToken(token);
                    }
                    else {
                        _tok->restoreState(state);
                    }
                    if (_tok->hasIndented(indentPos)) {
                        auto  savePos = token.linePosition;
                        Node* subNode = nullptr;
                        if (parentNode->isList()) {
                            Node* temp = (*parentNode)(index);
                            if (temp != nullptr) {
                                subNode = temp;
                            }
                        }
                        if (subNode == nullptr) {
                            _tok->error("Invalid syntax - invalid node specified");
                            return false;
                        }
                        if (!parseBlock(token, subNode, savePos, level + 1)) {
                            if (_tok->isError()) {
                                return false;
                            }
                        }
                        // if (level == 1) {
                        //     break;      // Return to level 0
                        // }
                        continue;
                    }
                    else {
                        _tok->fetchToken(token);
                        auto* node = parentNode->isList() ? (*parentNode)(index) : nullptr;
                        if (node == nullptr) {
                            // If it's not there, or not a list, add a new unTyped node into the parent...
                            node = parentNode->addUntypedNode();
                        }
                        if (!parseBlock(token, node, indentPos, level)) {
                            if (_tok->isError()) {
                                return false;
                            }
                        }
                        continue;
                    }
                }
                else {
                    parseNotFound = true;
                }
            }
            else if (token.type == TOK_TYPE_IDENT || token.type == TOK_TYPE_KEYWORD ||
                     token.type == TOK_TYPE_STRING || token.type == TOK_TYPE_STRING_LIT) {
                #ifdef DEBUG
                cout << "Ident at line pos: " << token.linePosition << endl;
                #endif
                int   savePos     = token.linePosition;
                auto* state       = _tok->saveState();
                auto  key         = token.getBuffer();
                /**
                 * @brief skip CRLFs/Spaces then check for ":" to see if we have a map entry
                 */
                _tok->setGetCRLF(false);
                _tok->setGetSpaces(false);
                _tok->fetchToken(token);
                if (token.type == TOK_TYPE_OPER && token.code == COLON_CODE) {
                    _tok->deleteState(state);
                    _tok->fetchToken(token);
                    indentPos = savePos;
                    // Look for the key to see if the user is trying to edit an existing value...
                    auto* node = parentNode->isMap() ? (*parentNode)(key) : nullptr;
                    if (node == nullptr) {
                        // If it's not there, add a new unTyped node at the key specified...
                        node = parentNode->addUntypedNode(key);
                    }
                    #ifdef DEBUG
                    cout << endl << "Calling PB0: " << savePos << endl;
                    #endif
                    if (!parseBlock(token, node, savePos, level + 1)) {
                        return false;
                    }
                    #ifdef DEBUG
                    cout << "Returned from PB0...continue loop" << endl;
                    #endif
                    continue;
                }
                else {
                    // No map key was found so restart the token scan from the ident/string
                    _tok->restoreState(state);
                }
            }
            else {
                parseNotFound = true;
            }
            #ifdef DEBUG
            cout << "  >>> Parsing block value" << endl;
            #endif
            if (!parseBlockValue(token, parentNode, level)) {
                if (_tok->isError()) {
                    return false;
                }
                if (parseNotFound) {
                    _tok->error("No valid tokens found...avoiding infinite loop");
                    return false;
                }
            }
            break;
        }
        return true;
    }

    bool KAMLParser::parseTypeCast(Token& token) {
        bool result = true;
        if (token.type == TOK_TYPE_OPER && token.code == LT_CODE) {
            _tok->fetchToken(token);
            if (token.type == TOK_TYPE_KEYWORD) {
                switch (token.code) {
                    case STR_CODE: {
                        _currCast = CastType::STR_CAST;
                        break;
                    }
                    case SEQ_CODE: {
                        _currCast = CastType::SEQ_CAST;
                        break;
                    }
                    case PROPER_CODE: {
                        _currCast = CastType::PROPER_CAST;
                        break;
                    }
                    case INT_CODE: {
                        _currCast = CastType::INT_CAST;
                        break;
                    }
                    case FLOAT_CODE: {
                        _currCast = CastType::FLOAT_CAST;
                        break;
                    }
                    case BOOL_CODE: {
                        _currCast = CastType::BOOL_CAST;
                        break;
                    }
                    case TIME_CODE: {
                        _currCast = CastType::TIME_CAST;
                        break;
                    }
                    case TIMESTAMP_CODE: {
                        _currCast = CastType::TS_CAST;
                        break;
                    }
                    case DATE_CODE: {
                        _currCast = CastType::DATE_CAST;
                        break;
                    }
                    default: {
                        logger->error("KAML file error at line %d: %s", token.lineNumber, "invalid cast");
                        result = false;
                        break;
                    }
                }
                _tok->fetchToken(token);
                if (token.type == TOK_TYPE_OPER && token.code == GT_CODE) {
                    _tok->fetchToken(token);
                }
            }
        }
        return result;
    }

    void KAMLParser::applyTypeCast(Node* valueNode) {
        switch (_currCast) {
            case CastType::NO_CAST:
                break;
            case CastType::STR_CAST: {
                auto sval = valueNode->asString();
                valueNode->setScalar(sval.c_str());
                break;
            }
            case CastType::SEQ_CAST: {
                auto sval = valueNode->asString();
                valueNode->setScalar(sval.c_str(), ScalarType::SEQ);
                break;
            }
            case CastType::PROPER_CAST: {
                auto sval = valueNode->asString();
                valueNode->setScalar(sval.c_str(), ScalarType::PROPER);
                break;
            }
            case CastType::INT_CAST: {
                auto ival = valueNode->asInt();
                valueNode->setScalar(ival);
                break;
            }
            case CastType::FLOAT_CAST: {
                auto fval = valueNode->asFloat();
                valueNode->setScalar(fval);
                break;
            }
            case CastType::BOOL_CAST: {
                auto bval = valueNode->asBool();
                valueNode->setScalar(bval);
                break;
            }
            case CastType::TIME_CAST: {
//                auto tval = TimeUtils::parseLocalTime(valueNode->asString().c_str());
                auto tval = valueNode->asTime();
                valueNode->setScalar(tval, ScalarType::TIME);
                break;
            }
            case CastType::TS_CAST: {
//                auto tval = TimeUtils::parseTimestamp(valueNode->asString().c_str());
                auto tval = valueNode->asTimestamp();                
                valueNode->setScalar(tval, ScalarType::TIMESTAMP);
                break;
            }
            case CastType::DATE_CAST: {
//                auto dval = TimeUtils::parseLocalDate(valueNode->asString().c_str());
                auto dval = valueNode->asDate();
                valueNode->setScalar(dval, ScalarType::DATE);
                break;
            }
        }
    }

    /**
     * @brief parse the next value (scalar, list entry, or map entry)
     * 
     * @param token 
     * @param parentNode this must be a list or map
     * @return parse result (true - parse was successful, false - parse failed)
     */
    bool KAMLParser::parseBlockValue(Token& token, Node* parentNode, int level) {
        bool  result = false;
        CastType saveCast = _currCast;
        bool timeFound = false;
        bool keyFound  = false;

        Node* valueNode = nullptr;

        if (valueNode == nullptr) {
            if (parentNode->isList()) {
                valueNode = parentNode->addUntypedNode();
            }
            else {
                valueNode = parentNode;
            }
        }
        if (!parseTypeCast(token)) {
            return false;
        }
        if (token.type == TOK_TYPE_OPER) {
            if (token.code == LBRACK_CODE) {
                result = parseFlowList(token, valueNode);
                _currCast = saveCast;
                return result;
            }
            else if (token.code == LBRACE_CODE) {
                result = parseFlowMap(token, valueNode);
                _currCast = saveCast;
                return result;
            }
            else if (token.code == DASH_CODE) {
                if (keyFound) {
                    // If we found a key above, set the indent level at the
                    // found dash position so that no indenting behaviour occurs
                    _tok->setIndentHere();
                }
                _tok->fetchToken(token);
                result = parseBlock(token, valueNode, true, level + 1);
                _currCast = saveCast;
                return result;
            }
            // Falls through to the parseScalar below...
        }
        result = parseScalar(token, valueNode);
        if (result) {
            applyTypeCast(valueNode);
        }
        _currCast = saveCast;
        return result;
    }

    /**
     * @brief parse the next flow value (scalar, list entry, or map entry)
     * 
     * @param token 
     * @param parentNode this must be a list or map
     * @return parse result (true - parse was successful, false - parse failed)
     */
    bool KAMLParser::parseFlowValue(Token& token, Node* parentNode) {
        bool  result = false;
        CastType saveCast = _currCast;
        bool timeFound = false;
        bool keyFound  = false;

        Node* valueNode = nullptr;

        if (token.type == TOK_TYPE_IDENT || token.type == TOK_TYPE_STRING || token.type == TOK_TYPE_STRING_LIT) {
            auto* state    = _tok->saveState();
            auto  key      = token.getBuffer();
            /**
             * @brief skip CRLFs/Spaces then check for ":" to see if we have a map entry
             */
            _tok->setGetCRLF(false);
            _tok->setGetSpaces(false);
            _tok->fetchToken(token);
            if (token.type == TOK_TYPE_OPER && token.code == COLON_CODE) {
                _tok->deleteState(state);
                _tok->fetchToken(token);
                keyFound = true;

                // If a key is found, then add it with a new "null node" as the value into the parentNode map
                valueNode = parentNode->addUntypedNode(key);
            }
            else {
                // No map key was found so restart the token scan from the ident/string
                _tok->restoreState(state);
            }
        }
        if (valueNode == nullptr) {
            if (parentNode->isList()) {
                valueNode = parentNode->addUntypedNode();
            }
            else {
                valueNode = parentNode;
            }
        }
        if (!parseTypeCast(token)) {
            return false;
        }
        if (token.type == TOK_TYPE_OPER) {
            if (token.code == LBRACK_CODE) {
                result = parseFlowList(token, valueNode);
                _currCast = saveCast;
                return result;
            }
            else if (token.code == LBRACE_CODE) {
                result = parseFlowMap(token, valueNode);
                _currCast = saveCast;
                return result;
            }
            // Falls through to the parseScalar below...
        }
        result = parseScalar(token, valueNode);
        if (result) {
            applyTypeCast(valueNode);
        }
        _currCast = saveCast;
        return result;
    }

    bool KAMLParser::parseFlowList(Token& token, Node* parentNode) {
        bool result = false;

        if (token.type == TOK_TYPE_OPER && token.code == LBRACK_CODE) {
            parentNode->setList();
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
                result = parseFlowValue(token, parentNode);
                if (!result) {
                    break;
                }
            }
        }
        return result;
    }

    bool KAMLParser::parseFlowMap(Token& token, Node* parentNode) {
        bool result = false;

        if (token.type == TOK_TYPE_OPER && token.code == LBRACE_CODE) {
            parentNode->setMap();
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
                result = parseFlowValue(token, parentNode);
                if (!result) {
                    break;
                }
            }
        }
        return result;
    }

    bool KAMLParser::parseScalar(Token& token, Node* scalarNode) {
        bool result = false;
        if (token.type == TOK_TYPE_IDENT) {
            result = parseUnquotedString(token, scalarNode);
        }
        else if (token.type == TOK_TYPE_OPER) {
            if (token.code == VBAR_CODE) {
                result = parseFormattedString(token, scalarNode);
            }
        }
        else {
            switch (token.type) {
                case TOK_TYPE_CHAR_LIT: {
                    scalarNode->setScalar(token.getBuffer().c_str());
                    _tok->fetchToken(token);
                    result = true;
                    break;
                }
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
                case TOK_TYPE_STRING:
                case TOK_TYPE_STRING_LIT: {
                    scalarNode->setScalar(token.getBuffer().c_str());
                    _tok->fetchToken(token);
                    result = true;
                    break;
                }
            }            
        }
        return result;
    }

    /**
     * @brief parses an unquoted string into this node
     * 
     * @param token the first token of the unquoted string
     * @param node holds the unquoted string
     * @return bool 
     */
    bool KAMLParser::parseUnquotedString(Token& token, Node* scalarNode) {
        String value;
        bool   result = false;
        _tok->setGetCRLF(true);
        _tok->setGetSpaces(true);
        _tok->setGetUnknown(true);
        while (true) {
            if (token.type == TOK_TYPE_SPECIAL && (token.code == EOL_CODE || token.code == EOS_CODE)) {
                value.trim_inplace();
                scalarNode->setScalar(value.c_str());
                _tok->setGetCRLF(false);
                _tok->setGetSpaces(false);
                _tok->setGetUnknown(false);
                if (token.code == EOL_CODE) {
                    _tok->fetchToken(token);
                }
                result = true;
                break;
            }
            else if (token.type == TOK_TYPE_OPER && token.code == BACKSLASH_CODE) {
                _tok->setGetCRLF(false);
                _tok->setGetSpaces(false);
                _tok->fetchToken(token);
                _tok->setGetCRLF(true);
                _tok->setGetSpaces(true);
            }
            value.append( token.getBuffer() );
            _tok->fetchToken(token);
        }
        return result;
    }

    /**
     * @brief parses a formatted string into this node
     * 
     * @param token the token of the initial vertical bar
     * @param node holds the formatted string
     * @return bool 
     */
    bool KAMLParser::parseFormattedString(Token& token, Node* scalarNode) {
        String value;
        bool   result = false;
        bool   eolFound = false;
        while (true) {
            if (token.type == TOK_TYPE_OPER && token.code == VBAR_CODE) {
                _tok->setGetCRLF(true);
                _tok->setGetSpaces(true);
                result   = true;
                eolFound = false;
                while (true) {
                    _tok->fetchToken(token);
                    value.append( token.getBuffer() );
                    if (token.type == TOK_TYPE_SPECIAL && (token.code == EOL_CODE || token.code == EOS_CODE)) {
                        _tok->setGetCRLF(false);
                        _tok->setGetSpaces(false);
                        if (token.code == EOL_CODE) {
                            eolFound = true;
                            _tok->fetchToken(token);
                        }
                        break;
                    }
                }
            }
            else {
                if (result) {
                    scalarNode->setScalar(value.c_str());
                    if (!eolFound) {
                        _tok->fetchToken(token);
                    }
                }
                break;
            }
        }
        return result;
    }   
}