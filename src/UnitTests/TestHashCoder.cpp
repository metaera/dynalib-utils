//
// Created by Ken Kopelson on 17/10/17.
//

#include <string>
#include <iostream>

#include "catch.hpp"
#include "../Utilities/HashCoder.h"

SCENARIO("HashCoder behaviour is correct") {
    GIVEN("a list of data values of various types") {
        WHEN("a hash code is generated") {
            THEN("a correct value is returned") {
                auto* coder = new HashCoder();
                coder->add(true);
                coder->add<float>(34.5);
                coder->add<double>(23.57);
                coder->add<int>(35);
                coder->add<long>(76);
                coder->add<long long>(0xFEFEFEFEFEFEFEFE);
                coder->add("Test String1");
                coder->add(std::string("Test String2"));
                coder->add<std::string>("Test String3");
                int code = coder->getCode();
                CHECK(code == 226512554);
            }
        }
    }
}
