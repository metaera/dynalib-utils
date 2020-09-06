//
// Created by Ken Kopelson on 16/10/17.
//

#ifndef ROBOCHEFEMULATOR_TESTOBJECT_H
#define ROBOCHEFEMULATOR_TESTOBJECT_H

#include "../Utilities/IHashable.h"
#include "../Utilities/HashCoder.h"
#include "../Utilities/ICopyable.h"

class TestObject : public IHashable<TestObject>, public ICopyable<TestObject> {
    long _value;
public:
    TestObject() : _value(0) {}
    explicit TestObject(long value);
    virtual ~TestObject();

    void test();
    long getValue() const { return _value; }
    TestObject* setValue(long value) {
        _value = value;
        return this;
    }

    TestObject* copy() override {
        return new TestObject(_value);
    }

    int hashCode() const override {
        auto code = HashCoder();
        code.add(_value);
        return code.getCode();
    }

    bool operator== (const TestObject &other) const override {
        return _value == other.getValue();
    }

};

#endif //ROBOCHEFEMULATOR_TESTOBJECT_H
