#ifndef KAML_MANAGER_H
#define KAML_MANAGER_H

#include "../String.h"
#include "../DynaHashMap.h"
#include "../IntWrapper.h"
#include "../Parser/Tokenizer.h"
#include "../Parser/Token.h"
#include "File.h"
#include "../Graph/Node.h"

using namespace Graph;

class DynaLogger;
class KAMLParser;
class JSONParser;
class JSONGenerator;

namespace KAML {

    MAKE_MAPTYPE_DEF(String, File, File);

    class Manager {
    private:
        static Manager* _instance;
        FileMap* _fileMap = nullptr;

        Manager();

    public:
        static Manager* getInstance();
        static void     deleteInstance();
        static File&    loadFile(const String& fileName);
        static void     closeAllFiles();

        virtual ~Manager();

        void   shutdown();
        File&  createNewFile(const String& fileName);
        File&  load(const String& fileName);
        File*  findOpened(const String& fileName);
        void   close(const String& fileName);
        bool   parseKAML(String& kamlStr, Node* rootNode);
        bool   parseJSON(String& jsonStr, Node* rootNode);
        String genJSON(Node& node, bool pretty = false);
        void   closeAll();
    };
}

#endif