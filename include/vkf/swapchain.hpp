#pragma once // #

#include "./incornation.hpp"
#include "./instance.hpp"
#include "./device.hpp"
#include "./queue.hpp"

// 
namespace vkf {

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

    struct SurfaceWindow {
#ifdef VKT_USE_GLFW
        GLFWwindow* window = nullptr;
#endif

        SurfaceFormat surfaceFormat = {};
        VkExtent2D surfaceSize = VkExtent2D{ 0u, 0u };
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        VkRenderPass renderPass = VK_NULL_HANDLE;
        std::vector<Framebuffer> swapchainBuffers = {};
    };

    //  
    class SwapChain {
    public:
        std::shared_ptr<Instance> instance = {};
        std::shared_ptr<Device> device = {};

        // 
        SurfaceWindow surfaceWindow = {};
        vkf::ImageRegion depthImage = {};

        // 
        SwapChain(){

        }
        SwapChain(std::shared_ptr<Instance> instance, std::shared_ptr<Device> device) : instance(instance), device(device) {

        }
        SwapChain(std::shared_ptr<Device> device) : instance(device->instance), device(device) {
            
        }

#ifdef VKT_USE_GLFW
        //
        inline virtual SurfaceWindow& createWindowOnly(uint32_t WIDTH, uint32_t HEIGHT, const std::string& title = std::string("TestApp")) {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            surfaceWindow.surfaceSize = VkExtent2D{ WIDTH, HEIGHT };
            surfaceWindow.window = glfwCreateWindow(WIDTH, HEIGHT, title.c_str(), nullptr, nullptr);
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
            return surfaceWindow;
        };

        // create window and surface for this application (multi-window not supported)
        inline virtual SurfaceWindow& createWindowSurface(SurfaceWindow& applicationWindow) {
            applicationWindow.surfaceSize = VkExtent2D{ applicationWindow.surfaceSize.width, applicationWindow.surfaceSize.height };
            glfwMakeContextCurrent(nullptr); // CONTEXT-REQUIRED!

            vkh::handleVk(glfwCreateWindowSurface(*instance, applicationWindow.window, nullptr, (VkSurfaceKHR*)&applicationWindow.surface));

            glfwMakeContextCurrent(applicationWindow.window); // CONTEXT-REQUIRED!
            return applicationWindow;
        };

        // create window and surface for this application (multi-window not supported)
        inline virtual SurfaceWindow& createWindowSurface(uint32_t WIDTH, uint32_t HEIGHT, const std::string& title = std::string("TestApp")) {
            return this->createWindowSurface(this->createWindowOnly(WIDTH, HEIGHT, title));
        };

        // create window and surface for this application (multi-window not supported)
        inline virtual SurfaceWindow& createWindowSurface(GLFWwindow* window, uint32_t WIDTH, uint32_t HEIGHT) {
            surfaceWindow.surfaceSize = VkExtent2D{ WIDTH, HEIGHT };
            surfaceWindow.window = window;
            return createWindowSurface(surfaceWindow);
        };
#endif

        VkRenderPass& createRenderPass()
        { // TODO: Render Pass V2
            auto formats = surfaceWindow.surfaceFormat;
            auto render_pass_helper = vkh::VsRenderPassCreateInfoHelper();

            render_pass_helper.addColorAttachment(vkh::VkAttachmentDescription{
                .format = VkFormat(formats.colorFormat),
                .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .finalLayout = VK_IMAGE_LAYOUT_GENERAL
            });

            render_pass_helper.setDepthStencilAttachment(vkh::VkAttachmentDescription{
                .format = VkFormat(formats.depthFormat),
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
            });

            auto dp0 = vkh::VkSubpassDependency{
                .srcSubpass = VK_SUBPASS_EXTERNAL,
                .dstSubpass = 0u,
            };

            auto dp1 = vkh::VkSubpassDependency{
                .srcSubpass = 0u,
                .dstSubpass = VK_SUBPASS_EXTERNAL,
            };

            {
                auto srcStageMask = vkh::VkPipelineStageFlags{ .eColorAttachmentOutput = 1, .eTransfer = 1, .eBottomOfPipe = 1, };   ASSIGN(dp0, srcStageMask);
                auto dstStageMask = vkh::VkPipelineStageFlags{ .eColorAttachmentOutput = 1, };                                       ASSIGN(dp0, dstStageMask);
                auto srcAccessMask = vkh::VkAccessFlags{ .eColorAttachmentWrite = 1 };                                               ASSIGN(dp0, srcAccessMask);
                auto dstAccessMask = vkh::VkAccessFlags{ .eColorAttachmentRead = 1, .eColorAttachmentWrite = 1 };                    ASSIGN(dp0, dstAccessMask);
                auto dependencyFlags = vkh::VkDependencyFlags{ .eByRegion = 1 };                                                      ASSIGN(dp0, dependencyFlags);
            }

            {
                auto srcStageMask = vkh::VkPipelineStageFlags{ .eColorAttachmentOutput = 1 };                                         ASSIGN(dp1, srcStageMask);
                auto dstStageMask = vkh::VkPipelineStageFlags{ .eTopOfPipe = 1, .eColorAttachmentOutput = 1, .eTransfer = 1 };        ASSIGN(dp1, dstStageMask);
                auto srcAccessMask = vkh::VkAccessFlags{ .eColorAttachmentRead = 1, .eColorAttachmentWrite = 1 };                     ASSIGN(dp1, srcAccessMask);
                auto dstAccessMask = vkh::VkAccessFlags{ .eColorAttachmentRead = 1, .eColorAttachmentWrite = 1 };                     ASSIGN(dp1, dstAccessMask);
                auto dependencyFlags = vkh::VkDependencyFlags{ .eByRegion = 1 };                                                       ASSIGN(dp1, dependencyFlags);
            }

            render_pass_helper.addSubpassDependency(dp0);
            render_pass_helper.addSubpassDependency(dp1);

            vkh::handleVk(device->dispatch->CreateRenderPass(render_pass_helper, nullptr, &surfaceWindow.renderPass));
            return surfaceWindow.renderPass;
        }

