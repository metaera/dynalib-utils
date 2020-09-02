#ifndef JSON_GENERATOR_H
#define JSON_GENERATOR_H

#include "../String.h"
#include "../DynaList.h"
#include "../IntWrapper.h"
#include "../Parser/Tokenizer.h"
#include "../Parser/Token.h"
#include "../Graph/Node.h"

using namespace Graph;
class DynaLogger;

namespace KAML {

    class JSONGenerator {
        static DynaLogger* logger;
        bool _pretty;
        int  _indentLevel;

    private:
        void _addIndent(String& str);
    public:
        static String& jsonise(String& str);
        static String& unjsonise(String& str);

        JSONGenerator();
        virtual ~JSONGenerator();

        bool   isPretty();
        String generateJson(Node& rootNode, bool pretty = false, int docSelect = -1);
        bool   generateValue(Node& node, String& outStr);
        bool   generateTypeObject(Node& node, String& outStr, const String& type, const String& value);
        bool   generateScalar(Node& node, String& outStr);
    };
}

#endif