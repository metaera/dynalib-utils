//
// Created by Ken Kopelson on 25/10/17.
//

#include "Utilities/SecurityToken.h"

bool SecurityToken::isValid() {
    // TODO: add code to check for valid signed token
    return true;
}
bool SecurityToken::isPermitted(uint32_t actions) {
    return IS_ALL_SET(_permissions, actions);
}
