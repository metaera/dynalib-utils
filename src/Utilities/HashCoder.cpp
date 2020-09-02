//
// Created by Ken Kopelson on 20/10/17.
//

#include "HashCoder.h"

int hashc::hashSeed() {
    return HASH_SEED;
}

int hashc::hashMultiplier() {
    return HASH_MULTIPLIER;
}

int hashc::hashMask() {
    return HASH_MASK;
}

int hashc::getInitialCode() {
    return HASH_SEED;
}

int hashc::getFinalCode(int code) {
    return (unsigned)code & HASH_MASK;
}

int hashc::hashCode(bool key) {
    return (int) key;
}

int hashc::hashCode(char key) {
    return key;
}

int hashc::hashCode(double key) {
    auto*    b    = (char*) &key;
    unsigned hash = HASH_SEED;
    for (int i = 0; i < (int) sizeof(double); i++) {
        hash = HASH_MULTIPLIER * hash + (int) *b++;
    }
    return hash & HASH_MASK;
}

int hashc::hashCode(float key) {
    auto*    b    = (char*) &key;
    unsigned hash = HASH_SEED;
    for (int i = 0; i < (int) sizeof(float); i++) {
        hash = HASH_MULTIPLIER * hash + (int) *b++;
    }
    return hash & HASH_MASK;
}

int hashc::hashCode(int key) {
    return (unsigned)key & HASH_MASK;
}

int hashc::hashCode(long key) {
    return (unsigned)key & HASH_MASK;
}

int hashc::hashCode(long long key) {
    auto*    b    = (char*) &key;
    unsigned hash = HASH_SEED;
    for (int i = 0; i < (int) sizeof(long long); i++) {
        hash = HASH_MULTIPLIER * hash + (int) *b++;
    }
    return hash & HASH_MASK;
}

int hashc::hashCode(const char *str) {
    unsigned hash = HASH_SEED;
    for (int i = 0; str && str[i] != 0; i++) {
        hash = HASH_MULTIPLIER * hash + str[i];
    }
    return int(hash & HASH_MASK);
}

int hashc::hashCode(const std::string &str) {
    unsigned hash = HASH_SEED;
    unsigned long n = str.length();
    for (int i = 0; i < n; i++) {
        hash = HASH_MULTIPLIER * hash + str[i];
    }
    return int(hash & HASH_MASK);
}

int hashc::hashCode(void *key) {
    return hashCode(reinterpret_cast<long>(key));
}
