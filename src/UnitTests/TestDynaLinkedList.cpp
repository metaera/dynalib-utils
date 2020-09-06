//
// Created by Ken Kopelson on 17/10/17.
//

#include <iostream>

using namespace std;

#include "catch.hpp"
#include "TestObject.h"
#include "Utilities/String.h"
#include "Utilities/DynaLinkedListImpl.h"

#define OBJ_COUNT 10000

MAKE_LINKEDLISTTYPE_INSTANCE(TestObject, TestObject);

SCENARIO("DynaLinkedList operations function properly") {
    GIVEN("a newly created DynaLinkedList that holds objects of type 'TestObject'") {
        auto* dList = new TestObjectLinkedList();

        WHEN("pushing " + to_string(OBJ_COUNT) + " new objects at front of list") {
            for (long i = 0; i < OBJ_COUNT; ++i) {
                dList->push(new TestObject(i));
            }
            THEN("all objects are in the list in reverse order") {
                long j = OBJ_COUNT - 1;
                auto* entry = dList->getFirst();
                for (uint i = 0; i < OBJ_COUNT; ++i, --j) {
                    TestObject* item = entry->getObject();
                    CHECK((item->getValue() == j));
                    item->test();
                    entry = entry->getNext();
                }
                AND_WHEN("moving all the objects to reverse them again") {
                    auto* dest = dList->getFirst();
                    for (uint i = 0; i < OBJ_COUNT; ++i) {
                        entry = dList->getLast();
                        dList->move(entry, dest, i != 0);
                        dest = entry;
                    }
                    THEN("all objects are in the list in forward order") {
                        for (uint i = 0; i < OBJ_COUNT; ++i) {
                            TestObject* item = dList->pop();
                            CHECK((item->getValue() == i));
                        }
                    }
                }
            }
        }
        WHEN("inserting items into list, and also into a reference (non-owner) list") {
            auto* dList2 = new TestObjectLinkedList();
            dList2->setOwnsMembers(false);

            for (long i = 0; i < OBJ_COUNT; ++i) {
                auto* obj = new TestObject(i);
                dList->push(obj);
                dList2->push(obj);
            }

            THEN("deleting reference list leaves owner-list intact") {
                for (long i = 0; i < OBJ_COUNT; ++i) {
                    dList2->pop();
                }
                auto* dest = dList->getFirst();
                for (uint i = 0; i < OBJ_COUNT; ++i) {
                    auto* entry = dList->getLast();
                    dList->move(entry, dest, i != 0);
                    dest = entry;
                }
                for (uint i = 0; i < OBJ_COUNT; ++i) {
                    TestObject* item = dList->pop();
                    CHECK((item->getValue() == i));
                }
            }
            delete dList2;
        }
        delete dList;
    }
}
