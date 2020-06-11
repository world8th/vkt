#pragma once // #

#include <vkh/core.hpp>

namespace vkt {

    // application surface format information structure
    //struct SurfaceFormat : public std::enable_shared_from_this<SurfaceFormat> {
    struct SurfaceFormat {
        VkFormat colorFormat = VK_FORMAT_UNDEFINED;
        VkFormat depthFormat = VK_FORMAT_UNDEFINED;
        VkFormat stencilFormat = VK_FORMAT_UNDEFINED;
        VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        VkFormatProperties colorFormatProperties = {};
    };

    // framebuffer with command buffer and fence
    struct Framebuffer : public std::enable_shared_from_this<Framebuffer> {
        VkFramebuffer frameBuffer = VK_NULL_HANDLE;
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE; // terminal command (barrier)
        VkFence waitFence = VK_NULL_HANDLE;
        VkImage image = VK_NULL_HANDLE;
        VkSemaphore drawSemaphore = VK_NULL_HANDLE, computeSemaphore = VK_NULL_HANDLE, presentSemaphore = VK_NULL_HANDLE;
        //VkSemaphore timeline = VK_NULL_HANDLE;
    };

#pragma pack(push, 1)
    struct GeometryInstance {
        //float transform[12];
        glm::mat3x4 transform = glm::mat3x4(1.f);
        uint32_t instanceId : 24;
        uint32_t mask : 8;
        uint32_t instanceOffset : 24;
        uint32_t flags : 8;
        uint64_t accelerationStructureHandle;
    };
#pragma pack(pop)

};
