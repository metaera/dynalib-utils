//
// Created by Ken Kopelson on 20/10/17.
//

#ifndef ICOPYABLE_H
#define ICOPYABLE_H

template <typename T> class ICopyable {
public:
    ICopyable() = default;
    virtual ~ICopyable() = default;
    virtual T* copy() = 0;
};

#endif //ICOPYABLE_H
