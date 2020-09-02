//
// Created by Ken Kopelson on 25/10/17.
//

#ifndef SECURITYTOKEN_H
#define SECURITYTOKEN_H

#include <cstdint>
#include "BitManip.h"

#define ACTION_SET_SECURE_BIT       (0x01 << 0)
#define ACTION_CLEAR_SECURE_BIT     (0x01 << 1)

/**
 * @brief This class will store a digitally-signed token, where it contains identifying information that
 * matches the logon credentials of the user. If the token has not expired, and the information matches, the
 * user is granted the permissions encoded in the signed token.  This class holds the information and
 * provides the basic methods for checking it.
 * 
 * The main actions secured by this token allow the user set/clear the flags on the particular Thing, which
 * would allow various options to be adjusted, particularly for accessing the node tree below the secured
 * node.
 * 
 */
class SecurityToken {
    uint32_t _permissions = 0xFFFFFFFF;
public:
    SecurityToken() = default;
    virtual ~SecurityToken() = default;
    bool isValid();
    bool isPermitted(uint32_t actions);
};


#endif //ROBOCHEFEMULATOR_SECURITYTOKEN_H