        virtual SurfaceFormat& getSurfaceFormat()
        {
            const VkPhysicalDevice& gpu = device->physical;
            std::vector<VkSurfaceFormatKHR> surfaceFormats = {};
            vkh::vsGetPhysicalDeviceSurfaceFormatsKHR(instance->dispatch, gpu, surfaceWindow.surface, surfaceFormats);
            const std::vector<VkFormat> preferredFormats = { VK_FORMAT_R16G16B16A16_UNORM, VK_FORMAT_A2B10G10R10_UNORM_PACK32, VK_FORMAT_A2R10G10B10_UNORM_PACK32, VK_FORMAT_R8G8B8A8_SRGB, VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_A8B8G8R8_SRGB_PACK32, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_A8B8G8R8_UNORM_PACK32 };
            VkFormat surfaceColorFormat = surfaceFormats.size() == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED ? VK_FORMAT_R8G8B8A8_SRGB : surfaceFormats[0].format;

            // search preferred surface format support
            bool surfaceFormatFound = false;
            uint32_t surfaceFormatID = 0;
            for (int i = 0; i < preferredFormats.size(); i++)
            {
                if (surfaceFormatFound) break;
                for (int f = 0; f < surfaceFormats.size(); f++)
                {
                    if (surfaceFormats[f].format == preferredFormats[i])
                    {
                        surfaceFormatFound = true;
                        surfaceFormatID = f;
                        break;
                    }
                }
            }

            // get supported color format
            surfaceColorFormat = surfaceFormats[surfaceFormatID].format;
            VkColorSpaceKHR surfaceColorSpace = surfaceFormats[surfaceFormatID].colorSpace;

            // get format properties?
            auto formatProperties = vkh::vsGetPhysicalDeviceFormatProperties(instance->dispatch, gpu, surfaceColorFormat);//gpu.getFormatProperties(surfaceColorFormat);

            // only if these depth formats
            std::vector<VkFormat> depthFormats = {
                VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT,
                VK_FORMAT_D32_SFLOAT,         VK_FORMAT_D16_UNORM };

            // choice supported depth format
            VkFormat surfaceDepthFormat = depthFormats[0];
            for (auto format : depthFormats) {
                auto depthFormatProperties = vkh::vsGetPhysicalDeviceFormatProperties(instance->dispatch, gpu, format);
                if (depthFormatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
                    surfaceDepthFormat = format; break;
                }
            };

            // return format result
            auto& sfd = surfaceWindow.surfaceFormat;
            sfd.colorSpace = surfaceColorSpace;
            sfd.colorFormat = surfaceColorFormat;
            sfd.depthFormat = surfaceDepthFormat;
            sfd.colorFormatProperties = formatProperties; // get properties about format
            return sfd;
        }

