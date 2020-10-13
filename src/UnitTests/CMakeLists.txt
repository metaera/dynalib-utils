set(CMAKE_CXX_STANDARD 17)

# set()

# set(GCC_COVERAGE_COMPILE_FLAGS "-g -O0 -coverage -fprofile-arcs -ftest-coverage")
# set(GCC_COVERAGE_LINK_FLAGS    "-coverage -lgcov")
# set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
# set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

set(UTIL_SOURCES
    TestUtilsMain.cpp
    TestDynaArray.cpp
    TestBTreeArray.cpp
    TestObject.cpp
    TestDynaList.cpp
    TestDynaRingBuffer.cpp
    TestDynaBuffer.cpp
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
#target_code_coverage(TestUtils AUTO)
target_link_libraries(TestUtils PUBLIC dynalib-utils)

add_test(NAME TestUtils COMMAND TestUtils)