//
// Created by Ken Kopelson on 16/10/17.
//

#include <string>
#include <iostream>
using namespace std;

#include "TestObject.h"

#include "../Utilities/DynaArrayImpl.h"
#include "../Utilities/DynaListImpl.h"

// This creates an instantiated class of DynaList for holding TestObjects
MAKE_LISTTYPE_INSTANCE(TestObject, TestObject);

TestObject::TestObject(long value) : _value(value) {
}

TestObject::~TestObject() {
//    cout << "Destructor called: " << _value << endl;
}

void TestObject::test() {
//    cout << "This is from TestObject" << endl;
}

