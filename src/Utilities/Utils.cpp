//
// Created by Ken Kopelson on 8/10/17.
//

#include "Utils.h"
#include <locale>
#include <algorithm>
#include "String.h"
#include "Exception.h"

char uToL[256];
char lToU[256];
bool mapsInitFlag = false;

string trim(const string &s)
{
   auto wsfront=find_if_not(s.begin(),s.end(),[](int c) { return isspace(c);});
   auto wsback=find_if_not(s.rbegin(),s.rend(),[](int c) { return isspace(c);}).base();
   return (wsback<=wsfront ? string() : string(wsfront,wsback));
}

void initConvertMaps() {
    for (int i = 0, len = sizeof(uToL); i < len; ++i) {
        if (isupper(i)) {
            uToL[i]      = (char)(i + 32);
            lToU[i]      = (char)i;
            lToU[i + 32] = (char)i;
        }
        else if (i >= 32 && isupper(i - 32)){
            uToL[i] = (char)i;
        }
        else {
            uToL[i] = (char)i;
            lToU[i] = (char)i;
        }
    }
    mapsInitFlag = true;
}

String* toLower(String* str) {
    if (!mapsInitFlag)
        initConvertMaps();
    auto str2 = new String();
    for (unsigned long i = 0, len = str->length(); i < len; ++i) {
        str2->append(1, uToL[(*str)[i]]);
    }
    return str2;
}

String* toUpper(String* str) {
    if (!mapsInitFlag)
        initConvertMaps();
    auto str2 = new String();
    for (unsigned long i = 0, len = str->length(); i < len; ++i) {
        str2->append(1, lToU[(*str)[i]]);
    }
    return str2;
}

String& toLower(String& str) {
    if (!mapsInitFlag)
        initConvertMaps();
    for (unsigned long i = 0, len = str.length(); i < len; ++i) {
        str.replace(i, 1, 1, uToL[str[i]]);
    }
    return str;
}

String& toUpper(String& str) {
    if (!mapsInitFlag)
        initConvertMaps();
    for (unsigned long i = 0, len = str.length(); i < len; ++i) {
        str.replace(i, 1, 1, lToU[str[i]]);
    }
    return str;
}

String& toLower(const String& str) {
    if (!mapsInitFlag)
        initConvertMaps();
    auto str2 = new String();
    for (char i : str) {
        str2->append(1, uToL[i]);
    }
    return *str2;
}

String& toUpper(const String& str) {
    if (!mapsInitFlag)
        initConvertMaps();
    auto str2 = new String();
    for (char i : str) {
        str2->append(1, lToU[i]);
    }
    return *str2;
}

String toLowerStack(const String& str) {
    if (!mapsInitFlag)
        initConvertMaps();
    String str2;
    for (char i : str) {
        str2.append(1, uToL[i]);
    }
    return String(str2);
}

String toUpperStack(const String& str) {
    if (!mapsInitFlag)
        initConvertMaps();
    String str2;
    for (char i : str) {
        str2.append(1, lToU[i]);
    }
    return String(str2);
}

char* toLower(char* str, int size) {
    if (!mapsInitFlag)
        initConvertMaps();
    for (unsigned long i = 0; i < size; ++i) {
        str[i] = uToL[str[i]];
    }
    return str;
}

char* toUpper(char* str, int size) {
    if (!mapsInitFlag)
        initConvertMaps();
    for (unsigned long i = 0; i < size; ++i) {
        str[i] = lToU[str[i]];
    }
    return str;
}

char toLower(char ch) {
    if (!mapsInitFlag)
        initConvertMaps();
    return uToL[ch];
}

char toUpper(char ch) {
    if (!mapsInitFlag)
        initConvertMaps();
    return lToU[ch];
}

const char HEX_CHARS[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

char* uint64ToHexChars(uint64_t inValue, char* outBuf, int outOffset) {
    return byteArrayToHexChars((const uint8_t*)(&inValue), (int)((inValue - 1L) / 0xFF) + 1, outBuf, outOffset);
}

char* uint64ToHexChars(uint64_t inValue, int inSize, char* outBuf, int outOffset) {
    return byteArrayToHexChars((const uint8_t*)(&inValue), inSize, outBuf, outOffset);
}

char* byteArrayToHexChars(const uint8_t* inData, int inSize, char* outBuf, int outOffset) {
    int x = outOffset;
    for (int i = inSize - 1; i >= 0; --i) {
        uint8_t b   = inData[i];
        outBuf[x++] = HEX_CHARS[(int)(b & 0xF0u) >> 4u];
        outBuf[x++] = HEX_CHARS[(int)(b & 0x0Fu)];
    }
    outBuf[x] = '\0';
    return outBuf;
}

String byteArrayToHexString(const uint8_t* inData, int inSize) {
    char buf[(inSize - 1) * 2 + 1];
    byteArrayToHexChars(inData, inSize, buf, 0);
    return String(buf);
}
