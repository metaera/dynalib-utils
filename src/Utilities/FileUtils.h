/**
 * @file FileUtils.h
 * @author Ken Kopelson (ken@metaera.com)
 * @brief 
 * @version 0.1
 * @date 2020-01-08
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#if !defined FILE_UTILS_H
#define FILE_UTILS_H

#include <iostream>
#include <fstream>
#include "String.h"
using namespace std;

class FileUtils {
public:
    bool static   ensureDirectory(const char* directory, String& outStr, bool includesFileName = false);
    bool static   pathExists(const char* path);
    String static joinPath(const String& path1, const String& path2);
};

#endif