        virtual void updateSwapchainFramebuffer(std::shared_ptr<Queue> queue, std::vector<Framebuffer>& swapchainBuffers, VkSwapchainKHR& swapchain, VkRenderPass& renderpass) {
            // The swapchain handles allocating frame images.
            auto& surfaceFormats = getSurfaceFormat();
            auto  gpuMemoryProps = vkh::vsGetPhysicalDeviceMemoryProperties(instance->dispatch, device->physical);//physicalDevice.getMemoryProperties();

            // 
            auto imageUsage = vkh::VkImageUsageFlags{ .eTransferSrc = 1, .eDepthStencilAttachment = 1 };
            auto imageInfoVK = vkh::VkImageCreateInfo{};
            imageInfoVK.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfoVK.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfoVK.flags = VkImageCreateFlagBits{};
            imageInfoVK.pNext = nullptr;
            imageInfoVK.arrayLayers = 1;
            imageInfoVK.extent = VkExtent3D{ surfaceWindow.surfaceSize.width, surfaceWindow.surfaceSize.height, 1u };
            imageInfoVK.format = { surfaceFormats.depthFormat };
            imageInfoVK.imageType = VK_IMAGE_TYPE_2D;
            imageInfoVK.mipLevels = 1;
            imageInfoVK.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfoVK.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfoVK.usage = imageUsage;

            // 
            VmaAllocationCreateInfo allocCreateInfo = {};
            allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

            // 
            vkh::VkImageCreateFlags iflg = {};
            vkt::unlock32(iflg) = 0u;

            // 
            auto aspect = vkh::VkImageAspectFlags{ .eColor = 1 };
            auto dspect = vkh::VkImageAspectFlags{ .eDepth = 1 };

            // 
            {
                auto aspect = vkh::VkImageAspectFlags{ .eDepth = 1, .eStencil = 1 };
                auto depuse = vkh::VkImageUsageFlags{ .eTransferDst = 1, .eSampled = 1, .eDepthStencilAttachment = 1 };
                depuse.eDepthStencilAttachment = 1;

                vkh::VkImageCreateInfo imageCreate = {};
                imageCreate.format = surfaceFormats.depthFormat,
                imageCreate.extent = vkh::VkExtent3D{ surfaceWindow.surfaceSize.width, surfaceWindow.surfaceSize.height, 1u },
                imageCreate.usage = depuse;

                vkh::VkImageViewCreateInfo imageViewCreate = {};
                imageViewCreate.format = surfaceFormats.depthFormat,
                imageViewCreate.subresourceRange = vkh::VkImageSubresourceRange{.aspectMask = aspect };

                vkf::VmaMemoryInfo memInfo = {};
                this->depthImage = vkf::ImageRegion(std::make_shared<vkf::VmaImageAllocation>(this->device->allocator, imageCreate, memInfo), imageViewCreate, VK_IMAGE_LAYOUT_GENERAL);

                vkh::handleVk(device->dispatch->CreateSampler(vkh::VkSamplerCreateInfo{
                    .magFilter = VK_FILTER_LINEAR,
                    .minFilter = VK_FILTER_LINEAR,
                    .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                    .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
                }, nullptr, &this->depthImage.refSampler()));
            };

            // 
            auto swapchainImages = vkh::vsGetSwapchainImagesKHR(device->dispatch, swapchain);
            swapchainBuffers.resize(swapchainImages.size());
            for (int i = 0; i < swapchainImages.size(); i++)
            { // create framebuffers
                std::array<VkImageView, 2> views = {}; // predeclare views
                swapchainBuffers[i].image = swapchainImages[i];

                // Image Views
                auto flags = VkImageViewCreateFlags{};
                auto aspect = vkh::VkImageAspectFlags{ .eColor = 1 };

                auto imageViewCreate = vkh::VkImageViewCreateInfo{};
                imageViewCreate.flags = {},
                imageViewCreate.image = swapchainImages[i],
                imageViewCreate.viewType = VK_IMAGE_VIEW_TYPE_2D,
                imageViewCreate.format = surfaceFormats.colorFormat,
                imageViewCreate.components = vkh::VkComponentMapping{},
                imageViewCreate.subresourceRange = vkh::VkImageSubresourceRange{ aspect, 0, 1, 0, 1 };

                vkh::handleVk(device->dispatch->CreateImageView(imageViewCreate, nullptr, &views[0]));
                views[1] = this->depthImage.getImageView(); // depth view

                // 
                vkh::handleVk(device->dispatch->CreateFramebuffer(vkh::VkFramebufferCreateInfo{ .flags = {}, .renderPass = renderpass, .attachmentCount = uint32_t(views.size()), .pAttachments = views.data(), .width = surfaceWindow.surfaceSize.width, .height = surfaceWindow.surfaceSize.height, .layers = 1u }, nullptr, &swapchainBuffers[i].frameBuffer));
            };

            if (queue) {
                queue->submitOnce([&, this](VkCommandBuffer& cmd) {
                    this->device->dispatch->CmdClearDepthStencilImage(cmd, this->depthImage.transfer(cmd), this->depthImage.getImageLayout(), vkh::VkClearDepthStencilValue{ .depth = 1.0f, .stencil = 0 }, 1u, depthImage.getImageSubresourceRange());
                    for (int i = 0; i < swapchainImages.size(); i++) {
                        auto aspect = vkh::VkImageAspectFlags{ .eColor = 1u };
                        vkt::imageBarrier(cmd, vkt::ImageBarrierInfo{
                            .image = swapchainImages[i],
                            .targetLayout = VK_IMAGE_LAYOUT_GENERAL,
                            .originLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                            .subresourceRange = vkh::VkImageSubresourceRange{ aspect, 0u, 1u, 0u, 1u }
                        });
                    };
                });
            };
        };

