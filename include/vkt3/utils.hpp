#pragma once // #

// 
#include "essential.hpp"
#include "vector.hpp"
#include "image.hpp"

// 
namespace vkt {

    // Новый, инновационный, педиковатый интерфейс... 
    //using Tm = uint8_t;
    template<class Tm = uint8_t>
    static inline std::vector<Tm> DebugBufferData(vkt::uni_arg<VmaAllocator> allocator, vkt::uni_arg<VkQueue> queue, vkt::uni_arg<VkCommandPool> commandPool, vkt::Vector<Tm> mVector) {
        auto usage = vkh::VkBufferUsageFlags{.eTransferSrc = 1, .eStorageTexelBuffer = 1, .eStorageBuffer = 1, .eIndexBuffer = 1, .eVertexBuffer = 1, .eTransformFeedbackBuffer = 1 };
        auto flags = vkh::VkBufferCreateFlags{}; vkt::unlock32(flags) = 0u;
        auto gVector = vkt::Vector<Tm>(std::make_shared<vkt::VmaBufferAllocation>(allocator, vkh::VkBufferCreateInfo{.flags = flags, .size = uint64_t(mVector.range()), .usage = usage,
        }, vkt::VmaMemoryInfo{ .memUsage = VMA_MEMORY_USAGE_GPU_TO_CPU, .instanceDispatch = vkt::vkGlobal::instance, .deviceDispatch = vkt::vkGlobal::device }));

        vkt::submitOnce(vkt::vkGlobal::device, queue, commandPool, [&](const VkCommandBuffer& cmd) {
            (vkt::vkGlobal::device)->CmdCopyBuffer(cmd, mVector.buffer(), gVector.buffer(), 1u, vkh::VkBufferCopy{ .srcOffset = mVector.offset(), .dstOffset = gVector.offset(), .size = mVector.range() });
        });

        auto vect = std::vector<Tm>(mVector.size());
        memcpy(vect.data(), gVector.data(), gVector.range());
        return vect;
    };

    // 
    inline VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    };

    // 
    inline VkResult CreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pCallback) {
#ifdef VKT_VULKAN_DEBUG
        // Note: It seems that static_cast<...> doesn't work. Use the C-style forced cast.
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pCallback);
        }
        else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
#else
        return VK_SUCCESS;
#endif
    };

    // 
    inline std::vector<const char*> GetRequiredExtensions() {
#ifdef VKT_ENABLE_GLFW_LINKED
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = nullptr;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
#else
        std::vector<const char*> extensions{};
#endif

        // also want the "debug utils"
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        return extensions;
    };

    // 
    inline VkResult AllocateDescriptorSetWithUpdate(vkt::uni_ptr<xvk::Device>& device, vkh::VsDescriptorSetCreateInfoHelper& helper, VkDescriptorSet& descriptorSet, bool& protection) {
        if (!protection) {
            // Corrupt... 
            if (descriptorSet) { vkh::handleVk(device->FreeDescriptorSets(helper, 1u, &descriptorSet)); descriptorSet = {}; };

            // 
            bool created = false;
            if (!descriptorSet) { vkh::handleVk(device->AllocateDescriptorSets(helper, &descriptorSet)); created = true; };

            //
            if (descriptorSet && created) {
                const auto writes = helper.setDescriptorSet(descriptorSet).mapWriteDescriptorSet();
                device->UpdateDescriptorSets(writes.size(), reinterpret_cast<const VkWriteDescriptorSet*>(writes.data()), 0u, nullptr);
            };

            protection = true;
        };

        // 
        return VK_SUCCESS;
    };

    

};
