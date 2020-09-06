//
// Created by Ken Kopelson on 27/04/18.
//

#include <string>
#include <iostream>

using namespace std;

#include "catch.hpp"
#include "Utilities/UUID.h"

SCENARIO("UUID generation functions properly") {

    GIVEN("a proper buffer to hold UUID values") {
        char buf[37] = {};

        WHEN("we create many random UUID values") {
            THEN("the values are all different") {
                for (int i = 0; i < 100; ++i) {
                    UUID uuid = UUID::randomUUID();
                    uuid.format(buf);
                    auto uuidStr = String(buf);
//                    cout << uuidStr << endl;
                }
            }
        }

        WHEN("we create many name UUID values") {
            THEN("the values are all different") {
                int suffix = 0;
                for (int i = 0; i < 10; ++i) {
                    UUID uuid = UUID::nameUUIDFromBytes("Test Name " + to_string(suffix));
                    suffix = suffix == 0 ? 1 : 0;
                    uuid.format(buf);
                    auto uuidStr = String(buf);
                    cout << uuidStr << endl;
                }
            }
        }

        WHEN("we create a single random UUID") {
            UUID uuid = UUID::randomUUID();
            AND_WHEN("we extract the string representation and create a new UUID from it") {
                String str1 = uuid.toString();
                UUID uuid2  = UUID::fromString(str1);
                THEN("the new UUID matches the original UUID") {
                    String str2 = uuid2.toString();
                    CHECK((uuid == uuid2));
                }
            }
        }
    }
}


