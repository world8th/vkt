cmake_minimum_required(VERSION 3.17)
project(vkt-test VERSION 0.1.0)

include(CTest)
enable_testing()
set(CMAKE_CXX_STANDARD 20)

add_executable(vkt-test main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
