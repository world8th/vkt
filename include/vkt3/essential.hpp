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
//#include <misc/args.hxx>
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
#include <vkh/core.hpp>
#include <vkh/structures.hpp>
#include <vkh/helpers.hpp>

// 
#ifdef ENABLE_OPTIX_DENOISE
#include <cuda.h>
#include <cuda_runtime.h>
#include <optix/optix.h>
#include <optix/optix_stubs.h>
#include <optix/optix_types.h>
#endif

// 
namespace vkt {
#ifdef ENABLE_OPENGL_INTEROP
#ifndef VKT_USE_GLAD
    using namespace gl;
#endif
#endif

#ifdef ENABLE_OPTIX_DENOISE
#define OPTIX_CHECK(call)                                                      \
  do {                                                                         \
    OptixResult res = call;                                                    \
    if (res != OPTIX_SUCCESS) {                                                \
      std::stringstream ss;                                                    \
      ss << "Optix call (" << #call << " ) failed with code " << res           \
         << " (" __FILE__ << ":" << __LINE__ << ")\n";                         \
      std::cerr << ss.str().c_str() << std::endl;                              \
      throw std::runtime_error(ss.str().c_str());                              \
    }                                                                          \
  } while (false)

#define CUDA_CHECK(call)                                                       \
  do {                                                                         \
    cudaError_t error = call;                                                  \
    if (error != cudaSuccess) {                                                \
      std::stringstream ss;                                                    \
      ss << "CUDA call (" << #call << " ) failed with code " << error          \
         << " (" __FILE__ << ":" << __LINE__ << ")\n";                         \
      throw std::runtime_error(ss.str().c_str());                              \
    }                                                                          \
  } while (false)

#define OPTIX_CHECK_LOG(call)                                                  \
  do {                                                                         \
    OptixResult res = call;                                                    \
    if (res != OPTIX_SUCCESS) {                                                \
      std::stringstream ss;                                                    \
      ss << "Optix call (" << #call << " ) failed with code " << res           \
         << " (" __FILE__ << ":" << __LINE__ << ")\nLog:\n"                    \
         << log << "\n";                                                       \
      throw std::runtime_error(ss.str().c_str());                              \
    }                                                                          \
  } while (false)

    static void context_log_cb(unsigned int level, const char* tag,
        const char* message, void* /*cbdata */) {
        std::cerr << "[" << std::setw(2) << level << "][" << std::setw(12) << tag
            << "]: " << message << "\n";
    }
#endif

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

    // 
    typedef uint8_t BYTE;

    // Read binary (for SPIR-V)
    // Updated 10.05.2020
    inline std::vector<BYTE> readBinaryU8(vkt::uni_arg<std::string> filePath) { // open the file:
        std::vector<BYTE> vec{};
        std::ifstream file(filePath, std::ios::in | std::ios::binary | std::ios::ate);
        if (file.is_open()) { // Stop eating new lines in binary mode!!!
            file.unsetf(std::ios::skipws);

            // get its size:
            std::streampos fileSize;
            file.seekg(0, std::ios::end);
            fileSize = file.tellg();
            file.seekg(0, std::ios::beg);

            // reserve capacity
            vec.reserve(fileSize);
            vec.insert(vec.begin(), std::istream_iterator<BYTE>(file), std::istream_iterator<BYTE>());
        }
        else {
            std::cerr << "Failure to open " + *filePath << std::endl;
        };
        if (vec.size() < 1u) { std::cerr << "NO DATA " + *filePath << std::endl; };
        return vec;
    };

    // 
    static inline auto readBinary(vkt::uni_arg<std::string> filePath) {
        const auto vect8u = readBinaryU8(filePath);
        auto vect32 = std::vector<uint32_t>(tiled(uint64_t(vect8u.size()), uint64_t(4ull)));
        memcpy(vect32.data(), vect8u.data(), vect8u.size()); return vect32;
    };

    // read source (unused)
    static inline auto readSource(vkt::uni_arg<std::string> filePath, bool lineDirective = false) {
        std::string content = "";
        std::ifstream fileStream(filePath, std::ios::in);
        if (!fileStream.is_open()) {
            std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl; return content;
        };
        std::string line = "";
        while (!fileStream.eof()) {
            std::getline(fileStream, line);
            if (lineDirective || line.find("#line") == std::string::npos) content.append(line + "\n");
        };
        fileStream.close();
        return content;
    };




