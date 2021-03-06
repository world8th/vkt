//#ifdef OS_WIN
#if (defined(_WIN32) || defined(__MINGW32__) || defined(_MSC_VER_) || defined(__MINGW64__)) 
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif

//#ifdef OS_LNX
#ifdef __linux__
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#endif

//
#define ENABLE_EXTENSION_GLM
#define ENABLE_EXTENSION_VMA
#define ENABLE_EXTENSION_RTX
#define GLFW_INCLUDE_VULKAN
#define VK_ENABLE_BETA_EXTENSIONS

//
#ifndef VKH_USE_XVK
#define VKH_USE_XVK
#endif

//
#ifndef VKT_USE_VMA
#define VKT_USE_VMA
#endif

//
#ifndef VKH_USE_VULKAN_HPP
#define VKH_USE_VULKAN_HPP
#endif

// Force include for avoid GLAD problem...
// 
#include <vkh/matching.hpp>

//
#include <vkt/inline.hpp>
#include <vkt/essential.hpp>

// 
#include "./vector-typed.hpp"
#include "./image-region.hpp"

//
namespace vkf {

};
