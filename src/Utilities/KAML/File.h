#ifndef KAML_FILE_H
#define KAML_FILE_H

#include <cstdio>
#include "../String.h"
#include "../DynaList.h"
#include "../IntWrapper.h"
#include "../Graph/Node.h"

using namespace Graph;

namespace KAML {

    class File : public IHashable<File>, public ICopyable<File> {
        FILE*   _file = nullptr;
        Node*   _rootNode = nullptr;
        const String* _fileName = nullptr;

        void _init(const String& fileName);

    public:
        File();
        explicit File(const String& fileName);
        virtual ~File();
        File(const File& other);
        File* copy() override;

        static bool exists(const String& fileName);
        static bool canAccess(const String& fileName);

        bool    isOpen();
        bool    open(const String& fileName, const char* mode);
        const String* getFileName() const;
        size_t  read(String& buf);
        void    close();
        bool    save();
        bool    saveAs(const String& fileName);

        // void    setRootNode(Node* rootNode);
        Node&   getRootNode();
        Node&   getDoc(int index = 0);

        int hashCode() const override;
        bool operator== (const File &other) const override;
    };

}

#endif