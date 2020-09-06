//
// Created by Ken Kopelson on 19/10/17.
//
#include <iostream>
#include <cstring>

//using namespace std;

#include "catch.hpp"
#include "../Utilities/DynaRingBuffer.h"
#include "../Utilities/Exception.h"

#define BYTE_COUNT      100
#define USHORT_COUNT    (BYTE_COUNT / 2)
#define ULONG_COUNT     (BYTE_COUNT / 4)
#define STRING_COUNT    (BYTE_COUNT / 5)
#define LONG_STR_COUNT  (STRING_COUNT / 4)

SCENARIO("DynaRingBuffer operations function properly") {
    GIVEN("a newly allocated ring buffer of a max. size") {
        auto* ringBuf = new DynaRingBuffer(BYTE_COUNT);

        WHEN("we fill it up with bytes of value 0x55") {
            for (int i = 0; i < BYTE_COUNT; ++i)
                ringBuf->putByte(0x55);
            THEN("the count is correct") {
                CHECK(ringBuf->getCount() == BYTE_COUNT);
            }
            THEN("all the byte values are 0x55") {
                for (int i = 0; i < BYTE_COUNT; ++i) {
                    CHECK(ringBuf->getNextByte() == (uint8_t)0x55);
                }
                CHECK(ringBuf->isEmpty());
            }
        }
        WHEN("we fill it up with ushorts of value 0x2525") {
            for (int i = 0; i < USHORT_COUNT; ++i)
                ringBuf->putUShort(0x2525);
            THEN("the count is correct") {
                CHECK(ringBuf->getCount() == BYTE_COUNT);
            }
            THEN("all the ushort values are 0x2525") {
                for (int i = 0; i < USHORT_COUNT; ++i) {
                    CHECK(ringBuf->getNextUShort() == (ushort)0x2525);
                }
                CHECK(ringBuf->isEmpty());
            }
        }
        WHEN("we fill it up with ulongs of value 0x35353535") {
            for (int i = 0; i < ULONG_COUNT; ++i)
                ringBuf->putULong(0x35353535);
            THEN("the count is correct") {
                CHECK(ringBuf->getCount() == BYTE_COUNT);
            }
            THEN("all the ulong values are 0x35353535") {
                for (int i = 0; i < ULONG_COUNT; ++i) {
                    CHECK(ringBuf->getNextULong() == (ulong)0x35353535);
                }
                CHECK(ringBuf->isEmpty());
            }
        }
        WHEN("we fill it up with strings (char *) of value 'TEST' followed by null-byte") {
            for (int i = 0; i < STRING_COUNT; ++i)
                ringBuf->putString("TEST");
            THEN("the count is correct") {
                CHECK(ringBuf->getCount() == BYTE_COUNT);
            }
            THEN("all the string values are 'TEST'") {
                char buf[5];
                for (int i = 0; i < STRING_COUNT; ++i) {
                    ringBuf->getNextString(buf, 5);
                    CHECK(strcmp(buf, "TEST") == 0);
                }
                CHECK(ringBuf->isEmpty());
            }
        }
        WHEN("we fill it up with strings (char *) of different sizes, following each by null-byte") {
            for (int i = 0; i < LONG_STR_COUNT; ++i) {
                ringBuf->putString("TEST");
                ringBuf->putString("LONG_TEST");
                ringBuf->putString("TEST");
            }
            THEN("the count is correct") {
                CHECK(ringBuf->isFull());
                CHECK(ringBuf->getCount() == BYTE_COUNT);
            }
            THEN("all the string values are correct") {
                char buf[10];
                for (int i = 0; i < LONG_STR_COUNT; ++i) {
                    ringBuf->getNextString(buf, 10);
                    CHECK(strcmp(buf, "TEST") == 0);
                    ringBuf->getNextString(buf, 10);
                    CHECK(strcmp(buf, "LONG_TEST") == 0);
                    ringBuf->getNextString(buf, 10);
                    CHECK(strcmp(buf, "TEST") == 0);
                }
                CHECK(ringBuf->isEmpty());
            }
        }
        WHEN("we fill it up with strings (char *) of different fixed sizes, with no trailing null-byte") {
            for (int i = 0; i < LONG_STR_COUNT; ++i) {
                ringBuf->putString("TEST1", false);
                ringBuf->putString("LONG_TEST1", false);
                ringBuf->putString("TEST1", false);
            }
            THEN("the count is correct") {
                CHECK(ringBuf->isFull());
                CHECK(ringBuf->getCount() == BYTE_COUNT);
            }
            THEN("all the string values are correct") {
                char buf[10];
                for (int i = 0; i < LONG_STR_COUNT; ++i) {
                    ringBuf->getNextString(buf, 5, true);
                    CHECK(strcmp(buf, "TEST1") == 0);
                    ringBuf->getNextString(buf, 10, true);
                    CHECK(strcmp(buf, "LONG_TEST1") == 0);
                    ringBuf->getNextString(buf, 5, true);
                    CHECK(strcmp(buf, "TEST1") == 0);
                }
                CHECK(ringBuf->isEmpty());
            }
        }
        GIVEN("we fill it up with strings") {
            for (int i = 0; i < STRING_COUNT; ++i) {
                ringBuf->putString("TEST");
            }
            WHEN("one more is added") {
                THEN("a BufferOverrunException is thrown"){
                    CHECK_THROWS_AS(ringBuf->putString("TEST"), BufferOverrunException);
                }
            }
            WHEN("all the strings are removed") {
                char buf[10];
                for (int i = 0; i < STRING_COUNT; ++i) {
                    ringBuf->getNextString(buf, 10);
                }
                THEN("trying to take one more out throws a BufferUnderrunException") {
                    CHECK_THROWS_AS(ringBuf->getNextString(buf, 10), BufferUnderrunException);
                }
            }
        }
        WHEN("we fill it up with strings (char *) of value 'TEST' followed by null-byte") {
            for (int i = 0; i < STRING_COUNT; ++i)
                ringBuf->putString("TEST");
            THEN("we remove half the items") {
                int count = STRING_COUNT / 2;
                char buf[10];
                for (int i = 0; i < count; ++i)
                    ringBuf->getNextString(buf, 10);
                THEN("the byte count is half") {
                    CHECK(ringBuf->getCount() == (BYTE_COUNT / 2));
                }
                THEN("we add half the items back in") {
                    for (int i = 0; i < count; ++i)
                        ringBuf->putString("TEST");
                    THEN("the buffer is full") {
                        CHECK(ringBuf->isFull());
                    }
                }
            }
        }

    }
}
