cmake_minimum_required(VERSION 3.10.2)
project(dynalib-utils VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE OFF)

# Cross-compiling Settings
if (UNIX AND NOT APPLE)
    message(STATUS "Using Linux cross-compiling settings")
    set(CMAKE_SYSTEM_NAME Linux)
    # set(CMAKE_SYSTEM_PROCESSOR x86_64)
    set(CLANG_DEFAULT_CXX_STDLIB libc++)
    set(CLANG_DEFAULT_C_STDLIB libc)
    set(CMAKE_C_COMPILER clang)
    set(CMAKE_C_STANDARD 99)
    set(CLANG_DEFAULT_CXX_STDLIB libc++)
    set(CMAKE_CXX_COMPILER clang++)
    set(CMAKE_SHARED_LINKER_FLAGS "-lpthread")
    set(CMAKE_EXE_LINKER_FLAGS "-lpthread")
endif()

if (APPLE AND NOT UNIX)
    message(STATUS "Using Apple cross-compiling settings")
    set(CMAKE_SYSTEM_NAME Apple)
    set(CMAKE_SHARED_LINKER_FLAGS "-L/usr/local/opt/llvm/lib -Wl,-rpath,/usr/local/opt/llvm/lib")
    set(CMAKE_EXE_LINKER_FLAGS "-L/usr/local/opt/llvm/lib -Wl,-rpath,/usr/local/opt/llvm/lib")
endif()

if (APPLE)
    message(STATUS "Using Apple settings")
    #    include_directories( /Developer/Headers/FlatCarbon )
    #    find_library(CARBON_LIBRARY Carbon)
    #    find_library(QUICKTIME_LIBRARY QuickTime )
    find_library(CORE_SERVICES_LIBRARY CoreServices REQUIRED)
    mark_as_advanced(CORE_SERVICES_LIBRARY)
    set(EXTRA_LIBS ${CORE_SERVICES_LIBRARY})
endif(APPLE)

# Tells executable to export all symbols to MODULE plugin libraries when they load
set(CMAKE_ENABLE_EXPORTS TRUE)

# Find packages (installed with Brew)
set(CURL_LIBRARY "-lcurl")
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})
find_package(Boost REQUIRED COMPONENTS system filesystem thread date_time)
include_directories(${Boost_INCLUDE_DIRS})

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "../lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
   SET(CMAKE_INSTALL_RPATH "../lib")
ENDIF("${isSystemDir}" STREQUAL "-1")

