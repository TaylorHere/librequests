cmake_minimum_required(VERSION 3.23)
project(librequests C)

add_subdirectory(vendors/hashmap)

set(CMAKE_C_STANDARD 99)
add_library(librequests SHARED  src/librequests.c)
target_link_libraries(librequests hashmap)

target_include_directories(librequests
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        vendors/hashmap)

set(LIBRARY_TEST_SOURCE_FILES test/test.c)
add_executable(librequests-test ${LIBRARY_TEST_SOURCE_FILES})
target_link_libraries(librequests-test librequests)