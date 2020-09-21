//
// Created by Max Barrass on 20/09/2020.
//

#include <iostream>
#include <cstring>

using namespace std;

#include "catch.hpp"
#include "../Utilities/DynaBuffer.h"


SCENARIO("DynaBuffer operations function properly") {
    GIVEN("a newly allocated buffer array") {
        auto* buffer = new DynaBuffer(10, true);

        REQUIRE(buffer->getBufferSize() == 10);
        
    }
    GIVEN("a newly allocated buffer empty") {
        DynaBuffer* buffer = new DynaBuffer();

        REQUIRE(buffer->getBufferSize() == 0);
        
    }
    GIVEN("a newly allocated buffer can be set") {
        DynaBuffer* buffer = new DynaBuffer(10);
        for (int i = 0; i < buffer->getBufferSize(); ++i) {
            buffer->putByte(1);
        }

        cout << "AA" << buffer->getInternalTypedArrayAtPos(0) << endl;
        // REQUIRE(buffer->getInternalTypedArrayAtPos() == 0);

        REQUIRE(buffer->getBufferSize() == 10);

        WHEN("we fill it up with bytes of value 1") {
            //is full
            REQUIRE(buffer->isFull());
            //is not empty
            REQUIRE(!buffer->isEmpty());
            THEN("the count is correct") {                
                AND_THEN("all the byte values are 1") {
                    buffer->setPos(0);
                    for (int i = 0; i < 10; ++i) {
                        REQUIRE(buffer->getNextByte() == 1);
                    }
                }
            }
        }

        WHEN("the buffer is cleared") {
            buffer->clear();
            THEN("then it should be dirty and empty") {
                REQUIRE(buffer->isEmpty());
                REQUIRE(buffer->isDirty());
            }
        }
    }
    GIVEN("a newly allocated buffer can be set beyond its size") {
        DynaBuffer* buffer = new DynaBuffer(10);
        for (int i = 0; i < buffer->getBufferSize(); ++i) {
            buffer->putByte(1);
        }

        REQUIRE(buffer->getBufferSize() == 10);
        
        WHEN("buffer is full") {
            REQUIRE(buffer->isFull());
            THEN("adding another item throws a BufferOverrunException") {
                CHECK_THROWS_AS(buffer->putByte(1), BufferOverrunException);
            }
        }
        
    }
}


