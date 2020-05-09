#pragma once // #

// 
#ifdef USE_CIMG
#include "tinyexr.h"
#define cimg_plugin "CImg/tinyexr_plugin.hpp"
//#define cimg_use_png
//#define cimg_use_jpeg
#include "CImg.h"
#endif

// 
#ifndef NSM
#define NSM api
#endif

// Currently Windows Only Supported
#if (defined(_WIN32) || defined(__MINGW32__) || defined(_MSC_VER_) || defined(__MINGW64__)) 
#include <windows.h> // Fix HMODULE Type Error
#endif

// Default Backend
#if !defined(USE_D3D12) && !defined(USE_VULKAN)
#define USE_VULKAN
#endif

// 
#ifdef ENABLE_OPENGL_INTEROP
#include <glbinding/glbinding.h>
#include <glbinding/gl/gl.h>
#endif

// 
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>
#include <xvk/xvk.hpp>

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
#include <vkh/helpers.hpp>
#include "core.hpp"


namespace vkt {
//#ifdef USE_VULKAN
    template <typename T>
    static inline auto sgn(const T& val) { return (T(0) < val) - (val < T(0)); }

    template<class T = uint64_t>
    static inline T tiled(const T& sz, const T& gmaxtile) {
        // return (int32_t)ceil((double)sz / (double)gmaxtile);
        return sz <= 0 ? 0 : (sz / gmaxtile + sgn(sz % gmaxtile));
    }

    template <class T>
    static inline auto strided(const vkt::uni_arg<size_t>& sizeo) { return sizeof(T) * sizeo; }

    // Read binary (for SPIR-V)
    // Updated 03.12.2019 (add No Data error)
    static inline auto readBinary(std::string filePath ) {
        std::ifstream file(filePath, std::ios::in | std::ios::binary | std::ios::ate);
        std::vector<uint32_t> data = {};
        if (file.is_open()) {
            std::streampos size = file.tellg();
            data.resize(tiled(uint64_t(size), uint64_t(sizeof(uint32_t))));
            file.seekg(0, std::ios::beg);
            file.read((char *)data.data(), size);
            file.close();
        }
        else {
            std::cerr << "Failure to open " + filePath << std::endl;
        };
        if (data.size() < 1u) { std::cerr << "NO DATA " + filePath << std::endl; };
        return data;
    };

    // read source (unused)
    static inline auto readSource(vkt::uni_arg<std::string> filePath, bool lineDirective = false ) {
        std::string content = "";
        std::ifstream fileStream(filePath, std::ios::in);
        if (!fileStream.is_open()) {
            std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl; return content;
        }
        std::string line = "";
        while (!fileStream.eof()) {
            std::getline(fileStream, line);
            if (lineDirective || line.find("#line") == std::string::npos) content.append(line + "\n");
        }
        fileStream.close();
        return content;
    };

    // 
    static inline auto makeShaderModuleInfo(std::vector<uint32_t> code) {
        return vkh::VkShaderModuleCreateInfo{ .codeSize = code.size(), .pCode = code.data() };
    };

    // create shader module (BROKEN FOR XVK!)
    static inline auto createShaderModuleIntrusive(vkt::uni_ptr<xvk::Device> device, std::vector<uint32_t> code, vkt::uni_ptr<VkShaderModule> hndl) {
        if (sizeof(vkh::VkShaderModuleCreateInfo) != sizeof(::VkShaderModuleCreateInfo)) {
            std::cerr << "BROKEN 'vkh::VkShaderModuleCreateInfo' STRUCTURE!" << std::endl; assert(-1);
        };
        return device->CreateShaderModule(makeShaderModuleInfo(code), nullptr, hndl.get_ptr());
    };

    // 
    static inline auto createShaderModule(vkt::uni_ptr<xvk::Device> device, std::vector<uint32_t> code) {
        vkt::uni_arg<VkShaderModule> sm = VkShaderModule{};
        createShaderModuleIntrusive(device, code, sm);
        return sm;
    }; 

    struct FixConstruction {
        FixConstruction(vkh::VkPipelineShaderStageCreateInfo spi = {}, vkh::VkPipelineShaderStageRequiredSubgroupSizeCreateInfoEXT sgmp = {}) : spi(spi), sgmp(sgmp) {
        }

        vkh::VkPipelineShaderStageCreateInfo spi = {};
        vkh::VkPipelineShaderStageRequiredSubgroupSizeCreateInfoEXT sgmp = {};

