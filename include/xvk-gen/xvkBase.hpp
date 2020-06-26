#pragma once

// You may override the following macros in your application

// Interface functions accessibility
//	these are useful to override when you are extending xvk classes for your own higher-level abstraction
#ifndef XVK_INTERFACE_LOADER_RAW_FUNCTIONS_ACCESSIBILITY
	#define XVK_INTERFACE_LOADER_RAW_FUNCTIONS_ACCESSIBILITY public
#endif
#ifndef XVK_INTERFACE_RAW_FUNCTIONS_ACCESSIBILITY
	#define XVK_INTERFACE_RAW_FUNCTIONS_ACCESSIBILITY public
#endif
#ifndef XVK_INTERFACE_ABSTRACTED_FUNCTIONS_ACCESSIBILITY
	#define XVK_INTERFACE_ABSTRACTED_FUNCTIONS_ACCESSIBILITY public
#endif

// XVK_CHECK_FUNC
// This macro will validate that the function pointer is loaded before calling it, avoiding a segfault.
// However, this should never be needed if available extensions are checked correctly before calling these functions.
// In release mode, we should leave it empty and keep these functions truly zero-cost
// You may override this macro to do whatever you want
#ifndef XVK_CHECK_FUNC
	#ifdef _DEBUG
		#define XVK_CHECK_FUNC(func) \
			if (!func) throw std::runtime_error("Vulkan Function " #func " failed to load");
	#else
		#define XVK_CHECK_FUNC(func)
	#endif
#endif


////////////////////////////////////////////////////////////////////


// This include is needed for loading libraries dynamically. In GCC for linux you need to add -ldl in your compiler options. 
// XVK_EXPORT and XVK_IMPORT are available to define in cases where you want to compile/include xvk in a shared library and use it in the linking program
#ifdef _WIN32
	#include <windows.h>
	#ifndef __XVK_DLLEXPORT
	#define __XVK_DLLEXPORT __declspec(dllexport)
	#endif
	#ifndef __XVK_DLLIMPORT
	#define __XVK_DLLIMPORT __declspec(dllimport)
	#endif
#else
	#include <dlfcn.h>
	#ifndef __XVK_DLLEXPORT
	#define __XVK_DLLEXPORT
	#endif
	#ifndef __XVK_DLLIMPORT
	#define __XVK_DLLIMPORT
	#endif
#endif
#ifdef XVK_EXPORT
	#define XVK_LIB __XVK_DLLEXPORT
#endif
#ifdef XVK_IMPORT
	#define XVK_LIB __XVK_DLLIMPORT
#endif
#ifndef XVK_LIB
	#define XVK_LIB
#endif

// for std::runtime_error
#include <stdexcept>

// Here we automatically include the vulkan header if it is not already included
#ifndef VULKAN_H_
	#define VK_NO_PROTOTYPES
	#include "Vulkan-Headers/include/vulkan/vulkan.h"
#endif

// Function loading macros
#define XVK_DEF_INTERFACE_FUNC(func) PFN_##func func = 0;
#ifdef XVK_EXPOSE_NATIVE_VULKAN_FUNCTIONS_NAMESPACE
	#define XVK_DEF_INTERFACE_FUNC_H(func) extern XVK_LIB PFN_##func func;
	#define XVK_DEF_INTERFACE_FUNC_C(func) PFN_##func func = 0;
	#define XVK_LOAD_GLOBAL_FUNC(func) \
		if (!(this->func = (PFN_##func) vkGetInstanceProcAddr(nullptr, #func))){\
			throw std::runtime_error("Failed to load vulkan global function pointer for " #func);\
		}\
		XVK_EXPOSE_NATIVE_VULKAN_FUNCTIONS_NAMESPACE::func = this->func;
	#define XVK_LOAD_INSTANCE_FUNC(func) \
		this->func = (PFN_##func) loader->vkGetInstanceProcAddr(handle, #func);\
		XVK_EXPOSE_NATIVE_VULKAN_FUNCTIONS_NAMESPACE::func = this->func;
	#define XVK_LOAD_DEVICE_FUNC(func) \
		this->func = (PFN_##func) instance->vkGetDeviceProcAddr(handle, #func);\
		XVK_EXPOSE_NATIVE_VULKAN_FUNCTIONS_NAMESPACE::func = this->func;
	#include "xvkInterface.h"
#else
	#define XVK_DEF_INTERFACE_FUNC_H(func)
	#define XVK_DEF_INTERFACE_FUNC_C(func)
	#define XVK_LOAD_GLOBAL_FUNC(func) \
		if (!(func = (PFN_##func) vkGetInstanceProcAddr(nullptr, #func))){\
			throw std::runtime_error("Failed to load vulkan global function pointer for " #func);\
		}
	#define XVK_LOAD_INSTANCE_FUNC(func) \
		func = (PFN_##func) loader->vkGetInstanceProcAddr(handle, #func);
	#define XVK_LOAD_DEVICE_FUNC(func) \
		func = (PFN_##func) instance->vkGetDeviceProcAddr(handle, #func);
#endif

#ifdef XVK_USE_QT_VULKAN_LOADER
	#include <QVulkanInstance>
#endif

namespace xvk { namespace Base {
	class LoaderBase {
	protected:
		#ifndef XVK_USE_QT_VULKAN_LOADER
		#ifdef _WIN32
			HINSTANCE vulkanLib = nullptr;
		#else
			void* vulkanLib = nullptr;
		#endif
		
		LoaderBase() {}
		#endif
		
		virtual void LoadFunctionPointersInterface() = 0;
		
		virtual ~LoaderBase() {
			#ifndef XVK_USE_QT_VULKAN_LOADER
			// Unload vulkan loader library
			if (vulkanLib) {
				#ifdef _WIN32
					FreeLibrary(vulkanLib);
				#else
					dlclose(vulkanLib);
				#endif
				vulkanLib = nullptr;
			}
			#endif
		}
		
	public:
	
		#ifdef XVK_USE_QT_VULKAN_LOADER
			QVulkanInstance* qtVulkanInstance; 
			LoaderBase(QVulkanInstance* inst) : qtVulkanInstance(inst) {}

			PFN_vkVoidFunction vkGetInstanceProcAddr(VkInstance, const char* funcName) {
				return qtVulkanInstance->getInstanceProcAddr(funcName);
			}
		#else
		XVK_DEF_INTERFACE_FUNC( vkGetInstanceProcAddr )
		#endif
		
		bool operator()() {

			#ifndef XVK_USE_QT_VULKAN_LOADER
				if (vulkanLib) return true;
				#ifdef _WIN32
					#ifdef _T
						vulkanLib = LoadLibrary(_T("vulkan-1.dll"));
					#else
						vulkanLib = LoadLibrary("vulkan-1.dll");
					#endif
				#else
					vulkanLib = dlopen("libvulkan.so", RTLD_NOW);
				#endif
				if (!vulkanLib) {
					return false;
				}
				#ifdef _WIN32
					vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)GetProcAddress(vulkanLib, "vkGetInstanceProcAddr");
				#else
					vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)dlsym(vulkanLib, "vkGetInstanceProcAddr");
				#endif
				if (!vkGetInstanceProcAddr) {
					return false;
				}
			#endif

			LoadFunctionPointersInterface();
			
			return true;
		}
		
	};
	
	class InstanceBase {
	public: 
		LoaderBase* loader;
		VkInstance handle;
		
	protected:
	
		InstanceBase() : handle(VK_NULL_HANDLE){}
		InstanceBase(LoaderBase* loader, VkInstance instance) : loader(loader), handle(instance) {}
		
		virtual ~InstanceBase() {}
		
		virtual void LoadFunctionPointersInterface() = 0;
		
		void LoadFunctionPointers() {
			XVK_LOAD_INSTANCE_FUNC( vkGetDeviceProcAddr )
			LoadFunctionPointersInterface();
		}
		
	public:
	
		XVK_DEF_INTERFACE_FUNC( vkGetDeviceProcAddr )
		
	};
	
	class DeviceBase {
	public: 
		InstanceBase* instance;
		VkDevice handle;
		
	protected:
	
		DeviceBase() : handle(VK_NULL_HANDLE){}
		DeviceBase(InstanceBase* instance, VkDevice device) : instance(instance), handle(device) {}
		
		virtual ~DeviceBase() {}
		
		virtual void LoadFunctionPointersInterface() = 0;
		
		void LoadFunctionPointers() {
			LoadFunctionPointersInterface();
		}
		
	};
}}
