cmake_minimum_required(VERSION 3.23)
project(librequests C)

add_subdirectory(vendors/hashmap)

set(CMAKE_C_STANDARD 99)
set(LIBRARY_SOURCE_FILES librequests.h tcp.h http.h librequests.c)
add_library(librequests SHARED ${LIBRARY_SOURCE_FILES})
target_link_libraries(librequests hashmap)

target_include_directories(librequests PUBLIC vendors/hashmap)

set(LIBRARY_TEST_SOURCE_FILES test.c)
add_executable(librequests-test ${LIBRARY_TEST_SOURCE_FILES})
target_link_libraries(librequests-test librequests)