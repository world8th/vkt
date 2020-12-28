#pragma once // #

#include "./incornation.hpp"
#include "./device-extensions.hpp"
#include "./instance.hpp"
#include "./device.hpp"

// 
namespace vkf {

    // TODO: DEDICATED QUEUE OBJECT
    class Queue { public:
        std::shared_ptr<Instance> instance = {};
        std::shared_ptr<Device> device = {};

        // 
        VkQueue queue = VK_NULL_HANDLE;
        VkCommandPool commandPool = VK_NULL_HANDLE;
        uint32_t queueFamilyIndex = 0;
        uint32_t version = 0;

        // rise up vRt feature
        vkf::Vector<uint8_t> uploadBuffer = {};
        vkf::Vector<uint8_t> downloadBuffer = {};

        Queue(){
            
        };

        Queue(std::shared_ptr<Device> device) : device(device), instance(device->instance) {
            
        };

        operator VkQueue&(){
            return queue;
        };

        operator const VkQueue&() const {
            return queue;
        };

        // queueFamilyID - queueFamilyIndex from list of device, queueIndex - queue index itself
        virtual VkQueue& create(const uint32_t& queueFamilyID = 0u, const uint32_t& queueIndex = 0u){
            // 
            auto resetFlag = vkh::VkCommandPoolCreateFlags{ .eResetCommandBuffer = 1 };
            device->dispatch->GetDeviceQueue(this->queueFamilyIndex = device->queueFamilyIndices[queueFamilyID], queueIndex, &this->queue);
            vkt::handleVk(device->dispatch->CreateCommandPool(vkh::VkCommandPoolCreateInfo{ .flags = resetFlag, .queueFamilyIndex = queueFamilyIndex }, nullptr, &this->commandPool));

            {
                auto size = 1024ull * 1024ull * 256ull;
                auto bufferCreateInfo = vkh::VkBufferCreateInfo{
                    .size = size,
                    .usage = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT
                };
                auto vmaCreateInfo = vkf::VmaMemoryInfo{
                    .memUsage = VMA_MEMORY_USAGE_CPU_TO_GPU,
                    .instanceDispatch = instance->dispatch,
                    .deviceDispatch = device->dispatch
                };
                auto allocation = std::make_shared<vkf::VmaBufferAllocation>(device->allocator, bufferCreateInfo, vmaCreateInfo);
                uploadBuffer = vkf::Vector<uint8_t>(allocation, 0ull, size);
            };

            {
                auto size = 1024ull * 1024ull * 256ull;
                auto bufferCreateInfo = vkh::VkBufferCreateInfo{
                    .size = size,
                    .usage = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
                };
                auto vmaCreateInfo = vkf::VmaMemoryInfo{
                    .memUsage = VMA_MEMORY_USAGE_GPU_TO_CPU,
                    .instanceDispatch = instance->dispatch,
                    .deviceDispatch = device->dispatch
                };
                auto allocation = std::make_shared<vkf::VmaBufferAllocation>(device->allocator, bufferCreateInfo, vmaCreateInfo);
                downloadBuffer = vkf::Vector<uint8_t>(allocation, 0ull, size);
            };

            // 
            return this->queue;
        };

        // 
        virtual const Queue* downloadFromBuffer(void* data, vkf::VectorBase output, VkDeviceSize size) const {
            size = std::min(size, output.range());
            VkBufferCopy2KHR srcCopy = {
                .sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2_KHR,
                .pNext = nullptr,
                .srcOffset = output.offset(),
                .dstOffset = uploadBuffer.offset(),
                .size = size
            };
            VkCopyBufferInfo2KHR copyInfo = {
                .sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2_KHR,
                .pNext = nullptr,
                .srcBuffer = output,
                .dstBuffer = downloadBuffer,
                .regionCount = 1,
                .pRegions = &srcCopy
            };
            auto result = this->submitOnce([&](VkCommandBuffer commandBuffer) {
                device->dispatch->CmdCopyBuffer2KHR(commandBuffer, &copyInfo);
            });
            memcpy(data, downloadBuffer.mappedv(), size);
            return result;
        };

        // 
        virtual const Queue* uploadIntoBuffer(vkf::VectorBase input, const void* data, VkDeviceSize size) {
            size = std::min(size, input.range());
            VkBufferCopy2KHR srcCopy = {
                .sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2_KHR,
                .pNext = nullptr,
                .srcOffset = uploadBuffer.offset(),
                .dstOffset = input.offset(),
                .size = size
            };
            VkCopyBufferInfo2KHR copyInfo = {
                .sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2_KHR,
                .pNext = nullptr,
                .srcBuffer = uploadBuffer,
                .dstBuffer = input,
                .regionCount = 1,
                .pRegions = &srcCopy
            };
            memcpy(uploadBuffer.mappedv(), data, size);
            return this->submitOnce([&](VkCommandBuffer commandBuffer) {
                device->dispatch->CmdCopyBuffer2KHR(commandBuffer, &copyInfo);
            });
        };