        // 
        operator vkh::VkPipelineShaderStageCreateInfo& () { return spi; };
        operator vkh::VkPipelineShaderStageRequiredSubgroupSizeCreateInfoEXT& () { return sgmp; };
        operator const vkh::VkPipelineShaderStageCreateInfo& () const { return spi; };
        operator const vkh::VkPipelineShaderStageRequiredSubgroupSizeCreateInfoEXT& () const { return sgmp; };
    };

    // create shader module
    static inline auto makePipelineStageInfo(vkt::uni_ptr<xvk::Device> device, std::vector<uint32_t> code, vkt::uni_arg<vkh::VkShaderStageFlags> stage = vkh::VkShaderStageFlags{.eCompute = 1u}, vkt::uni_arg<const char *> entry = "main") {
        vkh::VkPipelineShaderStageCreateInfo spi = {};
        createShaderModuleIntrusive(device, code, spi.module);
        spi.pName = entry;
        spi.stage = stage->c();
        spi.pSpecializationInfo = {};
        return std::move(spi);
    };

    // create shader module
    static inline auto makeComputePipelineStageInfo(vkt::uni_ptr<xvk::Device> device, std::vector<uint32_t> code, vkt::uni_arg<const char *> entry = "main", vkt::uni_arg<uint32_t> subgroupSize = 0u) {
        auto f = FixConstruction{};
        f.spi = makePipelineStageInfo(device, code, vkh::VkShaderStageFlags{.eCompute = 1u}, entry);
        f.spi.flags = vkh::VkPipelineShaderStageCreateFlags{.eRequireFullSubgroups = 1u};
        createShaderModuleIntrusive(device, code, (VkShaderModule&)(f.spi.module));
        f.sgmp = vkh::VkPipelineShaderStageRequiredSubgroupSizeCreateInfoEXT{};
        f.sgmp.requiredSubgroupSize = subgroupSize;
        if (subgroupSize) f.spi.pNext = &f.sgmp;
        return std::move(f);
    };

    // create compute pipelines
    static inline auto createCompute(vkt::uni_ptr<xvk::Device> device, vkt::uni_arg<FixConstruction> spi, vkt::uni_arg<VkPipelineLayout> layout, vkt::uni_arg<VkPipelineCache> cache = VkPipelineCache{}, vkt::uni_arg<uint32_t> subgroupSize = 0u) {
        auto cmpi = vkh::VkComputePipelineCreateInfo{};
        cmpi.flags = {};
        cmpi.layout = layout;
        cmpi.stage = *spi;
        cmpi.basePipelineIndex = -1;
        VkPipeline pipeline = {};
        device->CreateComputePipelines(cache, 1u, cmpi, nullptr, &pipeline);
        return pipeline;
    };

    // create compute pipelines
    static inline auto createCompute(vkt::uni_ptr<xvk::Device> device, std::vector<uint32_t> code, vkt::uni_arg<VkPipelineLayout> layout, vkt::uni_arg<VkPipelineCache> cache = VkPipelineCache{}, vkt::uni_arg<uint32_t> subgroupSize = 0u) {
        auto f = makeComputePipelineStageInfo(device, code, "main", subgroupSize);
        if (subgroupSize && *subgroupSize) f.spi.pNext = &f.sgmp; // fix link
        return createCompute(device, f, layout, cache, subgroupSize);
    };

    // create compute pipelines
    static inline auto createCompute(vkt::uni_ptr<xvk::Device> device, vkt::uni_arg<std::string> path, vkt::uni_arg<VkPipelineLayout> layout, vkt::uni_arg<VkPipelineCache> cache = VkPipelineCache{}, vkt::uni_arg<uint32_t> subgroupSize = 0u) {
        return createCompute(device, readBinary(path), layout, cache, subgroupSize);
    };

