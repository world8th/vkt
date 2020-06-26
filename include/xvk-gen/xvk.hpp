#pragma once

#include "xvkInterface.hpp"

// GLFW
#ifdef XVK_INCLUDE_GLFW
	#include "glfw/include/GLFW/glfw3.h"
#endif

// GLM
#ifdef XVK_INCLUDE_GLM
	// GLM
	#define GLM_FORCE_RADIANS
	#define GLM_FORCE_DEPTH_ZERO_TO_ONE
	#define GLM_ENABLE_EXPERIMENTAL
	#include "glm/glm/glm.hpp"
	#include "glm/glm/gtc/matrix_transform.hpp"
	#include "glm/glm/gtx/hash.hpp"
	#include "glm/glm/gtx/texture.hpp"
#endif

namespace xvk {
	
	class Loader : public xvk::Interface::LoaderInterface {
	public:
		using LoaderInterface::LoaderInterface;
	};
	
	class Instance : public xvk::Interface::InstanceInterface {
	public:
		Instance(){}
		Instance(Loader* loader, VkInstance instance) : InstanceInterface(loader, instance) {
			LoadFunctionPointers();
		}
		Instance(Loader* loader, const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator = nullptr) {
			(*this)(loader, pCreateInfo, pAllocator);
		}
		~Instance() {
			if (handle != VK_NULL_HANDLE) {
				DestroyInstance(nullptr);
				handle = VK_NULL_HANDLE;
			}
		}
		
		Instance(const Instance&) = delete;
		Instance& operator=(const Instance&) = delete;
		Instance(Instance&&) = delete;
		Instance& operator=(Instance&&) = delete;
		
		void operator()(Loader* loader, const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator = nullptr){
			this->loader = loader;
			if (loader->vkCreateInstance(pCreateInfo, pAllocator, &handle) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create Vulkan Instance");
			}
			LoadFunctionPointers();
		}
	};
	
	class Device : public xvk::Interface::DeviceInterface {
	public:
		Device(){}
		Device(Instance* instance, VkDevice device) : DeviceInterface(instance, device) {
			LoadFunctionPointers();
		}
		Device(Instance* instance, VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator = nullptr) {
			(*this)(instance, physicalDevice, pCreateInfo, pAllocator);
		}
		~Device() {
			if (handle != VK_NULL_HANDLE) {
				DeviceWaitIdle();
				DestroyDevice(nullptr);
				handle = VK_NULL_HANDLE;
			}
		}
		
		Device(const Device&) = delete;
		Device& operator=(const Device&) = delete;
		Device(Device&&) = delete;
		Device& operator=(Device&&) = delete;
		
		void operator()(Instance* instance, VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator = nullptr){
			this->instance = instance;
			if (instance->CreateDevice(physicalDevice, pCreateInfo, pAllocator, &handle) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create Vulkan Device");
				return;
			}
			LoadFunctionPointers();
		}
		
	};
	
}
