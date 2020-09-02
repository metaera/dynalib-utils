#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "Utils.h"
#include "FileUtils.h"
#include "Synchronize.h"

bool FileUtils::ensureDirectory(const char* directory, String& outStr, bool includesFileName) {
    string tokenStr;
    char* dupDir = strdup((char*)directory);
    char* token;
    char* dirPtr = dupDir;
    struct stat st;

    finally1(dupDir) {
        delete dupDir;
    }
    end_finally

    outStr.clear();
    bool startAtRoot = directory[0] == '/';

    while ((token = strtok_r(dirPtr, "/", &dirPtr)) != nullptr)
    {
        tokenStr = trim(token);
        if (tokenStr.length() > 0) {
            if (startAtRoot || outStr.size() > 0) {
                outStr += "/";
            }
            startAtRoot = false;
            outStr += tokenStr;
            if (stat(outStr.c_str(), &st) == 0) {
                if (!S_ISDIR(st.st_mode)) {
                    return includesFileName;
                }
            }
            else if (mkdir(outStr.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH ) != 0) {
                return false;
            }
        }
    }
    return true;
}

bool FileUtils::pathExists(const char* path) {
    return access(path, F_OK) >= 0;
}

String FileUtils::joinPath(const String& path1, const String& path2) {
    String outStr = path1;
    if (outStr.at(outStr.length() - 1) != '/') {
        outStr += "/";        
    }
    outStr += path2;
    return outStr;
}


