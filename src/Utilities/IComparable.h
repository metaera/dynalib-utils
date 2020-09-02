//
// Created by Ken Kopelson on 21/10/17.
//

#ifndef ICOMPARABLE_H
#define ICOMPARABLE_H

template <class T> class IComparable {
public:
    IComparable() = default;
    virtual ~IComparable() = default;
    virtual bool operator== (const T& other) const = 0;
};

#endif //ICOMPARABLE_H
