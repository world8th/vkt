#pragma once // #

//
#ifndef VKT_CORE_USE_VMA
#define VKT_CORE_USE_VMA
#endif

// 
#include <vkt/core.hpp>
#include <vkt/inline.hpp>

// 
namespace vkf {

    // 
    struct VmaMemoryInfo {
        VmaMemoryUsage memUsage = VMA_MEMORY_USAGE_GPU_ONLY;
        std::shared_ptr<xvk::Instance> instanceDispatch = vkt::vkGlobal::instance;
        std::shared_ptr<xvk::Device> deviceDispatch = vkt::vkGlobal::device;

        // Resolve JavaCPP Problem...
        VmaMemoryInfo& setDeviceDispatch(std::shared_ptr<xvk::Device> ptr) { deviceDispatch = ptr; return *this; };
        VmaMemoryInfo& setInstanceDispatch(std::shared_ptr<xvk::Instance> ptr) { instanceDispatch = ptr; return *this; };

        // Resolve JavaCPP Problem...
        std::shared_ptr<xvk::Device>& getDeviceDispatch() { return deviceDispatch; };
        std::shared_ptr<xvk::Instance>& getInstanceDispatch() { return instanceDispatch; };
    };

    // TODO: Add XVK support
    struct MemoryAllocationInfo { //
        uint32_t glID = 0u, glMemory = 0u;      // 0U
#ifdef VKT_WIN32_DETECTED
        HANDLE handle = {};                     // 8U
#endif

        // Required for dispatch load (and for XVK)
        VkInstance instance = {};               // 16U
        VkPhysicalDevice physicalDevice = {};   // 24U
        VkDevice device = {};                   // 32U
        void* pMapped = nullptr;                // 40U

        // 
        VkDeviceMemory memory = {};             // 48U
        VkDeviceSize offset = 0ull;             // 56U
        VkDeviceSize range = 0ull;              // 64U
        VkDeviceSize reqSize = 0ull;            // 72U

        // 
        VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;    // 80U
        VmaMemoryUsage memUsage = VMA_MEMORY_USAGE_GPU_ONLY;        // 88U

        // 
        vkh::VkPhysicalDeviceMemoryProperties memoryProperties = {};

        // 
        std::vector<uint32_t> queueFamilyIndices = {};
        vkt::Instance instanceDispatch = vkt::vkGlobal::instance;
        vkt::Device deviceDispatch = vkt::vkGlobal::device;

        // 
        uint32_t getMemoryType(const uint32_t& memoryTypeBitsRequirement, const vkh::VkMemoryPropertyFlags& requiredProperties = { .eDeviceLocal = 1 }) const {
            const uint32_t memoryCount = memoryProperties.memoryTypeCount;
            for (uint32_t memoryIndex = 0; memoryIndex < memoryCount; ++memoryIndex) {
                const uint32_t memoryTypeBits = (1 << memoryIndex);
                const bool isRequiredMemoryType = memoryTypeBitsRequirement & memoryTypeBits;
                const auto properties = VkMemoryPropertyFlags(memoryProperties.memoryTypes[memoryIndex].propertyFlags);
                const bool hasRequiredProperties = (properties & VkMemoryPropertyFlags(requiredProperties)) == VkMemoryPropertyFlags(requiredProperties);
                if (isRequiredMemoryType && hasRequiredProperties) return static_cast<uint32_t>(memoryIndex);
            };
            return 0u;
        }
    };

};