        // 
        virtual const Queue* uploadIntoImage(vkf::ImageRegion image, const void* data, vkh::VkOffset3D offset = {0x0, 0x0, 0x0}, vkh::VkExtent3D extent = {0x10000u, 0x10000u, 0x10000u}, vkh::VkImageSubresourceLayers subresourceLayers = { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 0u, 1u }) {
            extent = glm::min(glm::uvec3(extent), glm::uvec3(image.getCreateInfo().extent) - glm::uvec3(glm::ivec3(offset)));
            vkh::BlockParams params = vkh::getBlockParams(image.getCreateInfo().format);
            VkBufferImageCopy2KHR srcCopy = {
                .sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2_KHR,
                .pNext = nullptr, 
                .bufferOffset = uploadBuffer.offset(),
                .bufferRowLength = 0u,//extent.width,
                .bufferImageHeight = 0u,//extent.height,
                .imageSubresource = subresourceLayers, // 
                .imageOffset = offset,
                .imageExtent = extent
            };
            VkCopyBufferToImageInfo2KHR copyInfo = {
                .sType = VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2_KHR,
                .pNext = nullptr,
                .srcBuffer = uploadBuffer,
                .dstImage = image,
                .dstImageLayout = image,
                .regionCount = 1u,
                .pRegions = &srcCopy
            };
            memcpy(uploadBuffer.mappedv(), data, extent.width*extent.height*extent.depth*params.bytesPerBlock);
            return this->submitOnce([&](VkCommandBuffer commandBuffer) {
                device->dispatch->vkCmdCopyBufferToImage2KHR(commandBuffer, &copyInfo);
            });
        };

        // 
        virtual VkCommandBuffer createCommandBuffer(vkh::uni_arg<bool> secondary = false, vkh::uni_arg<bool> once = false){
            return vkt::createCommandBuffer(device->dispatch, commandPool, secondary, once);
        };

        // 
        virtual const Queue* submitUtilize(vkh::uni_arg<VkCommandBuffer> cmds, vkh::uni_arg<vkh::VkSubmitInfo> smbi = vkh::VkSubmitInfo{}) const {
            this->submitUtilize(std::vector<VkCommandBuffer>{ cmds }, smbi);
            return this;
        };

        // 
        virtual const Queue* submitUtilize(const std::vector<VkCommandBuffer>& cmds, vkh::uni_arg<vkh::VkSubmitInfo> smbi = vkh::VkSubmitInfo{}) const {
            vkt::submitUtilize(device->dispatch, queue, commandPool, cmds, smbi);
            return this;
        };

        // 
        virtual const Queue* submitOnce(const std::function<void(VkCommandBuffer&)>& cmdFn, vkh::uni_arg<vkh::VkSubmitInfo> smbi = vkh::VkSubmitInfo{}) const {
            vkt::submitOnce(device->dispatch, queue, commandPool, cmdFn, smbi);
            return this;
        };

        // Async Version
        virtual std::future<const Queue*> submitOnceAsync(const std::function<void(VkCommandBuffer&)>& cmdFn, vkh::uni_arg<vkh::VkSubmitInfo> smbi = vkh::VkSubmitInfo{}) const {
            return std::async(std::launch::async | std::launch::deferred, [=, this]() {
                vkt::submitOnceAsync(device->dispatch, queue, commandPool, cmdFn, smbi).get();
                return this;
            });
        };

        // 
        virtual const Queue* submitCmd(vkh::uni_arg<VkCommandBuffer> cmds, vkh::uni_arg<vkh::VkSubmitInfo> smbi = vkh::VkSubmitInfo{}) const {
            return this->submitCmd(std::vector<VkCommandBuffer>{ cmds }, smbi);
        };

        // 
        virtual const Queue* submitCmd(const std::vector<VkCommandBuffer>& cmds, vkh::uni_arg<vkh::VkSubmitInfo> smbi = vkh::VkSubmitInfo{}) const {
            vkt::submitCmd(device->dispatch, queue, cmds, smbi);
            return this;
        };

        // 
        virtual const Queue* submitCmdAsync(const std::vector<VkCommandBuffer>& cmds, vkh::uni_arg<vkh::VkSubmitInfo> smbi = vkh::VkSubmitInfo{}) const {
            vkt::submitCmdAsync(device->dispatch, queue, cmds, smbi);
            return this;
        };

    };

};