    // create secondary command buffers for batching compute invocations
    static inline auto createCommandBuffer(vkt::uni_ptr<xvk::Device> device, vkt::uni_arg<VkCommandPool> cmdPool, vkt::uni_arg<bool> secondary = false, vkt::uni_arg<bool> once = false) {
        VkCommandBuffer cmdBuffer = {};

        vkh::VkCommandBufferAllocateInfo cmdi = vkh::VkCommandBufferAllocateInfo{};
        cmdi.commandPool = cmdPool;
        cmdi.level = (secondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY : VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        cmdi.commandBufferCount = 1;
        //cmdBuffer = (device->allocateCommandBuffers(cmdi))[0];
        device->AllocateCommandBuffers(cmdi, &cmdBuffer);

        //VkCommandBufferInheritanceInfo inhi = VkCommandBufferInheritanceInfo{};
        //inhi.pipelineStatistics = VkQueryPipelineStatisticFlagBits::eComputeShaderInvocations;

        vkh::VkCommandBufferBeginInfo bgi = {};
        bgi.flags = once ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        //bgi.pInheritanceInfo = secondary ? &inhi : nullptr;

        device->vkBeginCommandBuffer(cmdBuffer, bgi);
        //cmdBuffer.begin(bgi);

        return cmdBuffer;
    };

    // TODO: native image barrier in library
    struct ImageBarrierInfo {
        vkt::uni_ptr<xvk::Device> device = {};
        vkt::uni_arg<VkImage> image = {};
        vkt::uni_arg<VkImageLayout> targetLayout = VK_IMAGE_LAYOUT_GENERAL;
        vkt::uni_arg<VkImageLayout> originLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        vkt::uni_arg<vkh::VkImageSubresourceRange> subresourceRange = vkh::VkImageSubresourceRange{{}, 0u, 1u, 0u, 1u};
    };

    //
    static inline auto imageBarrier(
        const vkt::uni_arg<VkCommandBuffer>& cmd = VkCommandBuffer{},
        const vkt::uni_arg<ImageBarrierInfo>& info = ImageBarrierInfo{}) {
        VkResult result = VK_SUCCESS;  // planned to complete
        if (*info->originLayout == *info->targetLayout)
            return result;  // no need transfering more

        vkh::VkImageMemoryBarrier imageMemoryBarriers = {};
        imageMemoryBarriers.srcQueueFamilyIndex = ~0U;
        imageMemoryBarriers.dstQueueFamilyIndex = ~0U;
        imageMemoryBarriers.oldLayout = info->originLayout;
        imageMemoryBarriers.newLayout = info->targetLayout;
        imageMemoryBarriers.subresourceRange = reinterpret_cast<const VkImageSubresourceRange&>(*info->subresourceRange);
        imageMemoryBarriers.image = info->image;

        // Put barrier on top
        const auto srcStageMask = vkh::VkPipelineStageFlags{.eBottomOfPipe = 1};
        const auto dstStageMask = vkh::VkPipelineStageFlags{.eTopOfPipe = 1};
        const auto dependencyFlags = vkh::VkDependencyFlags{};
        auto srcMask = vkh::VkAccessFlags{}, dstMask = vkh::VkAccessFlags{};

        typedef VkImageLayout il;
        typedef VkAccessFlagBits afb;

        // Is it me, or are these the same?
        switch (info->originLayout) {
            case VK_IMAGE_LAYOUT_UNDEFINED:                         break;
            case VK_IMAGE_LAYOUT_GENERAL:                           srcMask.eTransferWrite = 1u; break;
            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:          srcMask.eColorAttachmentWrite = 1u; break;
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:  srcMask.eDepthStencilAttachmentWrite = 1u; break;
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:   srcMask.eDepthStencilAttachmentRead = 1u; break;
            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:          srcMask.eShaderRead = 1u; break;
            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:              srcMask.eTransferRead = 1u; break;
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:              srcMask.eTransferWrite = 1u; break;
            case VK_IMAGE_LAYOUT_PREINITIALIZED:                    srcMask.eTransferWrite = 1u, srcMask.eHostWrite = 1u; break; srcMask.eMemoryRead = 1u; break;
            case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:                   srcMask.eMemoryRead = 1u; break;
        };

        // 
        switch (info->targetLayout) {
            case VK_IMAGE_LAYOUT_UNDEFINED:                         break;
            case VK_IMAGE_LAYOUT_GENERAL:                           dstMask.eTransferWrite = 1u; break;
            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:          dstMask.eColorAttachmentWrite = 1u; break;
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:  dstMask.eDepthStencilAttachmentWrite = 1u; break;
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:   dstMask.eDepthStencilAttachmentRead = 1u; break;
            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:          dstMask.eShaderRead = 1u; break;
            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:              dstMask.eTransferRead = 1u; break;
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:              dstMask.eTransferWrite = 1u; break;
            case VK_IMAGE_LAYOUT_PREINITIALIZED:                    dstMask.eTransferWrite = 1u; break;
            case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:                   dstMask.eMemoryRead = 1u; break;
        };

        // assign access masks
        imageMemoryBarriers.srcAccessMask = srcMask;
        imageMemoryBarriers.dstAccessMask = dstMask;

        // 
        info->device->vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                             VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, {}, 
                             0u, nullptr, 
                             0u, nullptr, 
                             1u, imageMemoryBarriers);

        return result;
    };

