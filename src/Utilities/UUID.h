//
// Created by Ken Kopelson on 30/04/18.
//

#ifndef HR1FIRMWARE_UUID_H
#define HR1FIRMWARE_UUID_H

#include <cstdint>
#include "TypeDefs.h"
#include "String.h"
#include "crypto/ISAAC64Random.h"
#include "IComparable.h"
#include "IHashable.h"
#include "ICopyable.h"

const int UUID_SIZE = 16;

class UUID : public IHashable<UUID>, public ICopyable<UUID> {
    static Rand64Context _ctx64;
    uint64_t _mostSigBits;
    uint64_t _leastSigBits;
    int      _version = -1;

public:
    UUID() = default;
    explicit UUID(uint8_t* data);
    UUID(uint64_t mostSigBits, uint64_t leastSigBits);
    ~UUID() override = default;
    UUID(const UUID& other);
    UUID*       copy() override;
    int         hashCode() const override;
    bool operator== (const UUID& other) const override;

    static void   initRandom(char* seed);
    static const UUID randomUUID();
    static const UUID nameUUIDFromBytes(const String& name);
    static const UUID nameUUIDFromBytes(unsigned char* name, uint nameLen);
    static const UUID fromString(const String& name);
    uint64_t      getMostSignificantBits();
    uint64_t      getLeastSignificantBits();
    int           version();
    char*         format(char* buf) const;
    const String  toString() const;
};


#endif //HR1FIRMWARE_UUID_H