    // Used for directly create shader from code! (Prevent Code LOST!)
    inline std::vector<uint32_t> TempCode = {};

    // 
    static inline auto makeShaderModuleInfo(const std::vector<uint32_t>& code) {
        return vkh::VkShaderModuleCreateInfo{ .codeSize = code.size() * 4ull, .pCode = code.data() };
    };

    // create shader module (BROKEN FOR XVK!)
    static inline auto createShaderModuleIntrusive(vkt::uni_ptr<xvk::Device> device, const std::vector<uint32_t>& code, vkt::uni_ptr<VkShaderModule> hndl) {
        if (sizeof(vkh::VkShaderModuleCreateInfo) != sizeof(::VkShaderModuleCreateInfo)) {
            std::cerr << "BROKEN 'vkh::VkShaderModuleCreateInfo' STRUCTURE!" << std::endl; assert(-1);
        };
        vkh::handleVk(device->CreateShaderModule(makeShaderModuleInfo(TempCode = code), nullptr, hndl.get_ptr()));
        return hndl;
    };

    // 
    static inline auto createShaderModule(vkt::uni_ptr<xvk::Device> device, const std::vector<uint32_t>& code) {
        vkt::uni_arg<VkShaderModule> sm = VkShaderModule{};
        createShaderModuleIntrusive(device, TempCode = code, sm);
        return sm;
    };

    // 
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
    static inline auto makePipelineStageInfo(vkt::uni_ptr<xvk::Device> device, const std::vector<uint32_t>& code, vkt::uni_arg<VkShaderStageFlagBits> stage = VK_SHADER_STAGE_COMPUTE_BIT, vkt::uni_arg<const char*> entry = "main") {
        vkh::VkPipelineShaderStageCreateInfo spi = {};
#ifndef VKT_USE_CPP_MODULES
        createShaderModuleIntrusive(device, code, spi.module);
#else
        createShaderModuleIntrusive(device, code, spi.modular);
#endif
        spi.pName = entry;
        spi.stage = stage;
        spi.pSpecializationInfo = {};
        return std::move(spi);
    };

    // create shader module 
    static inline auto makePipelineStageInfoWithoutModule(vkt::uni_ptr<xvk::Device> device, vkt::uni_arg<VkShaderStageFlagBits> stage = VK_SHADER_STAGE_COMPUTE_BIT, vkt::uni_arg<const char*> entry = "main") {
        vkh::VkPipelineShaderStageCreateInfo spi = {};
        spi.pName = entry;
        spi.stage = stage;
        spi.pSpecializationInfo = {};
        return std::move(spi);
    };

    // create shader module
    static inline auto makeComputePipelineStageInfo(vkt::uni_ptr<xvk::Device> device, const std::vector<uint32_t>& code, vkt::uni_arg<const char*> entry = "main", vkt::uni_arg<uint32_t> subgroupSize = 0u) {
        auto f = FixConstruction{};
        auto fl = vkh::VkPipelineShaderStageCreateFlags{ .eRequireFullSubgroups = 1u };
        f.spi = makePipelineStageInfoWithoutModule(device, VK_SHADER_STAGE_COMPUTE_BIT, entry);
        f.spi.flags = fl;
#ifndef VKT_USE_CPP_MODULES
        createShaderModuleIntrusive(device, TempCode = code, (VkShaderModule&)(f.spi.module));
#else
        createShaderModuleIntrusive(device, TempCode = code, (VkShaderModule&)(f.spi.modular));
#endif
        f.sgmp = vkh::VkPipelineShaderStageRequiredSubgroupSizeCreateInfoEXT{};
        f.sgmp.requiredSubgroupSize = subgroupSize;
        if (subgroupSize) f.spi.pNext = &f.sgmp;
        return std::move(f);
    };

