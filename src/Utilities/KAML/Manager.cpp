
#include <ctime>
#include <sstream>
#include <iostream>
#include <locale>
#include <iomanip>
#include "../DynaHashMapImpl.h"
#include "../Parser/Token.h"
#include "../Parser/Tokenizer.h"
#include "../Parser/ErrorContext.h"
#include "../DynaLogger.h"
#include "File.h"
#include "KAMLParser.h"
#include "JSONParser.h"
#include "JSONGenerator.h"
#include "Manager.h"

using namespace KAML;

MAKE_MAPTYPE_INSTANCE(String, File, File);

Manager* Manager::_instance = nullptr;
DynaLogger* logger = DynaLogger::getLogger("KAML-Mgr");

namespace KAML {

    Manager::Manager() {
        _fileMap  = new FileMap();
    }

    Manager::~Manager() {
        shutdown();
    }

    Manager* Manager::getInstance() {
        if (_instance  == nullptr) {
            _instance = new Manager();
        }
        return _instance;
    }

    void Manager::deleteInstance() {
        if (_instance != nullptr) {
            delete _instance;
            _instance = nullptr;
        }
    }

    File& Manager::loadFile(const String& fileName) {
        return getInstance()->load(fileName);
    }

    void Manager::closeAllFiles() {
        getInstance()->closeAll();
    }

    void Manager::shutdown() {
        delete _fileMap;
        _fileMap = nullptr;
    }

    File& Manager::createNewFile(const String& fileName) {
        File* file = new File();
        _fileMap->put(new String(fileName), file);
        return *file;
    }

    /**
     * @brief loads a KAML or JSON file
     * 
     * @param fileName 
     * @return File& 
     */
    File& Manager::load(const String& fileName) {
        close(fileName);
        auto*  file     = new File(fileName);
        String buf      = String();
        bool   result   = false;
        _fileMap->put(new String(fileName), file);
        if (file->read(buf) > 0) {
            auto& rootNode = file->getRootNode();
            if (fileName.rfind(".kaml") != string::npos) {
                auto parser = KAMLParser();
                result = parser.parseKaml(buf, &rootNode);
                auto* tok = parser.getTok();
                if (tok->isError()) {
                    int line = tok->getErrorContext()->errorLine;
                    logger->error("KAML file terminated prematurely at line %d: %s", line, fileName.c_str());
                }
            }
            else if (fileName.rfind(".json") != string::npos) {
                auto parser = JSONParser();
                result = parser.parseJson(buf, &rootNode);
                auto* tok = parser.getTok();
                if (tok->isError()) {
                    int line = tok->getErrorContext()->errorLine;
                    logger->error("JSON file terminated prematurely at line %d: %s", line, fileName.c_str());
                }
            }
        }
        return *file;
    }

    /**
     * @brief get an opened File based on the fileName used to open it
     * 
     * @param fileName 
     * @return File* - nullptr if not found, othewise the File object
     */
    File* Manager::findOpened(const String& fileName) {
        return _fileMap->get(fileName);
    }

    void Manager::close(const String& fileName) {
        auto* file = findOpened(fileName);
        if (file != nullptr) {
            file->close();
            _fileMap->deleteEntry(fileName);
        }
    }


    bool Manager::parseKAML(String& kamlStr, Node* rootNode) {
        auto parser = KAMLParser();
        return parser.parseKaml(kamlStr, rootNode);
    }

    bool Manager::parseJSON(String& jsonStr, Node* rootNode) {
        auto parser = JSONParser();
        return parser.parseJson(jsonStr, rootNode);
    }

    String Manager::genJSON(Node& node, bool pretty) {
        auto gen = JSONGenerator();
        return gen.generateJson(node, pretty);
    }


    void Manager::closeAll() {
        for (File* file : _fileMap->values()) {
            auto* name = file->getFileName();
            if (name != nullptr) {
                _fileMap->deleteEntry(*name);
                file->close();
            }
        }
    }

}