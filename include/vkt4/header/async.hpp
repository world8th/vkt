#pragma once // #

#include "./definition.hpp"
//#include "./essential.hpp"
#include "./inline.hpp"

#include <iomanip>
#include <future>

import vkt_essential;

namespace vkt {

    // submit command (with async wait)
    static inline auto submitUtilize(vkt::uni_ptr<xvk::Device> device, vkt::uni_arg<VkQueue> queue, vkt::uni_arg<VkCommandPool> cmdPool, std::vector<VkCommandBuffer> cmds, vkt::uni_arg<vkh::VkSubmitInfo> smbi = vkh::VkSubmitInfo{}) {
        //
        if (cmds.size() > 0) {
            smbi->commandBufferCount = static_cast<uint32_t>(cmds.size());
            smbi->pCommandBuffers = (VkCommandBuffer*)cmds.data();
        };

        //
        VkFence fence = createFence(device, false);
        vkh::handleVk(device->QueueSubmit(queue, 1u, *smbi, fence));

        // 
        return std::async(std::launch::async | std::launch::deferred, [](vkt::uni_ptr<xvk::Device> device, vkt::uni_arg<VkQueue> queue, vkt::uni_arg<VkCommandPool> cmdPool, vkt::uni_arg<VkFence> fence, std::vector<VkCommandBuffer> cmds, vkt::uni_arg<vkh::VkSubmitInfo> smbi = vkh::VkSubmitInfo{}) {
            if (cmds.size() > 0) {
                smbi->commandBufferCount = static_cast<uint32_t>(cmds.size());
                smbi->pCommandBuffers = (VkCommandBuffer*)cmds.data();
            };
            vkh::handleVk(device->WaitForFences(1u, fence, true, 30ull * 1000ull * 1000ull * 1000ull));
            device->DestroyFence(fence, nullptr);
            device->FreeCommandBuffers(cmdPool, smbi->commandBufferCount, smbi->pCommandBuffers);
        }, device, queue, cmdPool, fence, cmds, smbi);
    };

    // once submit command buffer
    // TODO: return VkResult
    static inline auto submitOnceAsync(vkt::uni_ptr<xvk::Device> device, vkt::uni_arg<VkQueue> queue, vkt::uni_arg<VkCommandPool> cmdPool, const std::function<void(VkCommandBuffer&)>& cmdFn = {}, const vkt::uni_arg<vkh::VkSubmitInfo>& smbi = vkh::VkSubmitInfo{}) {
        VkCommandBuffer cmdBuf = createCommandBuffer(device, cmdPool, false); cmdFn(cmdBuf); vkh::handleVk(device->EndCommandBuffer(cmdBuf)); cmdFn(cmdBuf);
        return submitUtilize(device, queue, cmdPool, { cmdBuf }, smbi);
    };

};
