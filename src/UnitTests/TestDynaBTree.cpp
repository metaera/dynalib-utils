//
// Created by Ken Kopelson on 14/10/17.
//

#include <iostream>

using namespace std;

#include "catch.hpp"
#include "../Utilities/DynaBTreeImpl.h"

MAKE_BTREETYPE_INSTANCE(char[25], index_t, Char25Key);

SCENARIO("raw array can be allocated and reallocated") {
    GIVEN("a newly allocated raw array") {
    }
}

