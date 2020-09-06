//
// Created by Ken Kopelson on 14/10/17.
//

#include <iostream>

using namespace std;

#include "catch.hpp"
#include "../Utilities/DynaArrayImpl.h"

MAKE_ARRAYTYPE_INSTANCE(long, Long);

SCENARIO("raw array can be allocated and reallocated") {
    GIVEN("a newly allocated raw array") {
        auto* array = LongAllocArray::newArray(2000);

        REQUIRE(array != nullptr);

        WHEN("the array is reallocated to a new size") {
            array = LongAllocArray::reallocArray(array, 2000, 2500);
            THEN("the array is not a nullptr") {
                REQUIRE(array != nullptr);
            }
        }
    }
}

SCENARIO("all contents of raw array are preserved when reallocating") {
    GIVEN("a newly allocated raw array with items written into it") {
        auto* array = LongAllocArray::newArray(2000);
        for (int i = 0; i < 2000; ++i) {
            array[i] = i;
        }
        WHEN("the array is made larger") {
            array = LongAllocArray::reallocArray(array, 2000, 2500);
            THEN("all items are still in the resulting array") {
                int i = 0;
                for (; i < 2000; ++i) {
                    REQUIRE(array[i] == i);
                }
                for (; i < 2500; ++i) {
                    REQUIRE(array[i] == 0);
                }
            }
        }
        WHEN("the array is made smaller and then larger again") {
            array = LongAllocArray::reallocArray(array, 2000, 500);
            array = LongAllocArray::reallocArray(array, 500, 1000);

            THEN("the array is truncated and totally filled with items") {
                int i = 0;
                for (; i < 500; ++i) {
                    REQUIRE(array[i] == i);
                }
                for (; i < 1000; ++i) {
                    REQUIRE(array[i] == 0);
                }
            }
        }
        delete array;
    }
}

SCENARIO("DynaArray operations function properly") {
    GIVEN("a newly created DynaArray item of type 'long'") {
        auto *dArray = new LongArray();     // Starts off empty

        REQUIRE(dArray->isEmpty());   // count = 0
        REQUIRE(dArray->isFull());    // count = 0, capacity = 0

        WHEN("inserting 10 new items at index 0") {
            for (long i = 0; i < 10; ++i) {
                dArray->insert(0, i);
            }
            THEN("all values are in the array in reverse order") {
                long j = 9;
                for (uint i = 0; i < 10; ++i, --j) {
                    REQUIRE((*dArray)[i] == j);
                }
            }
        }
        WHEN("deleting 5 items at index 0") {
            THEN("") {

            }
        }
        WHEN("moving items") {
            THEN("") {

            }
        }
    }
}
