cmake_minimum_required (VERSION 3.10)
set (PROJECT_NAME rtvg)
project (${PROJECT_NAME} C CXX)

# Architecture specific
if (COMPILER_ID MATCHES "MSVC")
  set(MSVC_${lang}_ARCHITECTURE_ID "${ARCHITECTURE_ID}")
endif()

# Import modules 
set (DEP_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set (CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${DEP_DIR}" "${DEP_DIR}/lib/cmake/")
find_package (Vulkan QUIET)
find_package (ArrayFire QUIET)
#find_package (CUDA QUIET)

# Set source directories
set(SOURCES_LIST 
    "./include/*.h"
    "./include/*.hpp"
    "./include/radx/*"
    "./include/vulkan/*"
    "./include/vma/*"
    "./include/vku/*"
    "./include/vkt/*"
    "./include/volk/*"
    "./include/spirv/*"
    "./include/spirv/*/*"
    "./src/*.hpp"
    "./src/*.h"
    "./src/*/*.hpp"
    "./src/*/*.h"
    "./src/include/*.hpp"
    "./src/include/*.h"
    "./src/include/*/*.hpp"
    "./src/include/*/*.h"
)

# Set definition
set (DEFINES
    -D_CRT_SECURE_NO_WARNINGS
    -D_USE_MATH_DEFINES
    -DGLM_FORCE_SWIZZLE
    -DGLM_ENABLE_EXPERIMENTAL
    -DGLFW_INCLUDE_NONE
    -DEXPERIMENTAL_GLTF
    -DNSM=vkt
    -DTINY_RENDERER_VK
)

# Set MSVC/Windows specific 
if (MSVC OR MSVS OR MSYS OR MINGW)
    set(DEFINES ${DEFINES} -DOS_WIN -DWIN32) 
endif()

# Set Linux specific
if (UNIX OR LINUX)
    set(DEFINES ${DEFINES} -DOS_LNX) 
endif()

# set libs and include dirs of projects
set (LIBS glfw3)
set (LIB_DIR ${PROJECT_SOURCE_DIR} "${DEP_DIR}/lib")
set (INC_DIR 
    "${DEP_DIR}/include" 
    "${DEP_DIR}/src/" 
    "${DEP_DIR}/src/base/" 
    "${DEP_DIR}")

if (Vulkan_INCLUDE_DIR)
    set (INC_DIR ${INC_DIR} ${Vulkan_INCLUDE_DIR})
endif()

# set CUDA compiling options
if (CUDA_FOUND) 
set(CUDA_ARCH -gencode=arch=compute_70,code=sm_70)
set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} ${CUDA_ARCH} -g -G)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/")
set(CMAKE_CUDA_FLAGS ”-arch=sm_70 -std=c++14”)
endif()

# find package Thrust 
find_package (PkgConfig QUIET)
pkg_search_module(GLFW QUIET glfw3)
pkg_search_module(GLFW QUIET renderdoc)

find_package (Boost QUIET)
find_package (GLFW3 QUIET)

#if (CUDA_FOUND) 
    #set (INC_DIR ${INC_DIR} ${Boost_INCLUDE_DIR})
    #set (LIB_DIR ${LIB_DIR} ${Boost_LIBRARIES})
    #set (LIBS ${LIBS} ${CUDART_LIBRARY} ${CUDA_LIBRARY})
#endif()

if (CUDA_FOUND) 
    set (INC_DIR ${INC_DIR} ${CUDA_INCLUDE_DIRS} ${THRUST_INCLUDE_DIR})
    set (LIB_DIR ${LIB_DIR} ${CUDA_LIBRARY_DIRS})
    #set (LIBS ${LIBS} ${CUDART_LIBRARY} ${CUDA_LIBRARY})
endif()

if (ArrayFire_OpenCL_FOUND)
    set (INC_DIR ${INC_DIR} ${ArrayFire_INCLUDE_DIRS})
    set (LIBS ${LIBS} ${ArrayFire_OpenCL_LIBRARIES})
    set (DEFINES ${DEFINES} -DENABLE_ARRAYFIRE_BENCHMARK) 
endif()

# 
if (Vulkan_LIBRARY)
    set (LIBS ${LIBS} ${Vulkan_LIBRARY})
	set (INC_DIR ${INC_DIR} ${Vulkan_INCLUDE_DIR})
endif()

if (GLFW_LIBRARIES)
    set (LIBS ${LIBS} ${GLFW_LIBRARIES})
    set (INC_DIR ${INC_DIR} ${GLFW_INCLUDE_DIRS})
endif()


# set GCC8 (Linux) libs
# MingW-W64 TODO
if (UNIX OR LINUX)
find_package(X11 QUIET)
    if (X11_FOUND)
        set (LIBS ${LIBS} ${X11_LIBRARIES})
        set (INC_DIR ${INC_DIR} ${X11_INCLUDE_DIR})
    endif()
    set (LIBS ${LIBS} -static-libgcc -static-libstdc++ -lX11 -lXrandr -lpthread -ldl)
endif()

# set language specific
set (CMAKE_CXX_EXTENSIONS 1)
#set (CMAKE_CXX_STANDARD 17)
#set (CMAKE_C_STANDARD 11)

# compiler specific options
if (MSVC) # Clang in Visual Studio 2017
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /arch:AVX2 /tune:skylake /fp:strict /permissive- /Qspectre /JMC /Oy /Oi /Ot /Ox /GT /GL /Gy /Qpar /W3 /NODEFAULTLIB:library") # Native compiler of Visual Studio 2017
    set (CMAKE_DOTNET_TARGET_FRAMEWORK_VERSION "v4.7.2")
    # fix MSVC 2019 winprt lib 
    #set (LIB_DIR ${LIB_DIR} "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.20.27508/lib/x64") 
else()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -std=c++17 -march=skylake -mtune=skylake -fpermissive -w -Ofast")
    set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -m64 -std=c1x   -march=skylake -mtune=skylake -fpermissive -w -Ofast")
    if (CUDA_FOUND)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    endif()
    if (WIN32)
        set (CMAKE_CXX_FLAGS "${CMAK_CXX_FLAGS} -Wa,-mbig-obj")
        set (CMAKE_C_FLAGS "${CMAK_C_FLAGS} -Wa,-mbig-obj")
    endif()
endif()

# set optimizations for build types
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")

# set default compiler options
link_directories(${LIB_DIR})
include_directories(${INC_DIR})
file (GLOB RSOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${SOURCES_LIST})

# make gltfPBR
set (APPLICATION_NAME sort-test)

if (CUDA_FOUND)
set(DEFINES ${DEFINES} -DTHRUST_TESTABLE)
add_definitions(${DEFINES})
cuda_add_library(sort-thrust STATIC src/test/sort-thrust.cu)
set_target_properties(sort-thrust PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(sort-thrust PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
set(LIBS ${LIBS} sort-thrust)
endif()

# sort test
add_definitions(${DEFINES})
add_executable(${APPLICATION_NAME} src/examples/rtvg.cpp src/examples/main.cpp ${RSOURCES})
target_link_libraries(${APPLICATION_NAME} PRIVATE ${LIBS})


# add includes to MSVC explorer 
foreach(source IN LISTS RSOURCES)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach()