    // create compute pipelines
    static inline auto createCompute(vkt::uni_ptr<xvk::Device> device, vkt::uni_arg<FixConstruction> spi, vkt::uni_arg<VkPipelineLayout> layout, vkt::uni_arg<VkPipelineCache> cache = VkPipelineCache{}, vkt::uni_arg<uint32_t> subgroupSize = 0u) {
        auto cmpi = vkh::VkComputePipelineCreateInfo{};
        auto fl = vkh::VkPipelineCreateFlags{}; vkt::unlock32(fl) = 0u;
        cmpi.flags = fl;
        cmpi.layout = layout;
        cmpi.stage = *spi;
        cmpi.basePipelineIndex = -1;
        VkPipeline pipeline = {};
        vkh::handleVk(device->CreateComputePipelines(cache, 1u, cmpi, nullptr, &pipeline));
        return pipeline;
    };

    // create compute pipelines
    static inline auto createCompute(vkt::uni_ptr<xvk::Device> device, const std::vector<uint32_t>& code, vkt::uni_arg<VkPipelineLayout> layout, vkt::uni_arg<VkPipelineCache> cache = VkPipelineCache{}, vkt::uni_arg<uint32_t> subgroupSize = 0u) {
        auto f = makeComputePipelineStageInfo(device, TempCode = code, "main", subgroupSize);
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
        vkh::handleVk(device->AllocateCommandBuffers(cmdi, &cmdBuffer));

        //VkCommandBufferInheritanceInfo inhi = VkCommandBufferInheritanceInfo{};
        //inhi.pipelineStatistics = VkQueryPipelineStatisticFlagBits::eComputeShaderInvocations;

        vkh::VkCommandBufferBeginInfo bgi = {};
        bgi.flags = once ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        //bgi.pInheritanceInfo = secondary ? &inhi : nullptr;

        vkh::handleVk(device->vkBeginCommandBuffer(cmdBuffer, bgi));
        //cmdBuffer.begin(bgi);

        return cmdBuffer;
    };

    template <class T> static inline auto makeVector(const T* ptr, const size_t& size = 1) { std::vector<T>v(size); memcpy(v.data(), ptr, strided<T>(size)); return v; };

    template<class T, class Ty = T>
    std::vector<T> vector_cast(const std::vector<Ty>& Vy) {
        std::vector<T> V{}; for (auto& v : Vy) { V.push_back(reinterpret_cast<const T&>(v)); }; return std::move(V);
    };

    /* // general command buffer pipeline barrier (updated 26.04.2020)
    static inline void commandBarrier(const vkt::uni_arg<vk::CommandBuffer>& cmdBuffer) {
        vk::MemoryBarrier memoryBarrier = {};
        memoryBarrier.srcAccessMask = (vk::AccessFlagBits::eHostWrite | vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eAccelerationStructureWriteNV | vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eTransferWrite | vk::AccessFlagBits::eTransformFeedbackCounterWriteEXT | vk::AccessFlagBits::eMemoryWrite | vk::AccessFlagBits::eTransformFeedbackWriteEXT);
        memoryBarrier.dstAccessMask = (vk::AccessFlagBits::eHostRead  | vk::AccessFlagBits::eColorAttachmentRead  | vk::AccessFlagBits::eAccelerationStructureReadNV  | vk::AccessFlagBits::eShaderRead  | vk::AccessFlagBits::eTransferRead  | vk::AccessFlagBits::eTransformFeedbackCounterReadEXT  | vk::AccessFlagBits::eMemoryRead  | vk::AccessFlagBits::eUniformRead);
        const auto srcStageMask = vk::PipelineStageFlagBits::eTransformFeedbackEXT | vk::PipelineStageFlagBits::eTransfer | vk::PipelineStageFlagBits::eComputeShader | vk::PipelineStageFlagBits::eRayTracingShaderNV | vk::PipelineStageFlagBits::eAccelerationStructureBuildNV | vk::PipelineStageFlagBits::eHost | vk::PipelineStageFlagBits::eAllGraphics | vk::PipelineStageFlagBits::eColorAttachmentOutput;// | vk::PipelineStageFlagBits::eBottomOfPipe;
        const auto dstStageMask = vk::PipelineStageFlagBits::eTransformFeedbackEXT | vk::PipelineStageFlagBits::eTransfer | vk::PipelineStageFlagBits::eComputeShader | vk::PipelineStageFlagBits::eRayTracingShaderNV | vk::PipelineStageFlagBits::eAccelerationStructureBuildNV | vk::PipelineStageFlagBits::eHost | vk::PipelineStageFlagBits::eAllGraphics | vk::PipelineStageFlagBits::eColorAttachmentOutput;// | vk::PipelineStageFlagBits::eTopOfPipe;
        cmdBuffer->pipelineBarrier(srcStageMask, dstStageMask, {}, { memoryBarrier }, {}, {});
    };
    */