    template <class T> static inline auto makeVector(const T* ptr, const size_t& size = 1) { std::vector<T>v(size); memcpy(v.data(), ptr, strided<T>(size)); return v; };

    template<class T, class Ty = T>
    std::vector<T> vector_cast(const std::vector<Ty>& Vy) {
        std::vector<T> V{}; for (auto& v : Vy) { V.push_back(reinterpret_cast<const T&>(v)); }; return std::move(V);
    };

    // general command buffer pipeline barrier (updated 26.04.2020)
    static inline void commandBarrier(vkt::uni_ptr<xvk::Device> device, vkt::uni_arg<VkCommandBuffer> cmdBuffer) {
        vkh::VkMemoryBarrier memoryBarrier = {};
        memoryBarrier.srcAccessMask = { .eShaderWrite = 1,                   .eColorAttachmentWrite = 1, .eDepthStencilAttachmentWrite = 1, .eTransferWrite = 1, .eHostWrite = 1, .eMemoryWrite = 1, .eCommandProcessWrite = 1, .eAccelerationStructureWrite = 1, .eTransformFeedbackWrite = 1, .eTransformFeedbackCounterWrite = 1 };
        memoryBarrier.dstAccessMask = { .eUniformRead = 1, .eShaderRead = 1, .eColorAttachmentRead  = 1, .eDepthStencilAttachmentRead  = 1, .eTransferRead  = 1, .eHostRead  = 1, .eMemoryRead  = 1, .eCommandProcessRead  = 1, .eAccelerationStructureRead  = 1,                               .eTransformFeedbackCounterRead  = 1 };
        const vkh::VkPipelineStageFlags srcStageMask = { .eVertexShader = 1, .eTessellationEvaluationShader = 1, .eGeometryShader = 1, .eFragmentShader = 1, .eColorAttachmentOutput = 1, .eComputeShader = 1, .eTransfer = 1, .eHost = 1, .eRayTracingShader = 1, .eTransformFeedback = 1, .eAccelerationStructureBuild = 1 };
        const vkh::VkPipelineStageFlags dstStageMask = { .eVertexInput  = 1, .eTessellationControlShader    = 1, .eGeometryShader = 1, .eFragmentShader = 1,                              .eComputeShader = 1, .eTransfer = 1, .eHost = 1, .eRayTracingShader = 1, .eTransformFeedback = 1, .eAccelerationStructureBuild = 1 };
        device->vkCmdPipelineBarrier(cmdBuffer, srcStageMask, dstStageMask, {},
                1u, memoryBarrier,
                0u, nullptr,
                0u, nullptr);
        //vkCmdPipelineBarrier(cmdBuffer, srcStageMask, dstStageMask, {},
        //    1u, memoryBarrier,
        //    0u, nullptr,
        //    0u, nullptr);
    };


    // create fence function
    static inline auto createFence(vkt::uni_ptr<xvk::Device> device, const vkt::uni_arg<bool>& signaled = true) {
        VkFenceCreateInfo info = {};
        info.flags = signaled & 1;
        VkFence fence = {}; device->CreateFence(vkh::VkFenceCreateInfo{}, nullptr, &fence);//vkCreateFence(device, vkh::VkFenceCreateInfo{}, nullptr, &fence);
        return fence;
    };

    // submit command (with async wait)
    static inline auto submitCmd(vkt::uni_ptr<xvk::Device> device, const vkt::uni_arg<VkQueue>& queue, const std::vector<VkCommandBuffer>& cmds, vkt::uni_arg<vkh::VkSubmitInfo> smbi = vkh::VkSubmitInfo{}) {
        // no commands 
        if (cmds.size() <= 0) return;
        smbi->commandBufferCount = static_cast<uint32_t>(cmds.size());
        smbi->pCommandBuffers = (VkCommandBuffer*)cmds.data();

        VkFence fence = createFence(device, false);
        device->QueueSubmit(queue, 1u, *smbi, fence);
        device->WaitForFences(1u, &fence, true, 30ull * 1000ull * 1000ull * 1000ull);
        device->DestroyFence(fence, nullptr);

        return;
    };

