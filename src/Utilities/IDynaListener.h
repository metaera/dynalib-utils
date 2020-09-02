//
// Created by Ken Kopelson on 26/03/18.
//

#ifndef IDYNALISTENER_H
#define IDYNALISTENER_H

#include "IDynaEvent.h"

class IDynaListener {
public:
    IDynaListener() = default;
    virtual ~IDynaListener() = default;
    virtual void handeEvent(IDynaEvent* event) = 0;
};

#endif //IDYNALISTENER_H
