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
            vkh::handleVk(device->dispatch->CreateCommandPool(vkh::VkCommandPoolCreateInfo{ .flags = resetFlag, .queueFamilyIndex = queueFamilyIndex }, nullptr, &this->commandPool));

            // 
            return this->queue;
        };

        // 
        virtual VkCommandBuffer createCommandBuffer(vkt::uni_arg<bool> secondary = false, vkt::uni_arg<bool> once = false){
            return vkt::createCommandBuffer(device->dispatch, commandPool, secondary, once);
        };

        // 
        virtual const Queue* submitUtilize(vkt::uni_arg<VkCommandBuffer> cmds, vkt::uni_arg<vkh::VkSubmitInfo> smbi = vkh::VkSubmitInfo{}) const {
            this->submitUtilize(std::vector<VkCommandBuffer>{ cmds }, smbi);
            return this;
        };

        // 
        virtual const Queue* submitUtilize(const std::vector<VkCommandBuffer>& cmds, vkt::uni_arg<vkh::VkSubmitInfo> smbi = vkh::VkSubmitInfo{}) const {
            vkt::submitUtilize(device->dispatch, queue, commandPool, cmds, smbi);
            return this;
        };

        // 
        virtual const Queue* submitOnce(const std::function<void(VkCommandBuffer&)>& cmdFn, vkt::uni_arg<vkh::VkSubmitInfo> smbi = vkh::VkSubmitInfo{}) const {
            vkt::submitOnce(device->dispatch, queue, commandPool, cmdFn, smbi);
            return this;
        };

        // Async Version
        virtual std::future<const Queue*> submitOnceAsync(const std::function<void(VkCommandBuffer&)>& cmdFn, vkt::uni_arg<vkh::VkSubmitInfo> smbi = vkh::VkSubmitInfo{}) const {
            return std::async(std::launch::async | std::launch::deferred, [=, this]() {
                vkt::submitOnceAsync(device->dispatch, queue, commandPool, cmdFn, smbi).get();
                return this;
            });
        };

        // 
        virtual const Queue* submitCmd(vkt::uni_arg<VkCommandBuffer> cmds, vkt::uni_arg<vkh::VkSubmitInfo> smbi = vkh::VkSubmitInfo{}) const {
            return this->submitCmd(std::vector<VkCommandBuffer>{ cmds }, smbi);
        };

        // 
        virtual const Queue* submitCmd(const std::vector<VkCommandBuffer>& cmds, vkt::uni_arg<vkh::VkSubmitInfo> smbi = vkh::VkSubmitInfo{}) const {
            vkt::submitCmd(device->dispatch, queue, cmds, smbi);
            return this;
        };

        // 
        virtual const Queue* submitCmdAsync(const std::vector<VkCommandBuffer>& cmds, vkt::uni_arg<vkh::VkSubmitInfo> smbi = vkh::VkSubmitInfo{}) const {
            vkt::submitCmdAsync(device->dispatch, queue, cmds, smbi);
            return this;
        };

    };

};