    // general command buffer pipeline barrier (updated 26.04.2020)
    static inline void commandBarrier(vkt::uni_ptr<xvk::Device> device, vkt::uni_arg<VkCommandBuffer> cmdBuffer) {
        vkh::VkMemoryBarrier memoryBarrier = {};
        vkh::VkPipelineStageFlags srcStageMask = {};
        vkh::VkPipelineStageFlags dstStageMask = {};

        //memoryBarrier.srcAccessMask = { .eShaderWrite = 1,                   .eColorAttachmentWrite = 1, .eDepthStencilAttachmentWrite = 1, .eTransferWrite = 1, .eHostWrite = 1, .eMemoryWrite = 1, .eCommandProcessWrite = 1, .eAccelerationStructureWrite = 1, .eTransformFeedbackWrite = 1, .eTransformFeedbackCounterWrite = 1 };
        //memoryBarrier.dstAccessMask = { .eUniformRead = 1, .eShaderRead = 1, .eColorAttachmentRead  = 1, .eDepthStencilAttachmentRead  = 1, .eTransferRead  = 1, .eHostRead  = 1, .eMemoryRead  = 1, .eCommandProcessRead  = 1, .eAccelerationStructureRead  = 1,                               .eTransformFeedbackCounterRead  = 1 };
        //const vkh::VkPipelineStageFlags srcStageMask = { .eVertexInput = 1, .eVertexShader = 1, .eGeometryShader = 1, .eFragmentShader = 1, .eColorAttachmentOutput = 1, .eComputeShader = 1, .eTransfer = 1, .eHost = 1, .eAllGraphics = 1, .eRayTracingShader = 1, .eTransformFeedback = 1, .eAccelerationStructureBuild = 1 };
        //const vkh::VkPipelineStageFlags dstStageMask = { .eVertexInput = 1, .eVertexShader = 1, .eGeometryShader = 1, .eFragmentShader = 1, .eColorAttachmentOutput = 1, .eComputeShader = 1, .eTransfer = 1, .eHost = 1, .eAllGraphics = 1, .eRayTracingShader = 1, .eTransformFeedback = 1, .eAccelerationStructureBuild = 1 };

        // 
        srcStageMask = VkPipelineStageFlagBits(VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT | VK_PIPELINE_STAGE_HOST_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_VERTEX_INPUT_BIT | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR | VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR);
        dstStageMask = srcStageMask;

        //
        //srcStageMask.eBottomOfPipe = 1;
        //dstStageMask.eTopOfPipe = 1;

        // 
        memoryBarrier.srcAccessMask = VkAccessFlagBits(VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT | VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR | VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT);
        memoryBarrier.dstAccessMask = VkAccessFlagBits(VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_MEMORY_READ_BIT  /*| VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT*/ | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT | VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_UNIFORM_READ_BIT);
        //memoryBarrier.srcAccessMask = memoryBarrier.srcAccessMask | memoryBarrier.dstAccessMask;

        // 
        (device->vkCmdPipelineBarrier(cmdBuffer, srcStageMask, dstStageMask, {},
            1u, memoryBarrier,
            0u, nullptr,
            0u, nullptr));
    };

    // create fence function
    static inline auto createFence(vkt::uni_ptr<xvk::Device> device, const vkt::uni_arg<bool>& signaled = true) {
        VkFenceCreateInfo info = {};
        info.flags = signaled & 1;
        VkFence fence = {}; vkh::handleVk(device->CreateFence(vkh::VkFenceCreateInfo{}, nullptr, &fence));//vkCreateFence(device, vkh::VkFenceCreateInfo{}, nullptr, &fence);
        return fence;
    };