        virtual VkSwapchainKHR& createSwapchain()
        {
            auto& formats = getSurfaceFormat();
            auto surfaceCapabilities = vkh::vsGetPhysicalDeviceSurfaceCapabilitiesKHR(instance->dispatch, device->physical, surfaceWindow.surface);
            auto surfacePresentModes = vkh::vsGetPhysicalDeviceSurfacePresentModesKHR(instance->dispatch, device->physical, surfaceWindow.surface);

            // check the surface width/height.
            if (!(surfaceCapabilities.currentExtent.width == -1 || surfaceCapabilities.currentExtent.height == -1))
            {
                surfaceWindow.surfaceSize = surfaceCapabilities.currentExtent;
            }

            // get supported present mode, but prefer mailBox
            auto presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
            std::vector<VkPresentModeKHR> priorityModes = { VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_FIFO_RELAXED_KHR, VK_PRESENT_MODE_FIFO_KHR };

            bool found = false; // 
            for (auto pm : priorityModes) {
                if (found) break;
                for (auto sfm : surfacePresentModes) { if (pm == sfm) { presentMode = pm; found = true; break; } }
            };

            // swapchain info
            auto imageUsage = vkh::VkImageUsageFlags{ .eColorAttachment = 1 };
            auto swapchainCreateInfo = vkh::VkSwapchainCreateInfoKHR{};
            swapchainCreateInfo.surface = surfaceWindow.surface;
            swapchainCreateInfo.minImageCount = std::min(surfaceCapabilities.maxImageCount, 3u);
            swapchainCreateInfo.imageFormat = formats.colorFormat;
            swapchainCreateInfo.imageColorSpace = formats.colorSpace;
            swapchainCreateInfo.imageExtent = surfaceWindow.surfaceSize;
            swapchainCreateInfo.imageArrayLayers = 1;
            swapchainCreateInfo.imageUsage = imageUsage;
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
            swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            swapchainCreateInfo.presentMode = presentMode;
            swapchainCreateInfo.clipped = true;

            // create swapchain
            vkh::handleVk(device->dispatch->CreateSwapchainKHR(swapchainCreateInfo, nullptr, &surfaceWindow.swapchain));
            return surfaceWindow.swapchain;
        };

        virtual std::vector<Framebuffer>& createSwapchainFramebuffer(std::shared_ptr<Queue> queue, VkSwapchainKHR& swapchain, VkRenderPass& renderpass) { // framebuffers vector
            updateSwapchainFramebuffer(queue, surfaceWindow.swapchainBuffers, swapchain, renderpass);
            for (int i = 0; i < surfaceWindow.swapchainBuffers.size(); i++)
            { // create semaphore
                VkSemaphoreTypeCreateInfo timeline = {};
                timeline.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
                timeline.initialValue = i;

                // 
                vkh::handleVk(device->dispatch->CreateSemaphore(vkh::VkSemaphoreCreateInfo{}, nullptr, &surfaceWindow.swapchainBuffers[i].drawSemaphore));
                vkh::handleVk(device->dispatch->CreateSemaphore(vkh::VkSemaphoreCreateInfo{}, nullptr, &surfaceWindow.swapchainBuffers[i].computeSemaphore));
                vkh::handleVk(device->dispatch->CreateSemaphore(vkh::VkSemaphoreCreateInfo{}, nullptr, &surfaceWindow.swapchainBuffers[i].presentSemaphore));
                vkh::handleVk(device->dispatch->CreateFence(vkh::VkFenceCreateInfo{ .flags = {1} }, nullptr, &surfaceWindow.swapchainBuffers[i].waitFence));
            };
            return surfaceWindow.swapchainBuffers;
        };

        virtual void updateSwapchainFramebuffer(std::shared_ptr<Queue> queue) {
            this->updateSwapchainFramebuffer(queue, surfaceWindow.swapchainBuffers, surfaceWindow.swapchain, surfaceWindow.renderPass);
        };

        virtual std::vector<Framebuffer>& createSwapchainFramebuffer(std::shared_ptr<Queue> queue) {
            return this->createSwapchainFramebuffer(queue, surfaceWindow.swapchain, surfaceWindow.renderPass);
        };
    };
    
};
