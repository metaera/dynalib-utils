//
// Created by Ken Kopelson on 20/10/17.
//

#ifndef HASHCODER_H
#define HASHCODER_H

#include <string>
#include "TypeDefs.h"

namespace hashc {
    const int      HASH_SEED = 5381;               // Starting point for first cycle
    const int      HASH_MULTIPLIER = 33;           // Multiplier for each cycle
    const unsigned HASH_MASK = unsigned(-1) >> 1u; // All 1 bits except the sign

    int hashSeed();
    int hashMultiplier();
    int hashMask();

    int hashCode(bool key);
    int hashCode(char key);
    int hashCode(double key);
    int hashCode(float key);
    int hashCode(int key);
    int hashCode(long key);
    int hashCode(long long key);
    int hashCode(const char* str);
    int hashCode(const std::string& str);
    int hashCode(void* key);

    int getInitialCode();
    int getFinalCode(int code);

    template <class T> int add(int code, T value) {
        code += hashCode(value);
        code *= hashMultiplier();
        return code;
    }
}

class HashCoder {

    int _hashCode;

public:

    explicit HashCoder(int seed) : _hashCode(seed) {
    }
    
    HashCoder() : HashCoder(hashc::HASH_SEED) {
    }

    template <class T> void add(T value) {
        _hashCode += hashc::hashCode(value);
        _hashCode *= hashc::hashMultiplier();
    }

    void init(int seed) {
        _hashCode = seed;
    }

    void init() {
        init(hashc::HASH_SEED);
    }

    int getCode() {
        return hashc::getFinalCode(_hashCode);
    }
};


#endif //HASHCODER_H
