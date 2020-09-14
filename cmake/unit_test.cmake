# Create the unit testing executable.
# By using glob, any test source files that are added to test/ will automatically
# be added to the unit testing executable.
file(GLOB_RECURSE TEST_SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/UnitTests/*.cpp)

#TODO: Remove excluded Tests
list(REMOVE_ITEM TEST_SOURCE_FILES "${CMAKE_SOURCE_DIR}/src/UnitTests/TestDynaHashMap.cpp")

add_executable(UnitTests ${SOURCE_FILES_NO_MAIN} ${TEST_SOURCE_FILES})

#add_executable(TestUtils ${UTIL_SOURCES})
target_link_libraries(UnitTests PUBLIC dynalib-utils)

# Enable CMake `make test` support.
enable_testing()
add_test(NAME UnitTests COMMAND UnitTests)