set (TOP_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set (UTILS_SOURCE_DIR ${TOP_SOURCE_DIR}/Utilities)

include_directories(./src)

set(UTIL_SOURCES
    ${TOP_SOURCE_DIR}/CreateUtilTypes.cpp
    ${UTILS_SOURCE_DIR}/Utils.cpp
    ${UTILS_SOURCE_DIR}/FileUtils.cpp
    ${UTILS_SOURCE_DIR}/Position.cpp
    ${UTILS_SOURCE_DIR}/Stopwatch.cpp
    ${UTILS_SOURCE_DIR}/PageFile.cpp
    ${UTILS_SOURCE_DIR}/DynaBuffer.cpp
    ${UTILS_SOURCE_DIR}/DynaArray.cpp
    ${UTILS_SOURCE_DIR}/DynaLogger.cpp
    ${UTILS_SOURCE_DIR}/DynaSort.cpp
    ${UTILS_SOURCE_DIR}/DynaList.cpp
    ${UTILS_SOURCE_DIR}/DynaBTree.cpp
    ${UTILS_SOURCE_DIR}/CheckForError.cpp
    ${UTILS_SOURCE_DIR}/DynaRingBuffer.cpp
    ${UTILS_SOURCE_DIR}/DynaAlloc.cpp
    ${UTILS_SOURCE_DIR}/DynaHashMap.cpp
    ${UTILS_SOURCE_DIR}/Synchronize.cpp
    ${UTILS_SOURCE_DIR}/DynaWaitQueue.cpp
    ${UTILS_SOURCE_DIR}/HashCoder.cpp
    ${UTILS_SOURCE_DIR}/SecurityToken.cpp
    ${UTILS_SOURCE_DIR}/StringWrapper.cpp
    ${UTILS_SOURCE_DIR}/TimeUtils.cpp
    ${UTILS_SOURCE_DIR}/PrintUtils.cpp
    ${UTILS_SOURCE_DIR}/DynaHashSet.cpp
    ${UTILS_SOURCE_DIR}/DynaLinkedList.cpp
    ${UTILS_SOURCE_DIR}/Graph/Node.cpp
    ${UTILS_SOURCE_DIR}/KAML/Manager.cpp
    ${UTILS_SOURCE_DIR}/KAML/File.cpp
    ${UTILS_SOURCE_DIR}/KAML/KAMLParser.cpp
    ${UTILS_SOURCE_DIR}/KAML/JSONParser.cpp
    ${UTILS_SOURCE_DIR}/KAML/JSONGenerator.cpp
    ${UTILS_SOURCE_DIR}/Parser/Tokenizer.cpp
    ${UTILS_SOURCE_DIR}/Parser/Token.cpp
    ${UTILS_SOURCE_DIR}/Parser/TokenizerContext.cpp
    ${UTILS_SOURCE_DIR}/Parser/CharacterClass.cpp
    ${UTILS_SOURCE_DIR}/date/tz.cpp
    ${UTILS_SOURCE_DIR}/DynaEventTable.cpp
    ${UTILS_SOURCE_DIR}/crypto/ISAAC64Random.cpp
    ${UTILS_SOURCE_DIR}/crypto/ISAACRandom.cpp
    ${UTILS_SOURCE_DIR}/crypto/SHA256.cpp
    ${UTILS_SOURCE_DIR}/UUID.cpp
    ${UTILS_SOURCE_DIR}/DynaCache.cpp
    ${UTILS_SOURCE_DIR}/DynaBufferCache.cpp
)

set(UTIL_INCLUDES
    ${UTILS_SOURCE_DIR}/Utils.h
    ${UTILS_SOURCE_DIR}/BitManip.h
    ${UTILS_SOURCE_DIR}/Exception.h
    ${UTILS_SOURCE_DIR}/FileUtils.h
    ${UTILS_SOURCE_DIR}/Position.h
    ${UTILS_SOURCE_DIR}/Stopwatch.h
    ${UTILS_SOURCE_DIR}/PageFile.h
    ${UTILS_SOURCE_DIR}/DynaBuffer.h
    ${UTILS_SOURCE_DIR}/DynaCache.h
    ${UTILS_SOURCE_DIR}/DynaCacheImpl.h
    ${UTILS_SOURCE_DIR}/DynaBufferCache.h
    ${UTILS_SOURCE_DIR}/DynaBufferCacheImpl.h
    ${UTILS_SOURCE_DIR}/DynaArray.h
    ${UTILS_SOURCE_DIR}/DynaArrayImpl.h
    ${UTILS_SOURCE_DIR}/DynaLogger.h
    ${UTILS_SOURCE_DIR}/DynaSort.h
    ${UTILS_SOURCE_DIR}/DynaSortImpl.h
    ${UTILS_SOURCE_DIR}/DynaList.h
    ${UTILS_SOURCE_DIR}/DynaBTree.h
    ${UTILS_SOURCE_DIR}/DynaListImpl.h
    ${UTILS_SOURCE_DIR}/CheckForError.h
    ${UTILS_SOURCE_DIR}/DynaRingBuffer.h
    ${UTILS_SOURCE_DIR}/DynaAlloc.h
    ${UTILS_SOURCE_DIR}/DynaAllocImpl.h
    ${UTILS_SOURCE_DIR}/DynaHashMap.h
    ${UTILS_SOURCE_DIR}/DynaHashMapImpl.h
    ${UTILS_SOURCE_DIR}/Synchronize.h
    ${UTILS_SOURCE_DIR}/DynaWaitQueue.h
    ${UTILS_SOURCE_DIR}/DynaWaitQueueImpl.h
    ${UTILS_SOURCE_DIR}/HashCoder.h
    ${UTILS_SOURCE_DIR}/IBufferCachePage.h
    ${UTILS_SOURCE_DIR}/ICacheConst.h
    ${UTILS_SOURCE_DIR}/ICacheFile.h
    ${UTILS_SOURCE_DIR}/ICachePage.h
    ${UTILS_SOURCE_DIR}/IComparable.h
    ${UTILS_SOURCE_DIR}/ICopyable.h
    ${UTILS_SOURCE_DIR}/IDynaBufferCacheListener.h
    ${UTILS_SOURCE_DIR}/IDynaCacheListener.h
    ${UTILS_SOURCE_DIR}/IDynaComparator.h
    ${UTILS_SOURCE_DIR}/IDynaEvent.h
    ${UTILS_SOURCE_DIR}/IDynaListener.h
    ${UTILS_SOURCE_DIR}/IHashable.h
    ${UTILS_SOURCE_DIR}/IndexConfig.h
    ${UTILS_SOURCE_DIR}/IntWrapper.h
    ${UTILS_SOURCE_DIR}/SecurityToken.h
    ${UTILS_SOURCE_DIR}/String.h
    ${UTILS_SOURCE_DIR}/StringWrapper.h
    ${UTILS_SOURCE_DIR}/TimeUtils.h
    ${UTILS_SOURCE_DIR}/TypeDefs.h
    ${UTILS_SOURCE_DIR}/PrintUtils.h
    ${UTILS_SOURCE_DIR}/DynaHashSet.h
    ${UTILS_SOURCE_DIR}/DynaHashSetImpl.h
    ${UTILS_SOURCE_DIR}/DynaLinkedList.h
    ${UTILS_SOURCE_DIR}/DynaLinkedListImpl.h
    ${UTILS_SOURCE_DIR}/DynaEventTable.h
    ${UTILS_SOURCE_DIR}/UUID.h
)

set(GRAPH_INCLUDES
    ${UTILS_SOURCE_DIR}/Graph/Node.h
)

set(PARSER_INCLUDES
    ${UTILS_SOURCE_DIR}/Parser/Tokenizer.h
    ${UTILS_SOURCE_DIR}/Parser/Token.h
    ${UTILS_SOURCE_DIR}/Parser/TokenizerContext.h
    ${UTILS_SOURCE_DIR}/Parser/CharacterClass.h
    ${UTILS_SOURCE_DIR}/Parser/ErrorContext.h
    ${UTILS_SOURCE_DIR}/Parser/ASCIICharClass.h
)

set(KAML_INCLUDES
    ${UTILS_SOURCE_DIR}/KAML/Manager.h
    ${UTILS_SOURCE_DIR}/KAML/File.h
    ${UTILS_SOURCE_DIR}/KAML/KAMLParser.h
    ${UTILS_SOURCE_DIR}/KAML/JSONParser.h
    ${UTILS_SOURCE_DIR}/KAML/JSONGenerator.h
)

set(DATE_INCLUDES
    ${UTILS_SOURCE_DIR}/date/tz.h
)

set(CRYPTO_INCLUDES
    ${UTILS_SOURCE_DIR}/crypto/ISAAC64Random.h
    ${UTILS_SOURCE_DIR}/crypto/ISAACRandom.h
    ${UTILS_SOURCE_DIR}/crypto/SHA256.h
    ${UTILS_SOURCE_DIR}/crypto/standard.h
)

# Specify the libraries that will be linked in
add_library(dynalib-utils SHARED ${UTIL_SOURCES})

# Configure properties for the libraries
set_target_properties(dynalib-utils PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION 1.0)

# Link each of the libraries
target_link_libraries(dynalib-utils ${CURL_LIBRARIES})

# Build the test executable
add_executable(testKaml ${TOP_SOURCE_DIR}/examples/testKaml.cpp)
target_link_libraries(testKaml PUBLIC dynalib-utils)

# Enable unit testing
# enable_testing()
# add_subdirectory(src/UnitTests UnitTests)

add_custom_target(TARGET testKaml POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${TOP_SOURCE_DIR}/examples/sample.kaml ${CMAKE_CURRENT_BINARY_DIR}/sample.kaml)

install(TARGETS dynalib-utils LIBRARY DESTINATION lib)
install(FILES ${UTIL_INCLUDES} DESTINATION include/dynalib)
install(FILES ${GRAPH_INCLUDES} DESTINATION include/dynalib/Graph)
install(FILES ${PARSER_INCLUDES} DESTINATION include/dynalib/Parser)
install(FILES ${KAML_INCLUDES} DESTINATION include/dynalib/KAML)
install(FILES ${DATE_INCLUDES} DESTINATION include/dynalib/date)
install(FILES ${CRYPTO_INCLUDES} DESTINATION include/dynalib/crypto)

# Include extra CMake files that are used for testing and linting.
include(${CMAKE_SOURCE_DIR}/cmake/unit_test.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/clang_tools.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/cppcheck.cmake)

# Any source files added to include/ or src/ will automatically be added to the project.
file(GLOB_RECURSE INCLUDE_FILES ${CMAKE_SOURCE_DIR}/include/*.h)

# If external libraries are needed, use find_library() and target_link_libraries()
# to dynamically link them to your executable. Generally, you'll want to install
# them onto the host using your OS's package manager.
# Example:
#   find_library(LIB_FMT fmt)
#   target_link_libraries(clang-blueprint ${LIB_FMT})

# Enable all warnings except for no-sign-compare, which Clang disables
# by default.
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-sign-compare")

option(WERROR "Turn warnings into errors" OFF)
if (WERROR)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif ()

# auto install after build
add_custom_command(
    TARGET testKaml
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target install
)