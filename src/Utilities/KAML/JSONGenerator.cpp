#include <cmath>
#include "../Utils.h"
#include "../DynaListImpl.h"
#include "../DynaHashMapImpl.h"
#include "../Parser/Token.h"
#include "../Parser/Tokenizer.h"
#include "../Parser/ErrorContext.h"
#include "../DynaLogger.h"

#include "JSONGenerator.h"
#include "../TimeUtils.h"

using namespace KAML;

DynaLogger* JSONGenerator::logger = DynaLogger::getLogger("JSON-Gen");

namespace KAML {

    String& JSONGenerator::jsonise(String& str) {
        size_t pos = 0;
        while (pos != string::npos) {
            pos = str.find_first_of('\n', pos);
            if (pos != string::npos) {
                str.replace(pos, 1, "\\n", 0, 2);
                pos += 2;
            }
        }
        return str;
    }

    String& JSONGenerator::unjsonise(String& str) {
        size_t pos = 0;
        while (pos != string::npos) {
            pos = str.find("\\n", pos);
            if (pos != string::npos) {
                str.replace(pos, 2, "\n", 0, 1);
                ++pos;
            }
        }
        return str;
    }

    JSONGenerator::JSONGenerator() {
    }

    JSONGenerator::~JSONGenerator() {        
    }

    void JSONGenerator::_addIndent(String& str) {
        str += "\n";
        if (_indentLevel > 0) {
            int spaces = _indentLevel * 2;
            for (int i = 0; i < spaces; ++i) {
                str += " ";
            }
        }
    }

    String JSONGenerator::generateJson(Node& rootNode, bool pretty, int docSelect) {
        String outStr;
        _pretty = pretty;
        _indentLevel = 0;
        Node* start = &rootNode;
        if (rootNode.isList() && docSelect >= 0) {
            auto* list = rootNode.value().listValue;
            if (docSelect < list->count()) {
                start = &(rootNode[docSelect]);
            }
            else {
                return outStr;
            }
        }
        auto result = generateValue(*start, outStr);
        return outStr;
    }

    bool JSONGenerator::generateValue(Node& node, String& outStr) {
        if (node.isList()) {
            outStr += "[";
            if (_pretty) {
                ++_indentLevel;
                _addIndent(outStr);
            }
            bool first = true;
            for (Node* child : *(node.value().listValue)) {
                if (!first) {
                    outStr += ",";
                    if (_pretty) {
                        _addIndent(outStr);
                    }
                }
                auto result = generateValue(*child, outStr);
                first = false;
            }
            if (_pretty) {
                --_indentLevel;
                _addIndent(outStr);
            }
            outStr += "]";
        }
        else if (node.isMap()) {
            outStr += "{";
            if (_pretty) {
                ++_indentLevel;
                _addIndent(outStr);
            }
            auto map = node.value().mapValue;
            bool first = true;
            for (String* key : map->keys()) {
                if (!first) {
                    outStr += ",";
                    if (_pretty) {
                        _addIndent(outStr);
                    }
                }
                Node* child = (*map)[key];
                outStr += "\"";
                outStr += (*key);
                outStr += "\":";
                if (_pretty) {
                    outStr += " ";
                }
                auto result = generateValue(*child, outStr);
                first = false;
            }
            if (_pretty) {
                --_indentLevel;
                _addIndent(outStr);
            }
            outStr += "}";
        }
        else if (node.isNullPtr()) {
            outStr += node.asNullPtr();
        }
        else if (node.isScalar()) {
            auto result = generateScalar(node, outStr);
        }
        return true;
    }

    bool JSONGenerator::generateTypeObject(Node& node, String& outStr, const String& type, const String& value) {
        outStr += "{";
        if (_pretty) {
            ++_indentLevel;
            _addIndent(outStr);
        }
        outStr += "\"type\":";
        if (_pretty) {
            outStr += " ";
        }
        outStr += "\"";
        outStr += type;
        outStr += "\",";
        if (_pretty) {
            _addIndent(outStr);
        }
        outStr += "\"value\":";
        if (_pretty) {
            outStr += " ";
        }
        outStr += "\"";
        outStr += value;
        outStr += "\"";
        if (_pretty) {
            --_indentLevel;
            _addIndent(outStr);
        }
        outStr += "}";
        return true;
    }

    bool JSONGenerator::generateScalar(Node& node, String& outStr) {
        switch (node.scalarType()) {
            case ScalarType::NONE: {
                break;
            }
            case ScalarType::BOOL: {
                outStr += node.asString();
                break;
            }
            case ScalarType::INT: {
                outStr += tostr(node.asInt());
                break;
            }
            case ScalarType::FLOAT: {
                auto val = node.asFloat();
                if (val == INFINITY || val == -INFINITY || val == NAN) {
                    generateTypeObject(node, outStr, "float", node.asString());
                }
                else {
                    outStr += tostr(node.asFloat());
                }
                break;
            }
            case ScalarType::STRING: {
                outStr += "\"";
                String str = node.asString();
                outStr += jsonise(str);
                outStr += "\"";
                break;
            }
            case ScalarType::SEQ: {
                String str  = node.asString();
                generateTypeObject(node, outStr, "seq", jsonise(str));
                break;
            }
            case ScalarType::PROPER: {
                String str  = node.asString();
                generateTypeObject(node, outStr, "proper", jsonise(str));
                break;
            }
            case ScalarType::TIME: {
                generateTypeObject(node, outStr, "time", node.asString());
                break;
            }
            case ScalarType::TIMESTAMP: {
                generateTypeObject(node, outStr, "ts", node.asString());
                break;
            }
            case ScalarType::DATE: {
                generateTypeObject(node, outStr, "date", node.asString());
                break;
            }
        }
        return true;
    }
}