    // submit command (with async wait)
    static inline auto submitCmd(vkt::uni_ptr<xvk::Device> device, const vkt::uni_arg<VkQueue>& queue, const std::vector<VkCommandBuffer>& cmds, vkt::uni_arg<vkh::VkSubmitInfo> smbi = vkh::VkSubmitInfo{}) {
        // no commands 
        if (cmds.size() <= 0) return;
        smbi->commandBufferCount = static_cast<uint32_t>(cmds.size());
        smbi->pCommandBuffers = (VkCommandBuffer*)cmds.data();

        VkFence fence = createFence(device, false);
        vkh::handleVk(device->QueueSubmit(queue, 1u, *smbi, fence));
        vkh::handleVk(device->WaitForFences(1u, &fence, true, 30ull * 1000ull * 1000ull * 1000ull));
        (device->DestroyFence(fence, nullptr));

        return;
    };

    // once submit command buffer
    // TODO: return VkResult
    static inline auto submitOnce(vkt::uni_ptr<xvk::Device> device, vkt::uni_arg<VkQueue> queue, const vkt::uni_arg<VkCommandPool>& cmdPool, const std::function<void(VkCommandBuffer&)>& cmdFn = {}, const vkt::uni_arg<vkh::VkSubmitInfo>& smbi = vkh::VkSubmitInfo{}) {
        auto cmdBuf = createCommandBuffer(device, cmdPool, false); cmdFn(cmdBuf); vkh::handleVk(device->EndCommandBuffer(cmdBuf)); //vkEndCommandBuffer(cmdBuf);
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
        VkCommandBuffer cmdBuf = createCommandBuffer(device, cmdPool, false); cmdFn(cmdBuf); vkh::handleVk(device->EndCommandBuffer(cmdBuf));//vkEndCommandBuffer(cmdBuf);
        return std::async(std::launch::async | std::launch::deferred, [&]() {
            submitCmdAsync(device, queue, { cmdBuf }, smbi).get();
            device->FreeCommandBuffers(cmdPool, 1u, &cmdBuf);
            });
    };

    // Dedicated Semaphore Creator
    static inline void createSemaphore(vkt::uni_ptr<xvk::Device> device, VkSemaphore* vkSemaphore, unsigned* unitPtr = nullptr, const void* pNext = nullptr, const bool GL = false) {
        const auto exportable = vkh::VkExportSemaphoreCreateInfo{ .pNext = pNext, .handleTypes = { .eOpaqueWin32 = 1} };

        HANDLE handle{ INVALID_HANDLE_VALUE };
#ifdef UNICODE
        vkh::handleVk(device->CreateSemaphoreW(vkh::VkSemaphoreCreateInfo{ .pNext = &exportable }, nullptr, vkSemaphore));
#else
        vkh::handleVk(device->CreateSemaphoreA(vkh::VkSemaphoreCreateInfo{ .pNext = &exportable }, nullptr, vkSemaphore));
#endif
        vkh::handleVk(device->GetSemaphoreWin32HandleKHR(vkh::VkSemaphoreGetWin32HandleInfoKHR{ .semaphore = *vkSemaphore, .handleType = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32_BIT }, &handle));
        if (unitPtr) {
#ifdef ENABLE_OPTIX_DENOISE
#ifdef ENABLE_OPENGL_INTEROP
            if (GL)
#endif
            {
                cudaExternalSemaphoreHandleDesc externalSemaphoreHandleDesc = {};
                std::memset(&externalSemaphoreHandleDesc, 0, sizeof(externalSemaphoreHandleDesc));
                externalSemaphoreHandleDesc.flags = 0;
                externalSemaphoreHandleDesc.type = cudaExternalSemaphoreHandleTypeD3D12Fence;
                externalSemaphoreHandleDesc.handle.win32.handle = (void*)handle;
                CUDA_CHECK(cudaImportExternalSemaphore((cudaExternalSemaphore_t*)unitPtr, &externalSemaphoreHandleDesc));
            };
#endif
#ifdef ENABLE_OPENGL_INTEROP
#ifdef ENABLE_OPTIX_DENOISE
            if (GL)
#endif
            {
                glGenSemaphoresEXT(1, unitPtr);
                glImportSemaphoreWin32HandleEXT(*unitPtr, GL_HANDLE_TYPE_OPAQUE_WIN32_EXT, handle);
            };
#endif
        };
        //glCheckError();
    };

