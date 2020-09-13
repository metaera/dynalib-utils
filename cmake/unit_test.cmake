# Create the unit testing executable.
# By using glob, any test source files that are added to test/ will automatically
# be added to the unit testing executable.
file(GLOB_RECURSE TEST_SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/UnitTests/*.cpp)

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

add_executable(UnitTests ${SOURCE_FILES_NO_MAIN} ${TEST_SOURCE_FILES})

#add_executable(TestUtils ${UTIL_SOURCES})
target_link_libraries(UnitTests PUBLIC dynalib-utils)

# Enable CMake `make test` support.
enable_testing()
add_test(NAME UnitTests COMMAND UnitTests)
