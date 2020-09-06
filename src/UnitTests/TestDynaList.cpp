//
// Created by Ken Kopelson on 17/10/17.
//

#include <iostream>

using namespace std;

#include "catch.hpp"
#include "TestObject.h"
#include "Utilities/String.h"
#include "Utilities/DynaListImpl.h"

#define OBJ_COUNT 10000

MAKE_LISTTYPE_INSTANCE(TestObject, TestObject);

SCENARIO("DynaList operations function properly") {
    GIVEN("a newly created DynaList that holds objects of type 'TestObject'") {
        auto* dList = new TestObjectList();

        WHEN("inserting " + to_string(OBJ_COUNT) + " new objects at index 0") {
            for (long i = 0; i < OBJ_COUNT; ++i) {
                dList->insert(0, new TestObject(i));
            }
            THEN("all objects are in the list in reverse order") {
                long j = OBJ_COUNT - 1;
                for (uint i = 0; i < OBJ_COUNT; ++i, --j) {
                    TestObject* item = (*dList)[i];
                    CHECK((item->getValue() == j));
                    item->test();
                }
                AND_WHEN("moving all the objects to reverse them again") {
                    for (uint i = 0; i < OBJ_COUNT; ++i) {
                        dList->move(OBJ_COUNT - 1, i);
                    }
                    THEN("all objects are in the list in forward order") {
                        for (uint i = 0; i < OBJ_COUNT; ++i) {
                            TestObject* item = (*dList)[i];
                            CHECK((item->getValue() == i));
                        }
                    }
                }
            }
        }
        WHEN("inserting items into list, and also into a reference (non-owner) list") {
            auto* dList2 = new TestObjectList();
            dList2->setOwnsMembers(false);

            for (long i = 0; i < OBJ_COUNT; ++i) {
                auto* obj = new TestObject(i);
                dList->insert(0, obj);
                dList2->insert(0, obj);
            }

            THEN("deleting reference list leaves owner-list intact") {
                for (long i = 0; i < OBJ_COUNT; ++i) {
                    dList2->deleteItem(0);
                }
                for (uint i = 0; i < OBJ_COUNT; ++i) {
                    dList->move(OBJ_COUNT - 1, i);
                }
                for (uint i = 0; i < OBJ_COUNT; ++i) {
                    TestObject* item = (*dList)[i];
                    CHECK((item->getValue() == i));
                }
            }

            delete dList2;
        }


        delete dList;
    }
}
