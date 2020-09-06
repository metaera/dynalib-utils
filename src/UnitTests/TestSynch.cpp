#include "catch.hpp"
#include "../Utilities/Synchronize.h"
#include "../Utilities/Exception.h"
#include <type_traits>
#include <iostream>
#include <thread>

using namespace std;

class Test {
    Mutex mutex;
public:

    bool testFunc1(int& counter) synch(mutex) {
        ++counter;
        testFunc2(counter);
        synch_end
        return true;
    }

    bool testFunc2(int& counter) synch(mutex) {
        ++counter;
        synch_end
        return false;
    }

    bool testFunc3(int& counter) {
        int i = 0;

        synchronized(mutex) {
            ++counter;
        }

        return true;
    }
};

void t1(Test& test, int& counter) {
    for (int i = 0; i < 10000; ++i) {
        test.testFunc1(counter);
    }
    cout << "Thread 1 count: " << counter << endl;
}

void t2(Test& test, int& counter) {
    for (int i = 0; i < 10000; ++i) {
        test.testFunc2(counter);
    }
    cout << "Thread 2 count: " << counter << endl;
}

void t3(Test& test, int& counter) {
    for (int i = 0; i < 10000; ++i) {
        test.testFunc3(counter);
    }
    cout << "Thread 3 count: " << counter << endl;
}

void testFinally() {
    char* buf1 = new char[25];
    char* buf2 = new char[50];

    finally2(buf1,buf2) {
        cout << "Delete the two objects" << endl;
        delete[] buf1;
        delete[] buf2;
        throw Exception();
    }
    end_finally

    cout << "Do some things here then leave the function" << endl;
    cout << "Throwing an exception now..." << endl;
}

SCENARIO("Synchronization behaviour is correct") {
    GIVEN("a class with synchronized methods and three threads") {
        WHEN("a thread tries to invoke a method") {
            THEN("the threads will not collide") {
                int counter1 = 0;
                int counter2 = 0;
                int counter3 = 0;
                Test test;
                thread t_1(t1, ref(test), ref(counter1));
                thread t_2(t2, ref(test), ref(counter2));
                thread t_3(t3, ref(test), ref(counter3));
                t_1.join();
                t_2.join();
                t_3.join();
                CHECK(counter1 == 20000);
                CHECK(counter2 == 10000);
                CHECK(counter3 == 10000);
            }
        }
    }
}

SCENARIO("Finally block behaviour is correct") {
    GIVEN("a function with a finally block defined") {
        WHEN("the function ends, the finally block will trigger") {
            CHECK_THROWS(testFinally());
        }
    }
}
