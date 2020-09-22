#pragma once // #

// WARNING! IT JUST CODE!
// THERE IS NO HEADERS, DUE MODULES... 
//#include "./definition.hpp"
//#include "./inline.hpp"

// TODO: C++20 Modular
export namespace vkt {

    // 
    typedef uint8_t BYTE;

    // Read binary (for SPIR-V)
    // Updated 10.05.2020
    export inline std::vector<BYTE> readBinaryU8(vkt::uni_arg<std::string> filePath) { // open the file:
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
    export static inline auto readBinary(vkt::uni_arg<std::string> filePath) {
        const auto vect8u = readBinaryU8(filePath);
        auto vect32 = std::vector<uint32_t>(tiled(uint64_t(vect8u.size()), uint64_t(4ull)));
        memcpy(vect32.data(), vect8u.data(), vect8u.size()); return vect32;
    };

    // read source (unused)
    export static inline auto readSource(vkt::uni_arg<std::string> filePath, bool lineDirective = false) {
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

    // 
    export static inline auto makeShaderModuleInfo(const std::vector<uint32_t>& code) {
        return vkh::VkShaderModuleCreateInfo{ .codeSize = code.size() * 4ull, .pCode = code.data() };
    };

    // create shader module (BROKEN FOR XVK!)
    export static inline auto createShaderModuleIntrusive(vkt::uni_ptr<xvk::Device> device, const std::vector<uint32_t>& code, vkt::uni_ptr<VkShaderModule> hndl) {
        if (sizeof(vkh::VkShaderModuleCreateInfo) != sizeof(::VkShaderModuleCreateInfo)) {
            std::cerr << "BROKEN 'vkh::VkShaderModuleCreateInfo' STRUCTURE!" << std::endl; assert(-1);
        };
        vkh::handleVk(device->CreateShaderModule(makeShaderModuleInfo(code), nullptr, hndl.get_ptr()));
        return hndl;
    };

    // 
    export static inline auto createShaderModule(vkt::uni_ptr<xvk::Device> device, const std::vector<uint32_t>& code) {
        vkt::uni_arg<VkShaderModule> sm = VkShaderModule{};
        createShaderModuleIntrusive(device, code, sm);
        return sm;
    };

    // create shader module 
    export static inline auto makePipelineStageInfo(vkt::uni_ptr<xvk::Device> device, const std::vector<uint32_t>& code, vkt::uni_arg<VkShaderStageFlagBits> stage = VK_SHADER_STAGE_COMPUTE_BIT, vkt::uni_arg<const char*> entry = "main") {
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
    export static inline auto makePipelineStageInfoWithoutModule(vkt::uni_ptr<xvk::Device> device, vkt::uni_arg<VkShaderStageFlagBits> stage = VK_SHADER_STAGE_COMPUTE_BIT, vkt::uni_arg<const char*> entry = "main") {
        vkh::VkPipelineShaderStageCreateInfo spi = {};
        spi.pName = entry;
        spi.stage = stage;
        spi.pSpecializationInfo = {};
        return std::move(spi);
    };

    // create shader module
    export static inline auto makeComputePipelineStageInfo(vkt::uni_ptr<xvk::Device> device, const std::vector<uint32_t>& code, vkt::uni_arg<const char*> entry = "main", vkt::uni_arg<uint32_t> subgroupSize = 0u) {
        auto f = FixConstruction{};
        auto fl = vkh::VkPipelineShaderStageCreateFlags{ .eRequireFullSubgroups = 1u };
        f.spi = makePipelineStageInfoWithoutModule(device, VK_SHADER_STAGE_COMPUTE_BIT, entry);
        f.spi.flags = fl;
#ifndef VKT_USE_CPP_MODULES
        createShaderModuleIntrusive(device, code, (VkShaderModule&)(f.spi.module));
#else
        createShaderModuleIntrusive(device, code, (VkShaderModule&)(f.spi.modular));
#endif
        f.sgmp = vkh::VkPipelineShaderStageRequiredSubgroupSizeCreateInfoEXT{};
        f.sgmp.requiredSubgroupSize = subgroupSize;
        if (subgroupSize) f.spi.pNext = &f.sgmp;
        return std::move(f);
    };
    
    // create compute pipelines
    export static inline auto createCompute(vkt::uni_ptr<xvk::Device> device, vkt::uni_arg<FixConstruction> spi, vkt::uni_arg<VkPipelineLayout> layout, vkt::uni_arg<VkPipelineCache> cache = VkPipelineCache{}, vkt::uni_arg<uint32_t> subgroupSize = 0u) {
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
    export static inline auto createCompute(vkt::uni_ptr<xvk::Device> device, const std::vector<uint32_t>& code, vkt::uni_arg<VkPipelineLayout> layout, vkt::uni_arg<VkPipelineCache> cache = VkPipelineCache{}, vkt::uni_arg<uint32_t> subgroupSize = 0u) {
        auto f = makeComputePipelineStageInfo(device, code, "main", subgroupSize);
        if (subgroupSize && *subgroupSize) f.spi.pNext = &f.sgmp; // fix link
        return createCompute(device, f, layout, cache, subgroupSize);
    };

    // create compute pipelines
    export static inline auto createCompute(vkt::uni_ptr<xvk::Device> device, vkt::uni_arg<std::string> path, vkt::uni_arg<VkPipelineLayout> layout, vkt::uni_arg<VkPipelineCache> cache = VkPipelineCache{}, vkt::uni_arg<uint32_t> subgroupSize = 0u) {
        return createCompute(device, readBinary(path), layout, cache, subgroupSize);
    };

    // create secondary command buffers for batching compute invocations
    export static inline auto createCommandBuffer(vkt::uni_ptr<xvk::Device> device, vkt::uni_arg<VkCommandPool> cmdPool, vkt::uni_arg<bool> secondary = false, vkt::uni_arg<bool> once = false) {
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

    // general command buffer pipeline barrier (updated 26.04.2020)
    export static inline void commandBarrier(vkt::uni_ptr<xvk::Device> device, vkt::uni_arg<VkCommandBuffer> cmdBuffer) {
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
        memoryBarrier.dstAccessMask = VkAccessFlagBits(VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT | VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_UNIFORM_READ_BIT);
        //memoryBarrier.srcAccessMask = memoryBarrier.srcAccessMask | memoryBarrier.dstAccessMask;

        // 
        (device->vkCmdPipelineBarrier(cmdBuffer, srcStageMask, dstStageMask, {},
            1u, memoryBarrier,
            0u, nullptr,
            0u, nullptr));
    };


    // create fence function
    export static inline auto createFence(vkt::uni_ptr<xvk::Device> device, const vkt::uni_arg<bool>& signaled = true) {
        VkFenceCreateInfo info = {};
        info.flags = signaled & 1;
        VkFence fence = {}; vkh::handleVk(device->CreateFence(vkh::VkFenceCreateInfo{}, nullptr, &fence));//vkCreateFence(device, vkh::VkFenceCreateInfo{}, nullptr, &fence);
        return fence;
    };

    // submit command (with async wait)
    export static inline auto submitCmd(vkt::uni_ptr<xvk::Device> device, vkt::uni_arg<VkQueue> queue, const std::vector<VkCommandBuffer>& cmds, vkt::uni_arg<vkh::VkSubmitInfo> smbi = vkh::VkSubmitInfo{}) {
        // no commands 
        if (cmds.size() <= 0) return;
        smbi->commandBufferCount = static_cast<uint32_t>(cmds.size());
        smbi->pCommandBuffers = (VkCommandBuffer*)cmds.data();

        // 
        VkFence fence = createFence(device, false);
        vkh::handleVk(device->QueueSubmit(queue, 1u, *smbi, fence));
        vkh::handleVk(device->WaitForFences(1u, &fence, true, 30ull * 1000ull * 1000ull * 1000ull));
        (device->DestroyFence(fence, nullptr));

        // 
        return;
    };

    // once submit command buffer
    // TODO: return VkResult
    export static inline auto submitOnce(vkt::uni_ptr<xvk::Device> device, vkt::uni_arg<VkQueue> queue, vkt::uni_arg<VkCommandPool> cmdPool, const std::function<void(VkCommandBuffer&)>& cmdFn = {}, const vkt::uni_arg<vkh::VkSubmitInfo>& smbi = vkh::VkSubmitInfo{}) {
        auto cmdBuf = createCommandBuffer(device, cmdPool, false); cmdFn(cmdBuf); vkh::handleVk(device->EndCommandBuffer(cmdBuf)); //vkEndCommandBuffer(cmdBuf);
        submitCmd(device, queue, { cmdBuf }); device->FreeCommandBuffers(cmdPool, 1u, &cmdBuf);
    };

    // submit command (with async wait)
    // TODO: return VkResult
    // BROKEN! 
    //static inline auto submitCmdAsync(vkt::uni_ptr<xvk::Device> device, vkt::uni_arg<VkQueue> queue, std::vector<VkCommandBuffer> cmds, vkt::uni_arg<vkh::VkSubmitInfo> smbi = vkh::VkSubmitInfo{}) {
    //    return std::async(std::launch::async | std::launch::deferred, [=]() {
    //        return submitCmd(device, queue, cmds, smbi);
    //    });
    //};

    // Dedicated Semaphore Creator
    export static inline void createSemaphore(vkt::uni_ptr<xvk::Device> device, VkSemaphore* vkSemaphore, unsigned* unitPtr = nullptr, const void* pNext = nullptr, const bool GL = false) {
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

};
