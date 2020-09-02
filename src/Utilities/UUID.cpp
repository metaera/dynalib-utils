//
// Created by Ken Kopelson on 30/04/18.
//

#include <vector>
#include "String.h"
#include "UUID.h"
#include "Utils.h"
#include "Exception.h"
#include "crypto/SHA256.h"

Rand64Context UUID::_ctx64 = {.randcnt=0, .randa=0, .randb=0, .randc=0};

UUID::UUID(uint8_t* data) {
    uint64_t msb = 0;
    uint64_t lsb = 0;
    for (int i = 0; i < 8; ++i)
        msb = (msb << 8u) | (uint64_t)(data[i] & (uint8_t)0xFFu);
    for (int i = 8; i < 16; ++i)
        lsb = (lsb << 8u) | (uint64_t)(data[i] & (uint8_t)0xFFu);
    _mostSigBits  = msb;
    _leastSigBits = lsb;
}

UUID::UUID(uint64_t mostSigBits, uint64_t leastSigBits) {
    _mostSigBits  = mostSigBits;
    _leastSigBits = leastSigBits;
}

UUID::UUID(const UUID& other) {
    _mostSigBits  = other._mostSigBits;
    _leastSigBits = other._leastSigBits;
    _version      = other._version;
}

UUID* UUID::copy() {
    return new UUID(*this);
}

int UUID::hashCode() const {
    auto code = HashCoder();
    code.add((long long)_mostSigBits);
    code.add((long long)_leastSigBits);
    return code.getCode();
}
bool UUID::operator== (const UUID& other) const {
    return _mostSigBits == other._mostSigBits && _leastSigBits == other._leastSigBits;
}

void UUID::initRandom(char* seed) {
    isaac64Seed(seed, &_ctx64);
    isaac64Init(true, &_ctx64);
}

const UUID UUID::randomUUID() {
    if (_ctx64.randcnt == 0)
        initRandom((char*)"default UUID seed");

    uint8_t randomBytes[16] {};
    rand64Bytes(&_ctx64, randomBytes, 16);
    randomBytes[6] &= 0x0Fu;
    randomBytes[6] |= 0x40u;
    randomBytes[8] &= 0x3Fu;
    randomBytes[8] |= 0x80u;
    return UUID(randomBytes);
}

const UUID UUID::nameUUIDFromBytes(const String& name) {
    return nameUUIDFromBytes((unsigned char*)name.c_str(), (uint)name.length());
}

const UUID UUID::nameUUIDFromBytes(unsigned char* name, uint nameLen) {
    uint8_t digest[SHA256::DIGEST_SIZE] {};
    sha256(name, nameLen, digest);
    digest[6] &= 0x0Fu;
    digest[6] |= 0x30u;
    digest[8] &= 0x3Fu;
    digest[8] |= 0x80u;
    // Only the first 128-bits of the digest will be used to create the UUID...
    return UUID(digest);
}

const UUID UUID::fromString(const String& name) {
    std::vector<std::string> components = name.split("-");
    if (components.size() != 5)
        throw IllegalArgumentException("Invalid UUID string: " + name);

    uint64_t mostSigBits = std::stoul(components[0], nullptr, 16);
    mostSigBits <<= 16;
    mostSigBits |= std::stoul(components[1], nullptr, 16);
    mostSigBits <<= 16;
    mostSigBits |= std::stoul(components[2], nullptr, 16);

    uint64_t leastSigBits = std::stoul(components[3], nullptr, 16);
    leastSigBits <<= 48;
    leastSigBits |= std::stoul(components[4], nullptr, 16);

    return UUID(mostSigBits, leastSigBits);
}

uint64_t UUID::getMostSignificantBits() {
    return _mostSigBits;
}

uint64_t UUID::getLeastSignificantBits() {
    return _leastSigBits;
}

int UUID::version() {
    if (_version < 0)
        _version = (int)((_mostSigBits >> 12u) & 0x0Fu);
    return _version;
}

/**
 * Format UUID object into standard UUID string format XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
 * @param buf - must be 37 bytes to avoid overrun
 * @return
 */
char* UUID::format(char* buf) const {
    uint64ToHexChars(_mostSigBits >> 32u,  4,  buf, 0);
    buf[8]  = '-';
    uint64ToHexChars(_mostSigBits >> 16u,  2,  buf, 9);
    buf[13] = '-';
    uint64ToHexChars(_mostSigBits,         2,  buf, 14);
    buf[18] = '-';
    uint64ToHexChars(_leastSigBits >> 48u, 2,  buf, 19);
    buf[23] = '-';
    uint64ToHexChars(_leastSigBits,        6,  buf, 24);
    return buf;
}

const String UUID::toString() const {
    char buf[37];
    format(buf);
    return String(buf);
}