    // once submit command buffer
    // TODO: return VkResult
    static inline auto submitOnce(vkt::uni_ptr<xvk::Device> device, vkt::uni_arg<VkQueue> queue, const vkt::uni_arg<VkCommandPool>& cmdPool, const std::function<void(VkCommandBuffer&)>& cmdFn = {}, const vkt::uni_arg<vkh::VkSubmitInfo>& smbi = vkh::VkSubmitInfo{}) {
        auto cmdBuf = createCommandBuffer(device, cmdPool, false); cmdFn(cmdBuf); vkEndCommandBuffer(cmdBuf);
        submitCmd(device, queue, { cmdBuf }); device->FreeCommandBuffers(cmdPool, 1u, &cmdBuf);
    };

    // submit command (with async wait)
    // TODO: return VkResult
    static inline auto submitCmdAsync(vkt::uni_ptr<xvk::Device> device, vkt::uni_arg<VkQueue> queue, const std::vector<VkCommandBuffer>& cmds, const vkt::uni_arg<vkh::VkSubmitInfo>& smbi = vkh::VkSubmitInfo{}) {
        return std::async(std::launch::async | std::launch::deferred, [=]() {
            return submitCmd(device, queue, cmds, smbi);
        });
    };

    // once submit command buffer
    // TODO: return VkResult
    static inline auto submitOnceAsync(vkt::uni_ptr<xvk::Device> device, vkt::uni_arg<VkQueue> queue, const vkt::uni_arg<VkCommandPool>& cmdPool, const std::function<void(VkCommandBuffer&)>& cmdFn = {}, const vkt::uni_arg<vkh::VkSubmitInfo>& smbi = vkh::VkSubmitInfo{}) {
        VkCommandBuffer cmdBuf = createCommandBuffer(device, cmdPool, false); cmdFn(cmdBuf); vkEndCommandBuffer(cmdBuf);
        return std::async(std::launch::async | std::launch::deferred, [&]() {
            submitCmdAsync(device, queue, { cmdBuf }, smbi).get();
            device->FreeCommandBuffers(cmdPool, 1u, &cmdBuf);
        });
    };

    // 
#ifdef VULKAN_HPP
    template<class T>
    T handleHpp(vk::ResultValue<T> V) {
        assert(V.result == VkResult::eSuccess);
        return std::move(V.value);
    };
#endif

    // Global initials
    class vkGlobal { public: // Currently, only Vulkan Loader
        static inline bool initialized = false;
        static inline vkt::uni_ptr<xvk::Loader> loader = {};
        vkGlobal() { 
            if (!initialized) {
                loader = std::make_shared<xvk::Loader>();
                if (!(initialized = (*loader)())) { std::cerr << "vulkan load failed..." << std::endl; }; 
            };
        };
    };

    // TODO: Add XVK support
    struct MemoryAllocationInfo { // 
        uint32_t glMemory = 0u, glID = 0u;
        std::vector<uint32_t> queueFamilyIndices = {};
        
        // 
        vkt::uni_ptr<xvk::Instance> instanceDispatch = {};
        vkt::uni_ptr<xvk::Device> deviceDispatch = {};
        //vkt::uni_ptr<xvk::Loader> loader = {};

        // Required for dispatch load (and for XVK)
        VkInstance instance = {};
        VkPhysicalDevice physicalDevice = {};
        VkDevice device = {};

        // 
        VkDeviceMemory memory = {};
        VkDeviceSize offset = 0ull;
        VkDeviceSize range = 0ull;
        VkDeviceSize reqSize = 0ull;
        VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        //VkDispatchLoaderDynamic dispatch = {};

        HANDLE handle = {};
        void* pMapped = nullptr;
        vkh::VkPhysicalDeviceMemoryProperties memoryProperties = {};
        VmaMemoryUsage memUsage = VMA_MEMORY_USAGE_GPU_ONLY;

        // 
        int32_t getMemoryType(const uint32_t& memoryTypeBitsRequirement, const vkh::VkMemoryPropertyFlags& requiredProperties = { .eDeviceLocal = 1 }) const {
            const uint32_t memoryCount = memoryProperties.memoryTypeCount;
            for (uint32_t memoryIndex = 0; memoryIndex < memoryCount; ++memoryIndex) {
                const uint32_t memoryTypeBits = (1 << memoryIndex);
                const bool isRequiredMemoryType = memoryTypeBitsRequirement & memoryTypeBits;
                const auto properties = VkMemoryPropertyFlags(memoryProperties.memoryTypes[memoryIndex].propertyFlags);
                const bool hasRequiredProperties = (properties & VkMemoryPropertyFlags(requiredProperties)) == VkMemoryPropertyFlags(requiredProperties);
                if (isRequiredMemoryType && hasRequiredProperties) return static_cast<int32_t>(memoryIndex);
            }
            return -1;
        }
    };

