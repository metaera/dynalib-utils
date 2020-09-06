//
// Created by Ken Kopelson on 26/10/17.
//

#include <iostream>
#include <string>

#include "catch.hpp"
#include "../Utilities/IntWrapper.h"
#include "../Utilities/Stopwatch.h"
#include "../Utilities/DynaHashMapImpl.h"
#include "TestObject.h"
#include "../Utilities/String.h"

MAKE_MAPTYPE_INSTANCE(Integer, TestObject, IntTestObject);

SCENARIO("IntWrapper behaviour is correct") {
    GIVEN("two sets of numbers, wrapped and unwrapped") {
        int     a1 = 1000;
        int     b1  = 50;
        Integer a2 = 1000;
        Integer b2 = 50;

        WHEN("math is done on both sets") {
            THEN("performance is somewhat slower for the wrapped") {
                auto* sw = new Stopwatch();

                sw->start();
                for (int i = 0; i < 100000; ++i) {
                    a1 += 50;
                }
                sw->stop();
                sw->printMessage();

                sw->restart();
                for (int i = 0; i < 100000; ++i) {
                    a2 += 50;
                }
                sw->stop();
                sw->printMessage();
            }
        }

        WHEN("wrapped values are compared") {
            THEN("the comparisons are correct") {
                CHECK((a1 == a2));
                CHECK((b1 == b2));
                CHECK((a2 != b2));
                CHECK((a1 > b2));
                CHECK((a1 >= b2));
                CHECK((b1 < a2));
                CHECK((b1 <= a2));
                CHECK((a2 > b2));
                CHECK((a2 >= b2));
                CHECK((b2 < a2));
                CHECK((b2 <= a2));
            }
        }
    }
    GIVEN("two sets of numbers, wrapped and unwrapped") {
        Long a1 = 10;
        Long b1 = 20;
        Long c1 = 30;
        Long d1 = 40;

        long a2 = 10;
        long b2 = 20;
        long c2 = 30;
        long d2 = 40;

        WHEN("the same math is done on both sets") {
            THEN("the result is the same for both") {
                long result1 = ((a1 + b1 - c1) / 4L) * (b1 + c1 / d1) << 1L;
                long result2 = ((a2 + b2 - c2) / 4L) * (b2 + c2 / d2) << 1L;

                CHECK((result1 == result2));
            }
        }
    }

    GIVEN("a DynaHashMap that uses IntWrapped 'Integer' as the key") {
        auto* map = new DynaHashMap<Integer, TestObject>();

        WHEN("items are added, and then retrieved using regular ints") {
            map->put(Integer(24), new TestObject(56));
            map->put(Integer(25), new TestObject(57));
            map->put(Integer(26), new TestObject(58));
            map->put(Integer(27), new TestObject(59));
            map->put(28, new TestObject(60));
            THEN("the items are found") {
                TestObject* obj = map->get(24);
                CHECK((obj->getValue() == 56));
                obj = map->get(25);
                CHECK((obj->getValue() == 57));
                obj = map->get(26);
                CHECK((obj->getValue() == 58));
                obj = map->get(27);
                CHECK((obj->getValue() == 59));
                obj = map->get(28);
                CHECK((obj->getValue() == 60));
            }
        }
        delete map;
    }
    GIVEN("a DynaHashMap that uses String as the key") {
        auto* map = new DynaHashMap<String, TestObject>();

        WHEN("items are added, and then retrieved using regular ints") {
            auto* s = new String("Test5");

            map->put(new String("Test1"), new TestObject(56));
            map->put(new String("Test2"), new TestObject(57));
            map->put(new String("Test3"), new TestObject(58));
            map->put(new String("Test4"), new TestObject(59));
            map->put(s, new TestObject(60));
            map->put("Test6", new TestObject(61));
            THEN("the items are found") {
                TestObject* obj = map->get("Test1");
                CHECK((obj->getValue() == 56));
                obj = map->get("Test2");
                CHECK((obj->getValue() == 57));
                obj = map->get("Test3");
                CHECK((obj->getValue() == 58));
                obj = map->get("Test4");
                CHECK((obj->getValue() == 59));
                obj = map->get("Test5");
                CHECK((obj->getValue() == 60));
                obj = map->get("Test6");
                CHECK((obj->getValue() == 61));
            }
        }
        delete map;
    }
}
