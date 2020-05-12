﻿#pragma once // #

// 
#ifdef USE_CIMG
#include "tinyexr.h"
#define cimg_plugin "CImg/tinyexr_plugin.hpp"
//#define cimg_use_png
//#define cimg_use_jpeg
#include "CImg.h"
#endif

// 
#include <misc/args.hxx>
#include <misc/half.hpp>
#include <misc/pcg_random.hpp>

// 
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vec_swizzle.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>

// 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 
#include <iomanip>
#include <cmath>
#include <cfenv>
#include <ios>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <functional>
#include <memory>
#include <vector>
#include <thread>
#include <atomic>
#include <future>

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
        auto gVector = vkt::Vector<Tm>(std::make_shared<vkt::VmaBufferAllocation>(allocator, vkh::VkBufferCreateInfo{
            .size = mVector.range(),
            .usage = {.eTransferSrc = 1, .eStorageTexelBuffer = 1, .eStorageBuffer = 1, .eIndexBuffer = 1, .eVertexBuffer = 1, .eTransformFeedbackBuffer = 1 },
            }, vkt::VmaMemoryInfo{ .memUsage = VMA_MEMORY_USAGE_GPU_TO_CPU, .deviceDispatch = vkt::vkGlobal::device, .instanceDispatch = vkt::vkGlobal::instance }));

        vkt::submitOnce(vkt::vkGlobal::device, queue, commandPool, [&](const VkCommandBuffer& cmd) {
            (vkt::vkGlobal::device)->CmdCopyBuffer(cmd, mVector.buffer(), gVector.buffer(), 1u, vkh::VkBufferCopy{ .srcOffset = mVector.offset(), .dstOffset = gVector.offset(), .size = mVector.range() });
        });

        auto vect = std::vector<Tm>(mVector.size());
        memcpy(vect.data(), gVector.data(), gVector.range());
        return vect;
    };

    
    // 
    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    };

    // 
    VkResult CreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pCallback) {

        // Note: It seems that static_cast<...> doesn't work. Use the C-style forced cast.
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pCallback);
        }
        else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    };

    // 
    std::vector<const char*> GetRequiredExtensions() {
#ifdef VKT_ENABLE_GLFW_SUPPORT
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
    VkResult AllocateDescriptorSetWithUpdate(vkt::uni_ptr<xvk::Device>& device, vkh::VsDescriptorSetCreateInfoHelper& helper, VkDescriptorSet& descriptorSet, bool& protection) {
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
