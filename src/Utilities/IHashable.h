//
// Created by Ken Kopelson on 21/10/17.
//

#if !defined IHASHABLE_H
#define IHASHABLE_H

#include "IComparable.h"

template <class T> class IHashable : public IComparable<T> {
public:
    IHashable() = default;
    virtual ~IHashable() = default;
    virtual int hashCode() const = 0;
};

#endif //IHASHABLE_H