    // 
    VkResult AllocateDescriptorSetWithUpdate(vkt::uni_ptr<xvk::Device>& device, vkh::VsDescriptorSetCreateInfoHelper& helper, VkDescriptorSet& descriptorSet) {
        if (!descriptorSet) { device->AllocateDescriptorSets(helper, &descriptorSet); };
        device->UpdateDescriptorSets(1u, helper.setDescriptorSet(descriptorSet).mapWriteDescriptorSet()[0], 0u, {});
        return VK_SUCCESS;
    };




    #ifdef TBA_VULKAN_HPP_DEPRECATED
    // add dispatch in command buffer (with default pipeline barrier)
    static inline auto cmdDispatch(const vkt::uni_arg<VkCommandBuffer>& cmd, const vkt::uni_arg<VkPipeline>& pipeline, const vkt::uni_arg<uint32_t>& x = 1u, const vkt::uni_arg<uint32_t>& y = 1u, const vkt::uni_arg<uint32_t>& z = 1u, const vkt::uni_arg<bool>& barrier = true) {
        cmd->bindPipeline(VkPipelineBindPoint::eCompute, pipeline);
        cmd->dispatch(x, y, z);
        if (barrier) {
            commandBarrier(cmd); // put shader barrier
        }
        return VkResult::eSuccess;
    };

    // low level copy command between (prefer for host and device)
    static inline auto cmdCopyBufferL(const vkt::uni_arg<VkCommandBuffer>& cmd, const vkt::uni_arg<VkBuffer>& srcBuffer, const vkt::uni_arg<VkBuffer>& dstBuffer, const std::vector<VkBufferCopy>& regions, std::function<void(VkCommandBuffer)> barrierFn = commandBarrier) {
        if (srcBuffer && dstBuffer && regions.size() > 0) {
            VkCommandBuffer(cmd).copyBuffer(srcBuffer, dstBuffer, regions); barrierFn(cmd); // put copy barrier
        };
        return VkResult::eSuccess;
    };


    // short data set with command buffer (alike push constant)
    template<class T>
    static inline auto cmdUpdateBuffer(const vkt::uni_arg<VkCommandBuffer>& cmd, const vkt::uni_arg<VkBuffer>& dstBuffer, const vkt::uni_arg<VkDeviceSize>& offset, const std::vector<T>& data) {
        VkCommandBuffer(cmd).updateBuffer(dstBuffer, offset, data);
        //updateCommandBarrier(cmd);
        return VkResult::eSuccess;
    };

    // short data set with command buffer (alike push constant)
    template<class T>
    static inline auto cmdUpdateBuffer(const vkt::uni_arg<VkCommandBuffer>& cmd, const vkt::uni_arg<VkBuffer>& dstBuffer, const vkt::uni_arg<VkDeviceSize>& offset, const vkt::uni_arg<VkDeviceSize>& size, const vkt::uni_arg<T*> data = nullptr) {
        VkCommandBuffer(cmd).updateBuffer(dstBuffer, offset, size, data);
        //updateCommandBarrier(cmd);
        return VkResult::eSuccess;
    };

    // template function for fill buffer by constant value
    // use for create repeat variant
    template<uint32_t Rv>
    static inline auto cmdFillBuffer(const vkt::uni_arg<VkCommandBuffer>& cmd, const vkt::uni_arg<VkBuffer>& dstBuffer, const vkt::uni_arg<VkDeviceSize>& size = 0xFFFFFFFFull, const vkt::uni_arg<VkDeviceSize>& offset = 0ull) {
        VkCommandBuffer(cmd).fillBuffer(VkBuffer(dstBuffer), offset, size, Rv);
        //updateCommandBarrier(cmd);
        return VkResult::eSuccess;
    };

    void debugLabel(const VkCommandBuffer& buildCommand, const std::string& labelName = "", const VkDispatchLoaderDynamic& dispatch = {}) {
#ifdef VKT_ENABLE_DEBUG_MARK
        buildCommand.insertDebugUtilsLabelEXT(VkDebugUtilsLabelEXT().setColor({ 1.f,0.75,0.25f }).setPLabelName(labelName.c_str()), dispatch);
        buildCommand.setCheckpointNV(labelName.c_str(), dispatch);
#endif
    };
    #endif


//#endif
};