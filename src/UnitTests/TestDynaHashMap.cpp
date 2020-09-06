//
// Created by Ken Kopelson on 21/10/17.
//

#include <string>
#include <iostream>

using namespace std;

#include "catch.hpp"
#include "../Utilities/DynaHashMapImpl.h"
#include "TestObject.h"

#define OBJ_COUNT 10000


MAKE_MAPTYPE_INSTANCE(TestObject, TestObject, TestObject);

SCENARIO("DynaHashMap operations function properly") {

//    cout << "Type: " << typeid(bool).name() << endl;
//    cout << "Type: " << typeid(int).name() << endl;
//    cout << "Type: " << typeid(unsigned int).name() << endl;
//    cout << "Type: " << typeid(short).name() << endl;
//    cout << "Type: " << typeid(unsigned short).name() << endl;
//    cout << "Type: " << typeid(long).name() << endl;
//    cout << "Type: " << typeid(unsigned long).name() << endl;
//    cout << "Type: " << typeid(float).name() << endl;
//    cout << "Type: " << typeid(double).name() << endl;
//    cout << "Type: " << typeid(char).name() << endl;
//    cout << "Type: " << typeid(char*).name() << endl;
//    cout << "Type: " << typeid(unsigned char).name() << endl;
//    cout << "Type: " << typeid(TestObject).name() << endl;
//    cout << "Type: " << typeid(TestObject*).name() << endl;

    GIVEN("a newly created DynaHashMap that holds objects of type 'TestObject'") {
        auto* hashMap = new TestObjectMap();

        WHEN("instances are put in that have the same integer value") {
            auto* obj1 = new TestObject(24);
            auto* obj2 = new TestObject(56);
            hashMap->put(obj1, obj2);
            hashMap->put(obj1, obj2);
            hashMap->put(obj1, obj2);
            hashMap->put(obj1, obj2);

            THEN("each unique value only goes into the map once") {
                auto* result = hashMap->get(obj1);
                CHECK((result != nullptr && result->getValue() == 56));
                CHECK((hashMap->count() == 1));

                AND_WHEN("searching with a different key object of the same value") {
                    auto* obj3 = new TestObject(24);
                    result = hashMap->get(obj3);
                    delete obj3;

                    THEN("the element is also found") {
                        CHECK((result != nullptr && result->getValue() == 56));
                    }
                }
            }
        }
        WHEN(to_string(OBJ_COUNT) + " new TestObjects are added to the map") {
            for (int i = 0; i < OBJ_COUNT; ++i) {
                hashMap->put(new TestObject(i), new TestObject(i + 20000));
            }
            THEN("every value is found in the map") {
                auto* obj3 = new TestObject(0);

                for (int i = 0; i < OBJ_COUNT; ++i) {
                    auto* obj = hashMap->get(obj3->setValue(i));
                    CHECK(obj->getValue() == (i + 20000));
                }
                delete obj3;
                AND_THEN("number of items in map is " + to_string(OBJ_COUNT)) {
                    CHECK((hashMap->count() == OBJ_COUNT));
                }
            }
            THEN("every value is found in the map using the standard MapEntry iterator") {
                for (MapEntry<TestObject, TestObject>* entry : *hashMap) {
                    TestObject* value = entry != nullptr ? entry->getValue() : nullptr;
                }
                AND_THEN("number of items in map is " + to_string(OBJ_COUNT)) {
                    CHECK((hashMap->count() == OBJ_COUNT));
                }
            }
            THEN("every value is found in the map using key and value iterators, forward and reverse") {
                int count = 0;
                for (auto iter = hashMap->keys(); iter.hasNext(); ++iter) {
                    auto* key = *iter;
                    ++count;
                }
                CHECK((count == hashMap->count()));

                count = 0;
                for (auto iter = hashMap->values(); iter.hasNext(); ++iter) {
                    auto* value = *iter;
                    ++count;
                }
                CHECK((count == hashMap->count()));
            }
            THEN("every value is removed") {
                auto* obj3 = new TestObject(0);

                for (int i = 0; i < OBJ_COUNT; ++i) {
                    auto* obj = hashMap->remove(obj3->setValue(i));
                    CHECK((obj != nullptr && obj->getValue() == (i + 20000)));
                    delete obj;
                }
                delete obj3;
                AND_THEN("number of items in the map is 0") {
                    CHECK((hashMap->count() == 0));
                }
            }
        }
        delete hashMap;
    }
}