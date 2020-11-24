cmake_minimum_required(VERSION 3.17)
project(vkt-test VERSION 0.1.0)

include(CTest)
enable_testing()
set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} test/test.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src)
target_compile_definitions(${PROJECT_NAME} PRIVATE -DVKT_CORE_USE_VMA -DVKT_USE_GLFW -DNOMINMAX)
target_link_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/lib)
target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS} glfw3)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
