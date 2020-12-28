#pragma once // #

// 
#include "./definition.hpp"
#include "./inline.hpp"

// use helpers for essentials
#include <vkh/helpers.hpp>

// TODO: C++20 Modular
namespace vkt {

    // 
    typedef uint8_t BYTE;

    // Read binary (for SPIR-V)
    // Updated 10.05.2020
    inline std::vector<BYTE> readBinaryU8(vkh::uni_arg<std::string> filePath) { // open the file:
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
    static inline auto readBinary(vkh::uni_arg<std::string> filePath) {
        const auto vect8u = readBinaryU8(filePath);
        auto vect32 = std::vector<uint32_t>(vkh::tiled(uint64_t(vect8u.size()), uint64_t(4ull)));
        memcpy(vect32.data(), vect8u.data(), vect8u.size()); return vect32;
    };

    // read source (unused)
    static inline auto readSource(vkh::uni_arg<std::string> filePath, bool lineDirective = false) {
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

    // BROKEN FOR C++20 MODULES
    // Used for directly create shader from code! (Prevent Code LOST!)
    inline std::vector<uint32_t> TempCode = {};

    // 
    static inline auto makeShaderModuleInfo(const std::vector<uint32_t>& code) {
        return vkh::VkShaderModuleCreateInfo{ .codeSize = code.size() * 4ull, .pCode = code.data() };
    };

    // create shader module (BROKEN FOR XVK!)
    static inline auto createShaderModuleIntrusive(vkt::Device device, const std::vector<uint32_t>& code, vkh::uni_ptr<VkShaderModule> hndl) {
        if (sizeof(vkh::VkShaderModuleCreateInfo) != sizeof(::VkShaderModuleCreateInfo)) {
            std::cerr << "BROKEN 'vkh::VkShaderModuleCreateInfo' STRUCTURE!" << std::endl; assert(-1);
        };
        vkt::handleVk(device->CreateShaderModule(makeShaderModuleInfo(TempCode = code), nullptr, hndl.get_ptr()));
        return hndl;
    };

    // 
    static inline auto createShaderModule(vkt::Device device, const std::vector<uint32_t>& code) {
        vkh::uni_arg<VkShaderModule> sm = VkShaderModule{};
        createShaderModuleIntrusive(device, TempCode = code, sm);
        return sm;
    };

    // create shader module 
    static inline auto makePipelineStageInfo(vkt::Device device, const std::vector<uint32_t>& code, FLAGS(VkShaderStage) stage = VK_SHADER_STAGE_COMPUTE_BIT, vkh::uni_arg<const char*> entry = "main") {
        vkh::VkPipelineShaderStageCreateInfo spi = {};
        createShaderModuleIntrusive(device, code, spi.module);
        spi.pName = entry;
        spi.stage = stage;
        spi.pSpecializationInfo = {};
        return std::move(spi);
    };

    // create shader module 
    static inline auto makePipelineStageInfoWithoutModule(vkt::Device device, FLAGS(VkShaderStage) stage = VK_SHADER_STAGE_COMPUTE_BIT, vkh::uni_arg<const char*> entry = "main") {
        vkh::VkPipelineShaderStageCreateInfo spi = {};
        spi.pName = entry;
        spi.stage = stage;
        spi.pSpecializationInfo = {};
        return std::move(spi);
    };

    // create shader module
    static inline auto makeComputePipelineStageInfo(vkt::Device device, const std::vector<uint32_t>& code, vkh::uni_arg<const char*> entry = "main", vkh::uni_arg<uint32_t> subgroupSize = 0u) {
        auto f = FixConstruction{};
        auto fl = vkh::VkPipelineShaderStageCreateFlags{ .eRequireFullSubgroups = 1u };
        f.spi = makePipelineStageInfoWithoutModule(device, VK_SHADER_STAGE_COMPUTE_BIT, entry);
        f.spi.flags = fl;
        createShaderModuleIntrusive(device, TempCode = code, (VkShaderModule&)(f.spi.module));
        f.sgmp = vkh::VkPipelineShaderStageRequiredSubgroupSizeCreateInfoEXT{};
        f.sgmp.requiredSubgroupSize = subgroupSize;
        if (subgroupSize) f.spi.pNext = &f.sgmp;
        return std::move(f);
    };

    // create compute pipelines
    static inline auto createCompute(vkt::Device device, vkh::uni_arg<FixConstruction> spi, vkh::uni_arg<VkPipelineLayout> layout, vkh::uni_arg<VkPipelineCache> cache = VkPipelineCache{}, vkh::uni_arg<uint32_t> subgroupSize = 0u) {
        auto cmpi = vkh::VkComputePipelineCreateInfo{};
        auto fl = vkh::VkPipelineCreateFlags{}; vkh::unlock32(fl) = 0u;
        cmpi.flags = fl;
        cmpi.layout = layout;
        cmpi.stage = *spi;
        cmpi.basePipelineIndex = -1;
        VkPipeline pipeline = {};
        vkt::handleVk(device->CreateComputePipelines(cache, 1u, cmpi, nullptr, &pipeline));
        return pipeline;
    };

    // create compute pipelines
    static inline auto createCompute(vkt::Device device, const std::vector<uint32_t>& code, vkh::uni_arg<VkPipelineLayout> layout, vkh::uni_arg<VkPipelineCache> cache = VkPipelineCache{}, vkh::uni_arg<uint32_t> subgroupSize = 0u) {
        auto f = makeComputePipelineStageInfo(device, TempCode = code, "main", subgroupSize);
        if (subgroupSize && *subgroupSize) f.spi.pNext = &f.sgmp; // fix link
        return createCompute(device, f, layout, cache, subgroupSize);
    };

    // create compute pipelines
    static inline auto createCompute(vkt::Device device, vkh::uni_arg<std::string> path, vkh::uni_arg<VkPipelineLayout> layout, vkh::uni_arg<VkPipelineCache> cache = VkPipelineCache{}, vkh::uni_arg<uint32_t> subgroupSize = 0u) {
        return createCompute(device, readBinary(path), layout, cache, subgroupSize);
    };

    // create secondary command buffers for batching compute invocations
    static inline auto createCommandBuffer(vkt::Device device, vkh::uni_arg<VkCommandPool> cmdPool, vkh::uni_arg<bool> secondary = false, vkh::uni_arg<bool> once = false) {
        VkCommandBuffer cmdBuffer = {};

        vkh::VkCommandBufferAllocateInfo cmdi = vkh::VkCommandBufferAllocateInfo{};
        cmdi.commandPool = cmdPool;
        cmdi.level = (secondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY : VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        cmdi.commandBufferCount = 1;
        //cmdBuffer = (device->allocateCommandBuffers(cmdi))[0];
        vkt::handleVk(device->AllocateCommandBuffers(cmdi, &cmdBuffer));

        //VkCommandBufferInheritanceInfo inhi = VkCommandBufferInheritanceInfo{};
        //inhi.pipelineStatistics = VkQueryPipelineStatisticFlagBits::eComputeShaderInvocations;

        vkh::VkCommandBufferBeginInfo bgi = {};
        bgi.flags = once ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        //bgi.pInheritanceInfo = secondary ? &inhi : nullptr;

        vkt::handleVk(device->vkBeginCommandBuffer(cmdBuffer, bgi));
        //cmdBuffer.begin(bgi);

        return cmdBuffer;
    };

    // OUTDATED and DEPRECATED! Use now vkt::memoryBarrier with vkt::MemoryBarrierInfo
    // general command buffer pipeline barrier (updated 26.04.2020)
    static inline void commandBarrier(vkt::Device device, vkh::uni_arg<VkCommandBuffer> cmdBuffer) {
        vkt::memoryBarrier(cmdBuffer, vkt::MemoryBarrierInfo{
            .deviceDispatch = device,
            .srcStageMask = VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT | VK_PIPELINE_STAGE_HOST_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_VERTEX_INPUT_BIT | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR | VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
            .dstStageMask = VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT | VK_PIPELINE_STAGE_HOST_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_VERTEX_INPUT_BIT | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR | VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR
        });
    };

    // create fence function
    static inline auto createFence(vkt::Device device, const vkh::uni_arg<bool>& signaled = true) {
        VkFenceCreateInfo info = {};
        info.flags = signaled & 1;
        VkFence fence = {}; vkt::handleVk(device->CreateFence(vkh::VkFenceCreateInfo{}, nullptr, &fence));//vkCreateFence(device, vkh::VkFenceCreateInfo{}, nullptr, &fence);
        return fence;
    };

    // submit command (with async wait)
    static inline auto submitUtilize(vkt::Device device, vkh::uni_arg<VkQueue> queue, vkh::uni_arg<VkCommandPool> cmdPool, std::vector<VkCommandBuffer> cmds, vkh::uni_arg<vkh::VkSubmitInfo> smbi = vkh::VkSubmitInfo{}) {
        //
        if (cmds.size() > 0) {
            smbi->commandBufferCount = static_cast<uint32_t>(cmds.size());
            smbi->pCommandBuffers = (VkCommandBuffer*)cmds.data();
        };

        //
        VkFence fence = createFence(device, false);
        vkt::handleVk(device->QueueSubmit(queue, 1u, *smbi, fence));

        // 
        return std::async(std::launch::async | std::launch::deferred, [](vkt::Device device, vkh::uni_arg<VkQueue> queue, vkh::uni_arg<VkCommandPool> cmdPool, vkh::uni_arg<VkFence> fence, std::vector<VkCommandBuffer> cmds, vkh::uni_arg<vkh::VkSubmitInfo> smbi = vkh::VkSubmitInfo{}) {
            if (cmds.size() > 0) {
                smbi->commandBufferCount = static_cast<uint32_t>(cmds.size());
                smbi->pCommandBuffers = (VkCommandBuffer*)cmds.data();
            };
            vkt::handleVk(device->WaitForFences(1u, fence, true, 30ull * 1000ull * 1000ull * 1000ull));
            device->DestroyFence(fence, nullptr);
            device->FreeCommandBuffers(cmdPool, smbi->commandBufferCount, smbi->pCommandBuffers);
        }, device, queue, cmdPool, fence, cmds, smbi);
    };

    // submit command (with async wait)
    static inline auto submitCmd(vkt::Device device, vkh::uni_arg<VkQueue> queue, const std::vector<VkCommandBuffer>& cmds, vkh::uni_arg<vkh::VkSubmitInfo> smbi = vkh::VkSubmitInfo{}) {
        // no commands 
        if (cmds.size() <= 0) return;
        smbi->commandBufferCount = static_cast<uint32_t>(cmds.size());
        smbi->pCommandBuffers = (VkCommandBuffer*)cmds.data();

        // 
        VkFence fence = createFence(device, false);
        vkt::handleVk(device->QueueSubmit(queue, 1u, *smbi, fence));
        vkt::handleVk(device->WaitForFences(1u, &fence, true, 30ull * 1000ull * 1000ull * 1000ull));
        (device->DestroyFence(fence, nullptr));

        // 
        return;
    };

    // once submit command buffer
    // TODO: return VkResult
    static inline auto submitOnce(vkt::Device device, vkh::uni_arg<VkQueue> queue, vkh::uni_arg<VkCommandPool> cmdPool, const std::function<void(VkCommandBuffer&)>& cmdFn = {}, const vkh::uni_arg<vkh::VkSubmitInfo>& smbi = vkh::VkSubmitInfo{}) {
        auto cmdBuf = createCommandBuffer(device, cmdPool, false); cmdFn(cmdBuf); vkt::handleVk(device->EndCommandBuffer(cmdBuf)); //vkEndCommandBuffer(cmdBuf);
        submitCmd(device, queue, { cmdBuf }); device->FreeCommandBuffers(cmdPool, 1u, &cmdBuf);
    };

    // submit command (with async wait)
    // TODO: return VkResult
    static inline auto submitCmdAsync(vkt::Device device, vkh::uni_arg<VkQueue> queue, std::vector<VkCommandBuffer> cmds, vkh::uni_arg<vkh::VkSubmitInfo> smbi = vkh::VkSubmitInfo{}) {
        //
        if (cmds.size() > 0) {
            smbi->commandBufferCount = static_cast<uint32_t>(cmds.size());
            smbi->pCommandBuffers = (VkCommandBuffer*)cmds.data();
        };

        //
        VkFence fence = createFence(device, false);
        vkt::handleVk(device->QueueSubmit(queue, 1u, *smbi, fence));

        // 
        return std::async(std::launch::async | std::launch::deferred, [](vkt::Device device, vkh::uni_arg<VkQueue> queue, vkh::uni_arg<VkFence> fence, std::vector<VkCommandBuffer> cmds, vkh::uni_arg<vkh::VkSubmitInfo> smbi = vkh::VkSubmitInfo{}) {
            if (cmds.size() > 0) {
                smbi->commandBufferCount = static_cast<uint32_t>(cmds.size());
                smbi->pCommandBuffers = (VkCommandBuffer*)cmds.data();
            };
            vkt::handleVk(device->WaitForFences(1u, fence, true, 30ull * 1000ull * 1000ull * 1000ull));
            device->DestroyFence(fence, nullptr);
        }, device, queue, fence, cmds, smbi);
    };

    // once submit command buffer
    // TODO: return VkResult
    static inline auto submitOnceAsync(vkt::Device device, vkh::uni_arg<VkQueue> queue, vkh::uni_arg<VkCommandPool> cmdPool, const std::function<void(VkCommandBuffer&)>& cmdFn = {}, const vkh::uni_arg<vkh::VkSubmitInfo>& smbi = vkh::VkSubmitInfo{}) {
        VkCommandBuffer cmdBuf = createCommandBuffer(device, cmdPool, false); cmdFn(cmdBuf); vkt::handleVk(device->EndCommandBuffer(cmdBuf)); cmdFn(cmdBuf);
        return submitUtilize(device, queue, cmdPool, { cmdBuf }, smbi);
    };

#ifdef VKT_WIN32_DETECTED
#ifndef CreateSemaphore
#ifdef UNICODE
#define CreateSemaphore CreateSemaphoreW
#else
#define CreateSemaphore CreateSemaphoreA
#endif
#endif
#endif

    // Dedicated Semaphore Creator
    static inline void createSemaphore(vkt::Device device, VkSemaphore* vkSemaphore, unsigned* unitPtr = nullptr, const void* pNext = nullptr, const bool GL = false) {
        const auto exportable = vkh::VkExportSemaphoreCreateInfo{ .pNext = pNext, .handleTypes = vkh::VkExternalSemaphoreHandleTypeFlags{ 
#ifdef VKT_WIN32_DETECTED
            .eOpaqueWin32 = 1
#else
            .eOpaqueFd = 1
#endif
        } };
        vkt::handleVk(device->CreateSemaphore(vkh::VkSemaphoreCreateInfo{ .pNext = &exportable }, nullptr, vkSemaphore));

#ifdef VKT_WIN32_DETECTED
        HANDLE handle{ INVALID_HANDLE_VALUE };
        vkt::handleVk(device->GetSemaphoreWin32HandleKHR(vkh::VkSemaphoreGetWin32HandleInfoKHR{ .semaphore = *vkSemaphore, .handleType = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32_BIT }, &handle));
#endif

        if (unitPtr) 
        {
#ifdef VKT_WIN32_DETECTED
#ifdef VKT_CUDA_SUPPORT
#ifdef VKT_OPENGL_INTEROP
            if (!GL)
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
#ifdef VKT_OPENGL_INTEROP
#ifdef VKT_CUDA_SUPPORT
            if (GL)
#endif
            {
                glGenSemaphoresEXT(1, unitPtr);
                glImportSemaphoreWin32HandleEXT(*unitPtr, GL_HANDLE_TYPE_OPAQUE_WIN32_EXT, handle);
            };
#endif
#else

#endif
        };
        //glCheckError();
    };

    // REQUIRED VKH
    inline VkResult AllocateDescriptorSetWithUpdate(vkt::Device& device, vkh::VsDescriptorSetCreateInfoHelper& helper, VkDescriptorSet& descriptorSet, bool& protection) {
        if (!protection) {
            // Corrupt... 
            //if (descriptorSet) { vkt::handleVk(device->vkFreeDescriptorSets(device->handle, helper, 1u, &descriptorSet)); descriptorSet = {}; };

            // 
            bool created = false;
            if (!descriptorSet) { vkt::handleVk(device->AllocateDescriptorSets(helper, &descriptorSet)); created = true; };

            //
            if (descriptorSet && created) {
                const auto& writes = helper.setDescriptorSet(descriptorSet).mapWriteDescriptorSet();
                device->UpdateDescriptorSets(uint32_t(writes.size()), reinterpret_cast<const ::VkWriteDescriptorSet*>(writes.data()), 0u, nullptr);
            };

            // 
            protection = true;
        };

        // 
        return VK_SUCCESS;
    };

};
