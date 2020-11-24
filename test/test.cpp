#define VMA_IMPLEMENTATION

#include <vkf/swapchain.hpp>

// 
void error(int errnum, const char* errmsg)
{
    std::cerr << errnum << ": " << errmsg << std::endl;
};

// 
const uint32_t SCR_WIDTH = 800u, SCR_HEIGHT = 600u;

// 
int main() {
    glfwSetErrorCallback(error);
    glfwInit();

    // 
    if (GLFW_FALSE == glfwVulkanSupported()) {
        glfwTerminate(); return -1;
    };

    // 
    uint32_t canvasWidth = SCR_WIDTH, canvasHeight = SCR_HEIGHT; // For 3840x2160 Resolutions
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // 
    float xscale = 1.f, yscale = 1.f;
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    glfwGetMonitorContentScale(primary, &xscale, &yscale);

    // 
    std::string title = "TestApp";
    vkt::uni_ptr<vkf::Instance> instance = std::make_shared<vkf::Instance>();
    vkt::uni_ptr<vkf::Device> device = std::make_shared<vkf::Device>(instance);
    vkt::uni_ptr<vkf::Queue> queue = std::make_shared<vkf::Queue>(device);
    vkt::uni_ptr<vkf::SwapChain> manager = std::make_shared<vkf::SwapChain>(device);

    // 
    instance->create();
    vkf::SurfaceWindow& surface = manager->createWindowSurface(SCR_WIDTH * xscale, SCR_HEIGHT * yscale, title);

    // 
    device->create(0u, surface.surface);
    queue->create();

    // 
    vkf::SurfaceFormat& format = manager->getSurfaceFormat();
    VkRenderPass& renderPass = manager->createRenderPass();
    VkSwapchainKHR& swapchain = manager->createSwapchain();
    std::vector<vkf::Framebuffer>& framebuffers = manager->createSwapchainFramebuffer(queue);

    // 
    auto allocInfo = vkt::MemoryAllocationInfo{};
    allocInfo.device = *device;
    allocInfo.memoryProperties = device->memoryProperties;
    allocInfo.instanceDispatch = instance->dispatch;
    allocInfo.deviceDispatch = device->dispatch;

    // 
    auto renderArea = vkh::VkRect2D{ vkh::VkOffset2D{0, 0}, vkh::VkExtent2D{ uint32_t(canvasWidth / 1.f * xscale), uint32_t(canvasHeight / 1.f * yscale) } };
    auto viewport = vkh::VkViewport{ 0.0f, 0.0f, static_cast<float>(renderArea.extent.width), static_cast<float>(renderArea.extent.height), 0.f, 1.f };


    // TODO: Inline Uniforms
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    auto pipusage = vkh::VkShaderStageFlags{ .eVertex = 1, .eGeometry = 1, .eFragment = 1, .eCompute = 1, .eRaygen = 1, .eAnyHit = 1, .eClosestHit = 1, .eMiss = 1 };
    auto indexedf = vkh::VkDescriptorBindingFlags{ .eUpdateAfterBind = 1, .eUpdateUnusedWhilePending = 1, .ePartiallyBound = 1 };
    auto dflags = vkh::VkDescriptorSetLayoutCreateFlags{ .eUpdateAfterBindPool = 1 };
    std::vector<VkDescriptorSetLayout> layouts = {};
    std::vector<VkDescriptorSet> descriptorSets = {};
    std::vector<vkh::VkPushConstantRange> ranges = { vkh::VkPushConstantRange{.stageFlags = pipusage, .offset = 0u, .size = 16u } };
    vkh::handleVk(device->dispatch->CreatePipelineLayout(vkh::VkPipelineLayoutCreateInfo{  }.setSetLayouts(layouts).setPushConstantRanges(ranges), nullptr, &pipelineLayout));


    // 
    vkh::VsGraphicsPipelineCreateInfoConstruction pipelineInfo = {};
    pipelineInfo.stages = {
        vkt::makePipelineStageInfo(device->dispatch, vkt::readBinary(std::string("./shaders/render.vert.spv")), VK_SHADER_STAGE_VERTEX_BIT),
        vkt::makePipelineStageInfo(device->dispatch, vkt::readBinary(std::string("./shaders/render.frag.spv")), VK_SHADER_STAGE_FRAGMENT_BIT)
    };
    pipelineInfo.graphicsPipelineCreateInfo.layout = pipelineLayout;
    pipelineInfo.graphicsPipelineCreateInfo.renderPass = renderPass;//context->refRenderPass();
    pipelineInfo.inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    pipelineInfo.viewportState.pViewports = &reinterpret_cast<::VkViewport&>(viewport);
    pipelineInfo.viewportState.pScissors = &reinterpret_cast<::VkRect2D&>(renderArea);
    pipelineInfo.colorBlendAttachmentStates = { {} }; // Default Blend State
    pipelineInfo.dynamicStates = { VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_VIEWPORT };

    // 
    VkPipeline finalPipeline = {};
    vkh::handleVk(device->dispatch->CreateGraphicsPipelines(device->pipelineCache, 1u, pipelineInfo, nullptr, &finalPipeline));

    // 
    int64_t currSemaphore = -1;
    uint32_t currentBuffer = 0u;
    uint32_t frameCount = 0u;

    // 
    while (!glfwWindowShouldClose(surface.window)) { // 
        glfwPollEvents();

        // 
        int64_t n_semaphore = currSemaphore, c_semaphore = (currSemaphore + 1) % framebuffers.size(); // Next Semaphore
        currSemaphore = (c_semaphore = c_semaphore >= 0 ? c_semaphore : int64_t(framebuffers.size()) + c_semaphore); // Current Semaphore
        (n_semaphore = n_semaphore >= 0 ? n_semaphore : int64_t(framebuffers.size()) + n_semaphore); // Fix for Next Semaphores

        // 
        vkh::handleVk(device->dispatch->WaitForFences(1u, &framebuffers[c_semaphore].waitFence, true, 30ull * 1000ull * 1000ull * 1000ull));
        vkh::handleVk(device->dispatch->ResetFences(1u, &framebuffers[c_semaphore].waitFence));
        vkh::handleVk(device->dispatch->AcquireNextImageKHR(swapchain, std::numeric_limits<uint64_t>::max(), framebuffers[c_semaphore].presentSemaphore, nullptr, &currentBuffer));
        //fw->getDeviceDispatch()->SignalSemaphore(vkh::VkSemaphoreSignalInfo{.semaphore = framebuffers[n_semaphore].semaphore, .value = 1u});

        // 
        vkh::VkClearValue clearValues[2] = { {}, {} };
        clearValues[0].color = vkh::VkClearColorValue{}; clearValues[0].color.float32 = glm::vec4(0.f, 0.f, 0.f, 0.f);
        clearValues[1].depthStencil = VkClearDepthStencilValue{ 1.0f, 0 };

        // Create render submission 
        std::vector<VkSemaphore> waitSemaphores = { framebuffers[currentBuffer].presentSemaphore }, signalSemaphores = { framebuffers[currentBuffer].drawSemaphore };
        std::vector<VkPipelineStageFlags> waitStages = {
            vkh::VkPipelineStageFlags{.eFragmentShader = 1, .eComputeShader = 1, .eTransfer = 1, .eRayTracingShader = 1, .eAccelerationStructureBuild = 1 },
            vkh::VkPipelineStageFlags{.eFragmentShader = 1, .eComputeShader = 1, .eTransfer = 1, .eRayTracingShader = 1, .eAccelerationStructureBuild = 1 }
        };

        // create command buffer (with rewrite)
        VkCommandBuffer& commandBuffer = framebuffers[currentBuffer].commandBuffer;
        if (!commandBuffer) {
            commandBuffer = vkt::createCommandBuffer(device->dispatch, queue->commandPool, false, false); // do reference of cmd buffer

            // Use as present image
            {
                auto aspect = vkh::VkImageAspectFlags{ .eColor = 1u };
                vkt::imageBarrier(commandBuffer, vkt::ImageBarrierInfo{
                    .image = framebuffers[currentBuffer].image,
                    .targetLayout = VK_IMAGE_LAYOUT_GENERAL,
                    .originLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                    .subresourceRange = vkh::VkImageSubresourceRange{ aspect, 0u, 1u, 0u, 1u }
                });
            };

            // Reuse depth as general
            {
                auto aspect = vkh::VkImageAspectFlags{ .eDepth = 1u, .eStencil = 1u };
                vkt::imageBarrier(commandBuffer, vkt::ImageBarrierInfo{
                    .image = manager->depthImage.getImage(),
                    .targetLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                    .originLayout = VK_IMAGE_LAYOUT_GENERAL,
                    .subresourceRange = vkh::VkImageSubresourceRange{ aspect, 0u, 1u, 0u, 1u }
                });
            };

            // 
            device->dispatch->CmdBeginRenderPass(commandBuffer, vkh::VkRenderPassBeginInfo{ .renderPass = renderPass, .framebuffer = framebuffers[currentBuffer].frameBuffer, .renderArea = renderArea, .clearValueCount = 2u, .pClearValues = reinterpret_cast<vkh::VkClearValue*>(&clearValues[0]) }, VK_SUBPASS_CONTENTS_INLINE);
            device->dispatch->CmdSetViewport(commandBuffer, 0u, 1u, viewport);
            device->dispatch->CmdSetScissor(commandBuffer, 0u, 1u, renderArea);
            device->dispatch->CmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, finalPipeline);
            device->dispatch->CmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0u, descriptorSets.size(), descriptorSets.data(), 0u, nullptr);
            device->dispatch->CmdDraw(commandBuffer, 4, 1, 0, 0);
            device->dispatch->CmdEndRenderPass(commandBuffer);
            vkt::commandBarrier(device->dispatch, commandBuffer);

            // Use as present image
            {
                auto aspect = vkh::VkImageAspectFlags{ .eColor = 1u };
                vkt::imageBarrier(commandBuffer, vkt::ImageBarrierInfo{
                    .image = framebuffers[currentBuffer].image,
                    .targetLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                    .originLayout = VK_IMAGE_LAYOUT_GENERAL,
                    .subresourceRange = vkh::VkImageSubresourceRange{ aspect, 0u, 1u, 0u, 1u }
                });
            };

            // Reuse depth as general
            {
                auto aspect = vkh::VkImageAspectFlags{ .eDepth = 1u, .eStencil = 1u };
                vkt::imageBarrier(commandBuffer, vkt::ImageBarrierInfo{
                    .image = manager->depthImage.getImage(),
                    .targetLayout = VK_IMAGE_LAYOUT_GENERAL,
                    .originLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                    .subresourceRange = vkh::VkImageSubresourceRange{ aspect, 0u, 1u, 0u, 1u }
                });
            };

            // 
            device->dispatch->EndCommandBuffer(commandBuffer);
        };

        // Submit command once
        vkh::handleVk(device->dispatch->QueueSubmit(queue->queue, 1u, vkh::VkSubmitInfo{
            .waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size()), .pWaitSemaphores = waitSemaphores.data(), .pWaitDstStageMask = waitStages.data(),
            .commandBufferCount = 1u, .pCommandBuffers = &commandBuffer,
            .signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size()), .pSignalSemaphores = signalSemaphores.data()
        }, framebuffers[currentBuffer].waitFence));

        // 
        waitSemaphores = { framebuffers[c_semaphore].drawSemaphore };
        vkh::handleVk(device->dispatch->QueuePresentKHR(queue->queue, vkh::VkPresentInfoKHR{
            .waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size()), .pWaitSemaphores = waitSemaphores.data(),
            .swapchainCount = 1, .pSwapchains = &swapchain,
            .pImageIndices = &currentBuffer, .pResults = nullptr
        }));



    };


    return 0;
};
