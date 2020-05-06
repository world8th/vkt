#pragma once // #

#include "./utils.hpp"

namespace vkt {

    // application surface format information structure
    //struct SurfaceFormat : public std::enable_shared_from_this<SurfaceFormat> {
    struct SurfaceFormat {
        VkFormat colorFormat = {};
        VkFormat depthFormat = {};
        VkFormat stencilFormat = {};
        VkColorSpaceKHR colorSpace = {};
        VkFormatProperties colorFormatProperties = {};
    };

    // framebuffer with command buffer and fence
    struct Framebuffer : public std::enable_shared_from_this<Framebuffer> {
        VkFramebuffer frameBuffer = {};
        VkCommandBuffer commandBuffer = {}; // terminal command (barrier)
        VkFence waitFence = {};
        //VkImage image = {};

        VkSemaphore drawSemaphore = {}, computeSemaphore = {}, presentSemaphore = {};

        //VkSemaphore timeline = {};
    };

#pragma pack(push, 1)
    struct GeometryInstance {
        //float transform[12];
        glm::mat3x4 transform;
        uint32_t instanceId : 24;
        uint32_t mask : 8;
        uint32_t instanceOffset : 24;
        uint32_t flags : 8;
        uint64_t accelerationStructureHandle;
    };
#pragma pack(pop)

    


};
