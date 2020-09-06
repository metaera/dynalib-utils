//
// Created by Ken Kopelson on 27/12/17.
//

#include <string>
#include <iostream>

using namespace std;

#include "catch.hpp"
#include "../Utilities/DynaHashSetImpl.h"
#include "TestObject.h"

#define OBJ_COUNT 10000

MAKE_SETTYPE_INSTANCE(TestObject, TestObject);

SCENARIO("DynaHashSet operations function properly") {

    GIVEN("a newly created DynaHashSet that holds objects of type 'TestObject'") {
        auto* hashSet = new TestObjectSet();

        WHEN("instances are put in that have the same integer value") {
            auto* obj1 = new TestObject(24);
            auto* obj2 = new TestObject(56);
            hashSet->add(obj1);
            hashSet->add(obj1);
            hashSet->add(obj2);
            hashSet->add(obj2);

            THEN("each unique value only goes into the set once") {
                bool result = hashSet->contains(obj1);
                CHECK(result);
                CHECK(hashSet->count() == 2);

                AND_WHEN("searching with a different value of the same value") {
                    auto* obj3 = new TestObject(24);
                    result = hashSet->contains(obj3);
                    delete obj3;

                    THEN("the element is also found") {
                        CHECK(result);
                    }
                }
            }
        }
        WHEN(to_string(OBJ_COUNT) + " new TestObjects are added to the set") {
            for (int i = 0; i < OBJ_COUNT; ++i) {
                hashSet->add(new TestObject(i));
            }
            THEN("every value is found in the set") {
                auto* obj3 = new TestObject(0);

                for (int i = 0; i < OBJ_COUNT; ++i) {
                    CHECK(hashSet->contains(obj3->setValue(i)));
                }
                delete obj3;
                AND_THEN("number of items in set is " + to_string(OBJ_COUNT)) {
                    CHECK(hashSet->count() == OBJ_COUNT);
                }
            }
            THEN("every value is found in the set using the standard SetEntry iterator") {
                for (SetEntry<TestObject>* entry : *hashSet) {
                    TestObject* value = entry != nullptr ? entry->getValue() : nullptr;
                }
                AND_THEN("number of items in set is " + to_string(OBJ_COUNT)) {
                    CHECK(hashSet->count() == OBJ_COUNT);
                }
            }
            THEN("every value is found in the set using value iterator, forward and reverse") {
                int count = 0;
                for (auto iter = hashSet->values(); iter.hasNext(); ++iter) {
                    auto* value = *iter;
                    ++count;
                }
                CHECK(count == hashSet->count());
            }
            AND_WHEN("every value is removed") {
                auto* obj3 = new TestObject(0);

                for (int i = 0; i < OBJ_COUNT; ++i) {
                    auto* obj = hashSet->remove(obj3->setValue(i));
                    CHECK((obj != nullptr && obj->getValue() == i));
                    delete obj;
                }
                delete obj3;
                THEN("number of items in the map is 0") {
                    CHECK(hashSet->count() == 0);
                }
            }
        }
        WHEN("inserting items into the set, and also into a reference (non-owner) set") {
            auto* hashSet2 = new TestObjectSet();
            hashSet2->setOwnsMembers(false);

            for (int i = 0; i < OBJ_COUNT; ++i) {
                auto* obj = new TestObject(i);
                hashSet->add(obj);
                hashSet2->add(obj);
            }

            THEN("deleting reference set leaves owner-set intact") {
                auto* obj3 = new TestObject(0);
                for (long i = 0; i < OBJ_COUNT; ++i) {
                    hashSet2->deleteEntry(obj3->setValue(i));
                }
                for (uint i = 0; i < OBJ_COUNT; ++i) {
                    CHECK(hashSet->contains(obj3->setValue(i)));
                }
            }
            delete hashSet2;
        }


        delete hashSet;
    }
}
