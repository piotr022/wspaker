cmake_minimum_required(VERSION 3.10)

set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED True)

set(COMPILER_OPTIONS)

add_subdirectory(../ ${CMAKE_CURRENT_BINARY_DIR}/wspaker)  

project(paker_utils)

add_executable(paker_test paker_test.cpp)
target_link_libraries(paker_test wspaker)