    // 
#ifdef VULKAN_HPP
    template<class T>
    T handleHpp(vk::ResultValue<T> V) {
        //assert(V.result == VkResult::eSuccess);
        return std::move(V.value);
    };
#endif


    // Global initials
    class vkGlobal {
    public: // Currently, only Vulkan Loader
        static inline bool initialized = false;
        static inline std::shared_ptr<xvk::Loader> loader = {};
        static inline std::shared_ptr<xvk::Device> device = {};
        static inline std::shared_ptr<xvk::Instance> instance = {};

    #ifdef VKT_ENABLE_GLFW_SUPPORT
    #ifdef VKT_ENABLE_GLFW_LINKED
        vkGlobal(GLFWglproc(*glfwGetProcAddress)(const char*) = ::glfwGetProcAddress) {
    #else 
        vkGlobal(GLFWglproc(*glfwGetProcAddress)(const char*) = nullptr) {
    #endif
            if (!initialized) {
                loader = std::make_shared<xvk::Loader>();
                if (!(initialized = (*loader)())) { std::cerr << "vulkan load failed..." << std::endl; };
    #ifdef ENABLE_OPENGL_INTEROP
                if (glfwGetProcAddress) {
                    vkt::initializeGL(glfwGetProcAddress);
                }
    #endif
            };
        };
    #else
        vkGlobal() {
            if (!initialized) {
                loader = std::make_shared<xvk::Loader>();
                if (!(initialized = (*loader)())) { std::cerr << "vulkan load failed..." << std::endl; };
            };
        };
    #endif
    };

    // 
    struct VmaMemoryInfo {
        VmaMemoryUsage memUsage = VMA_MEMORY_USAGE_GPU_ONLY;
        std::shared_ptr<xvk::Instance> instanceDispatch = vkGlobal::instance;
        std::shared_ptr<xvk::Device> deviceDispatch = vkGlobal::device;


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
        vkt::uni_ptr<xvk::Instance> instanceDispatch = vkGlobal::instance;
        vkt::uni_ptr<xvk::Device> deviceDispatch = vkGlobal::device;

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

    // TODO: native image barrier in library
    struct ImageBarrierInfo {
        vkt::uni_ptr<xvk::Instance> instanceDispatch = vkGlobal::instance;
        vkt::uni_ptr<xvk::Device> deviceDispatch = vkGlobal::device;
        vkt::uni_arg<VkImage> image = {};
        vkt::uni_arg<VkImageLayout> targetLayout = VK_IMAGE_LAYOUT_GENERAL;
        vkt::uni_arg<VkImageLayout> originLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        vkt::uni_arg<vkh::VkImageSubresourceRange> subresourceRange = vkh::VkImageSubresourceRange{ {}, 0u, 1u, 0u, 1u };
    };

    //
    static inline auto imageBarrier(const vkt::uni_arg<VkCommandBuffer>& cmd = VkCommandBuffer{}, const vkt::uni_arg<ImageBarrierInfo>& info = ImageBarrierInfo{}) {
        VkResult result = VK_SUCCESS; // planned to complete
        if (*info->originLayout == *info->targetLayout) { return result; }; // no need transfering more

        //
        vkh::VkImageMemoryBarrier imageMemoryBarriers = {};
        imageMemoryBarriers.srcQueueFamilyIndex = ~0U;
        imageMemoryBarriers.dstQueueFamilyIndex = ~0U;
        imageMemoryBarriers.oldLayout = info->originLayout;
        imageMemoryBarriers.newLayout = info->targetLayout;
        imageMemoryBarriers.subresourceRange = reinterpret_cast<const VkImageSubresourceRange&>(*info->subresourceRange);
        imageMemoryBarriers.image = info->image;

        // Put barrier on top
        const auto srcStageMask = vkh::VkPipelineStageFlags{ .eBottomOfPipe = 1 };
        const auto dstStageMask = vkh::VkPipelineStageFlags{ .eTopOfPipe = 1 };
        const auto dependencyFlags = vkh::VkDependencyFlags{};
        auto srcMask = vkh::VkAccessFlags{};
        auto dstMask = vkh::VkAccessFlags{};

        //
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
        (info->deviceDispatch->vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, {},
                                                    0u, nullptr,
                                                    0u, nullptr,
                                                    1u, imageMemoryBarriers));

        //
        return result;
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
