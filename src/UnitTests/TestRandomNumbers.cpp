//
// Created by Ken Kopelson on 27/04/18.
//

#include <string>
#include <iostream>

using namespace std;

#include "catch.hpp"
#include "Utilities/crypto/ISAACRandom.h"
#include "Utilities/crypto/ISAAC64Random.h"

SCENARIO("Random Number generator functions properly") {

    GIVEN("a newly initialised 32-bit RNG") {

        RandContext ctx = {.randcnt=0, .randa=0, .randb=0, .randc=0};

        isaacSeed((char *)"This is a seed list", &ctx);
        isaacInit(true, &ctx);

        WHEN("we cycle through the numbers returned") {

            THEN("the values are all different") {
                for (int i = 0; i < 100000; ++i) {
                    ub4 value = randInt(&ctx);
//                    cout << "Value: " << value << endl;
                }
            }
        }
    }

    GIVEN("a newly initialised 64-bit RNG") {

        Rand64Context ctx64 = {.randcnt=0, .randa=0, .randb=0, .randc=0};
        isaac64Seed((char *)"This is a seed list", &ctx64);
        isaac64Init(true, &ctx64);

        WHEN("we cycle through the numbers returned") {

            THEN("the values are all different") {
                for (int i = 0; i < 100000; ++i) {
                    ub8 value = rand64Int(&ctx64);
//                    cout << "Value: " << value << endl;
                }
            }
        }

        WHEN("we cycle through the crypto byte sequences") {

            THEN("the values are all different") {
                auto* buf = new uint8_t[32];
                for (int i = 0; i < 1000; ++i) {
                    rand64Bytes(&ctx64, buf, 32);
//                    cout << "Value: " << value << endl;
                }
                delete[] buf;
            }
        }
    }
}

