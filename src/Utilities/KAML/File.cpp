#include "../DynaListImpl.h"
#include <cstddef>
#include <unistd.h>
#include "File.h"

using namespace KAML;

MAKE_LISTTYPE_INSTANCE(File, File);

namespace KAML {

    void File::_init(const String& fileName) {
        if (!exists(fileName.c_str()))
            throw FileNotFoundException("File not found: " + fileName);
        if (!canAccess(fileName))
            throw DataFileException("File not accessible: " + fileName);
        open(fileName, "r");
        _rootNode = new Node(NodeType::LIST);
        _fileName = new String(fileName);
    }


    File::File() {
        _rootNode = new Node(NodeType::UNTYPED);
        _rootNode->setList();
        _fileName = nullptr;
    }

    /**
     * @brief Construct a new File:: File object
     * 
     * @param fileName 
     * @throw FileNotFound exception if file is not found
     * @throw DataFileException is the file is not accessible
     */
    File::File(const String& fileName) : _file(nullptr) {
        _init(fileName);
    }

    File::~File() {
        close();
        delete _rootNode;
        _rootNode = nullptr;
        delete _fileName;
        _fileName = nullptr;
    }

    File::File(const File& other) {
        _init(*(other.getFileName()));
    }

    File* File::copy() {
        return new File(*this);
    }

    bool File::exists(const String& fileName) {
        return access(fileName.c_str(), F_OK) == 0;
    }

    bool File::canAccess(const String& fileName) {
        return access(fileName.c_str(), R_OK | W_OK) == 0;
    }

    bool File::isOpen() {
        return _file != nullptr;
    }

    bool File::open(const String& fileName, const char* mode) {
        close();
        _file = fopen(fileName.c_str(), mode);
        return isOpen();
    }

    const String* File::getFileName() const {
        return _fileName;
    }

    size_t File::read(String& buf) {
        // Determine file size
        fseek(_file, 0, SEEK_END);
        size_t size = ftell(_file);
        if (size > 0) {
            char* charBuf = new char[size + 1];
            rewind(_file);
            size = fread(charBuf, sizeof(char), size, _file);
            charBuf[size] = '\0';
            buf.append(String(charBuf));
            delete[] charBuf;
        }
        return size;
    }

    void File::close() {
        if (isOpen()) {
            fclose(_file);
            _file  = nullptr;
        }
    }

    bool File::save() {
        return false;
    }

    bool File::saveAs(const String& fileName) {
        return false;
    }

    // void File::setRootNode(Node* rootNode) {
    //     _rootNode = rootNode;
    // }

    Node& File::getRootNode() {
        return *_rootNode;
    }

    Node& File::getDoc(int index) {
        return (*_rootNode)[index];
    }

    int File::hashCode() const {
        auto code = HashCoder();
        code.add(_fileName);
        return code.getCode();
    }

    bool File::operator== (const File &other) const {
        return _fileName == other.getFileName();
    }

}