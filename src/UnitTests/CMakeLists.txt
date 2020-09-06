set(CMAKE_CXX_STANDARD 17)

set(UTIL_SOURCES
    TestUtilsMain.cpp
    TestDynaArray.cpp
    TestObject.cpp
    TestDynaList.cpp
    TestDynaRingBuffer.cpp
    TestDynaHashMap.cpp
    TestHashCoder.cpp
    TestSynch.cpp
    TestWrapper.cpp
    TestDynaHashSet.cpp
    TestDynaLinkedList.cpp
    TestASCIICharClass.cpp
    TestParser.cpp
    TestRandomNumbers.cpp
    TestPageFile.cpp
    TestUUID.cpp
)

include_directories(${TEST_SOURCE_DIRECTORY})

add_executable(TestUtils ${UTIL_SOURCES})
target_link_libraries(TestUtils PUBLIC dynalib-utils)

add_test(NAME TestUtils COMMAND TestUtils)