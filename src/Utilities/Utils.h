//
// Created by Ken Kopelson on 8/10/17.
//

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include "DynaList.h"

extern char uToL[256];
extern char lToU[256];
extern bool mapsInitFlag;

class String;

extern string  trim(const string &s);
extern void    initConvertMaps();
extern String* toLower(String* str);        // Returns a new String
extern String* toUpper(String* str);        // Returns a new String
extern String& toLower(const String& str);  // Returns a new String
extern String& toUpper(const String& str);  // Returns a new String
extern String  toLowerStack(const String& str);  // Returns a String copy on caller's stack
extern String  toUpperStack(const String& str);  // Returns a String copy on caller's stack
extern String& toLower(String& str);        // Changes the String passed in
extern String& toUpper(String& str);        // Changes the String passed in
extern char*   toLower(char* str);
extern char*   toUpper(char* str);
extern char    toLower(char str);
extern char    toUpper(char str);

template <typename T> string tostr(const T& t) { 
   ostringstream os; 
   os<<t; 
   return os.str(); 
} 

extern char*   uint64ToHexChars(uint64_t inValue, char* outBuf, int outOffset);
extern char*   uint64ToHexChars(uint64_t inValue, int inSize, char* outBuf, int outOffset);
extern char*   byteArrayToHexChars(const uint8_t* inData, int inSize, char* outBuf, int outOffset);
extern String  byteArrayToHexString(const uint8_t* inData, int inSize);

// #ifndef min
// #define min(x,y) ((x)<=(y)?(x):(y))
// #endif
// #ifndef max
// #define max(x,y) ((x)>=(y)?(x):(y))
// #endif
//#ifndef abs
//#define abs(a)   (((a)>=0)?(a):-(a))
//#endif

#endif //UTILS_H
