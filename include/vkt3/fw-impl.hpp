#pragma once // #
// 
#include "./fw.hpp"

#define VKT_IMPLEMENT_FRAMEWORK

// TODO: FULL REWRITE OF THAT "PROJECT"!!!
namespace vkt
{

#ifdef VKT_IMPLEMENT_FRAMEWORK
#ifdef VKT_ENABLE_GLFW_SUPPORT

    SurfaceFormat& GPUFramework::getSurfaceFormat(VkPhysicalDevice gpu)
    {
        //auto surfaceFormats = gpu.getSurfaceFormatsKHR(applicationWindow.surface);
        auto surfaceFormats = vkh::vsGetPhysicalDeviceSurfaceFormatsKHR(this->instanceDispatch, gpu, applicationWindow.surface);

        //const std::vector<VkFormat> preferredFormats = { VkFormat::eR16G16B16A16Unorm, VkFormat::eA2B10G10R10UnormPack32, VkFormat::eA2R10G10B10UintPack32, VkFormat::eR8G8B8A8Srgb, VkFormat::eB8G8R8A8Srgb, VkFormat::eA8B8G8R8SrgbPack32, VkFormat::eR8G8B8A8Unorm, VkFormat::eB8G8R8A8Unorm, VkFormat::eA8B8G8R8UnormPack32 };
        const std::vector<VkFormat> preferredFormats = { VK_FORMAT_R16G16B16A16_UNORM, VK_FORMAT_A2B10G10R10_UNORM_PACK32, VK_FORMAT_A2R10G10B10_UNORM_PACK32, VK_FORMAT_R8G8B8A8_SRGB, VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_A8B8G8R8_SRGB_PACK32, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_A8B8G8R8_UNORM_PACK32 };

        VkFormat surfaceColorFormat =
            surfaceFormats.size() == 1 &&
            surfaceFormats[0].format == VK_FORMAT_UNDEFINED
            ? VK_FORMAT_R8G8B8A8_SRGB
            : surfaceFormats[0].format;

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
        auto formatProperties = vkh::vsGetPhysicalDeviceFormatProperties(this->instanceDispatch, gpu, surfaceColorFormat);//gpu.getFormatProperties(surfaceColorFormat);

        // only if these depth formats
        std::vector<VkFormat> depthFormats = {
            VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT,
            VK_FORMAT_D32_SFLOAT,         VK_FORMAT_D16_UNORM };

        // choice supported depth format
        VkFormat surfaceDepthFormat = depthFormats[0];
        for (auto format : depthFormats) {
            auto depthFormatProperties = vkh::vsGetPhysicalDeviceFormatProperties(this->instanceDispatch, gpu, format);
            if (depthFormatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
                surfaceDepthFormat = format; break;
            }
        };

        // return format result
        auto& sfd = applicationWindow.surfaceFormat;
        sfd.colorSpace = surfaceColorSpace;
        sfd.colorFormat = surfaceColorFormat;
        sfd.depthFormat = surfaceDepthFormat;
        sfd.colorFormatProperties = formatProperties; // get properties about format

        return sfd;
    }

    VkRenderPass& GPUFramework::createRenderPass()
    { // TODO: Render Pass V2
        auto formats = applicationWindow.surfaceFormat;
        auto render_pass_helper = vkh::VsRenderPassCreateInfoHelper();

        render_pass_helper.addColorAttachment(vkh::VkAttachmentDescription{
            .format = VkFormat(formats.colorFormat),
            .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_GENERAL,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
            });

        render_pass_helper.setDepthStencilAttachment(vkh::VkAttachmentDescription{
            .format = VkFormat(formats.depthFormat),
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            .finalLayout = VK_IMAGE_LAYOUT_GENERAL
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

        vkh::handleVk(this->deviceDispatch->CreateRenderPass(render_pass_helper, nullptr, &applicationWindow.renderPass));
        return applicationWindow.renderPass;
    }


    void GPUFramework::updateSwapchainFramebuffer(std::vector<Framebuffer>& swapchainBuffers, VkSwapchainKHR& swapchain, VkRenderPass& renderpass) {
        // The swapchain handles allocating frame images.
        auto& surfaceFormats = getSurfaceFormat(this->physicalDevice);
        auto  gpuMemoryProps = vkh::vsGetPhysicalDeviceMemoryProperties(this->instanceDispatch, physicalDevice);//physicalDevice.getMemoryProperties();

        //
        auto imageUsage = vkh::VkImageUsageFlags{ .eTransferSrc = 1, .eDepthStencilAttachment = 1 };
        auto imageInfoVK = vkh::VkImageCreateInfo{};
        imageInfoVK.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfoVK.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfoVK.flags = VkImageCreateFlagBits{};
        imageInfoVK.pNext = nullptr;
        imageInfoVK.arrayLayers = 1;
        imageInfoVK.extent = VkExtent3D{ applicationWindow.surfaceSize.width, applicationWindow.surfaceSize.height, 1u };
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
            depuse = depuse | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

            vkt::VmaMemoryInfo memInfo = {};
            this->depthImage = vkt::ImageRegion(std::make_shared<vkt::VmaImageAllocation>(this->getAllocator(), vkh::VkImageCreateInfo{}.also([=](vkh::VkImageCreateInfo* it) {
                it->format = surfaceFormats.depthFormat,
                    it->extent = vkh::VkExtent3D{ applicationWindow.surfaceSize.width, applicationWindow.surfaceSize.height, 1u },
                    it->usage = depuse;
                return it;
                }), memInfo), vkh::VkImageViewCreateInfo{}.also([=](vkh::VkImageViewCreateInfo* it) {
                    it->format = surfaceFormats.depthFormat,
                        it->subresourceRange = vkh::VkImageSubresourceRange{ .aspectMask = aspect };
                    return it;
                    }), VK_IMAGE_LAYOUT_GENERAL);

            vkh::handleVk(this->deviceDispatch->CreateSampler(vkh::VkSamplerCreateInfo{
                .magFilter = VK_FILTER_LINEAR,
                .minFilter = VK_FILTER_LINEAR,
                .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
                }, nullptr, &this->depthImage.refSampler()));
        };

        //
        auto swapchainImages = vkh::vsGetSwapchainImagesKHR(this->deviceDispatch, swapchain);
        swapchainBuffers.resize(swapchainImages.size());
        for (int i = 0; i < swapchainImages.size(); i++)
        { // create framebuffers
            std::array<VkImageView, 2> views = {}; // predeclare views
            swapchainBuffers[i].image = swapchainImages[i];

            // Image Views
            auto flags = VkImageViewCreateFlags{};
            auto aspect = vkh::VkImageAspectFlags{ .eColor = 1 };
            vkh::handleVk(deviceDispatch->CreateImageView(vkh::VkImageViewCreateInfo{}.also([=](vkh::VkImageViewCreateInfo* it) {
                it->flags = {},
                    it->image = swapchainImages[i],
                    it->viewType = VK_IMAGE_VIEW_TYPE_2D,
                    it->format = surfaceFormats.colorFormat,
                    it->components = vkh::VkComponentMapping{},
                    it->subresourceRange = vkh::VkImageSubresourceRange{ aspect, 0, 1, 0, 1 };
                return it;
                }), nullptr, &views[0]));
            views[1] = this->depthImage.getImageView(); // depth view

            //
            vkh::handleVk(deviceDispatch->CreateFramebuffer(vkh::VkFramebufferCreateInfo{ .flags = {}, .renderPass = renderpass, .attachmentCount = uint32_t(views.size()), .pAttachments = views.data(), .width = applicationWindow.surfaceSize.width, .height = applicationWindow.surfaceSize.height, .layers = 1u }, nullptr, &swapchainBuffers[i].frameBuffer));
        };

        vkt::submitOnce(this->deviceDispatch, this->queue, this->commandPool, [&, this](VkCommandBuffer& cmd) {
            this->getDeviceDispatch()->CmdClearDepthStencilImage(cmd, this->depthImage.transfer(cmd), this->depthImage.getImageLayout(), vkh::VkClearDepthStencilValue{ .depth = 1.0f, .stencil = 0 }, 1u, depthImage.getImageSubresourceRange());
            for (int i = 0; i < swapchainImages.size(); i++) {
                vkt::imageBarrier(cmd, vkt::ImageBarrierInfo{
                    .image = swapchainImages[i],
                    .targetLayout = VK_IMAGE_LAYOUT_GENERAL,
                    .originLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                    .subresourceRange = vkh::VkImageSubresourceRange{ {}, 0u, 1u, 0u, 1u }.also([=](auto* it) {
                        auto aspect = vkh::VkImageAspectFlags{.eColor = 1u };
                        it->aspectMask = aspect;
                        return it;
                    })
                    });
            }

            vkt::imageBarrier(cmd, vkt::ImageBarrierInfo{
                .image = this->depthImage.getImage(),
                .targetLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                .originLayout = VK_IMAGE_LAYOUT_GENERAL,
                .subresourceRange = vkh::VkImageSubresourceRange{ {}, 0u, 1u, 0u, 1u }.also([=](auto* it) {
                    auto aspect = vkh::VkImageAspectFlags{.eDepth = 1u, .eStencil = 1u };
                    it->aspectMask = aspect;
                    return it;
                })
                });
            });
    }

    VkSwapchainKHR GPUFramework::createSwapchain()
    {
        auto& formats = getSurfaceFormat(this->physicalDevice);
        auto& surface = applicationWindow.surface;
        auto surfaceCapabilities = vkh::vsGetPhysicalDeviceSurfaceCapabilitiesKHR(this->instanceDispatch, physicalDevice, surface);
        auto surfacePresentModes = vkh::vsGetPhysicalDeviceSurfacePresentModesKHR(this->instanceDispatch, physicalDevice, surface);

        // check the surface width/height.
        if (!(surfaceCapabilities.currentExtent.width == -1 ||
            surfaceCapabilities.currentExtent.height == -1))
        {
            applicationWindow.surfaceSize = surfaceCapabilities.currentExtent;
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
        swapchainCreateInfo.surface = surface;
        swapchainCreateInfo.minImageCount = std::min(surfaceCapabilities.maxImageCount, 3u);
        swapchainCreateInfo.imageFormat = formats.colorFormat;
        swapchainCreateInfo.imageColorSpace = formats.colorSpace;
        swapchainCreateInfo.imageExtent = applicationWindow.surfaceSize;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = imageUsage;
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode = presentMode;
        swapchainCreateInfo.clipped = true;

        // create swapchain
        vkh::handleVk(this->deviceDispatch->CreateSwapchainKHR(swapchainCreateInfo, nullptr, &applicationWindow.swapchain));
        return applicationWindow.swapchain;
    };

    std::vector<Framebuffer>& GPUFramework::createSwapchainFramebuffer(VkSwapchainKHR& swapchain, VkRenderPass& renderpass) { // framebuffers vector
        auto& swapchainBuffers = applicationWindow.swapchainBuffers;
        updateSwapchainFramebuffer(swapchainBuffers, swapchain, renderpass);
        for (int i = 0; i < swapchainBuffers.size(); i++)
        { // create semaphore
            VkSemaphoreTypeCreateInfo timeline = {};
            timeline.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
            timeline.initialValue = i;

            // 
            vkh::handleVk(this->deviceDispatch->CreateSemaphore(vkh::VkSemaphoreCreateInfo{}, nullptr, &swapchainBuffers[i].drawSemaphore));
            vkh::handleVk(this->deviceDispatch->CreateSemaphore(vkh::VkSemaphoreCreateInfo{}, nullptr, &swapchainBuffers[i].computeSemaphore));
            vkh::handleVk(this->deviceDispatch->CreateSemaphore(vkh::VkSemaphoreCreateInfo{}, nullptr, &swapchainBuffers[i].presentSemaphore));
            vkh::handleVk(this->deviceDispatch->CreateFence(vkh::VkFenceCreateInfo{ .flags = {1} }, nullptr, &swapchainBuffers[i].waitFence));
        };
        return swapchainBuffers;
    }
#endif


    GPUFramework& GPUFramework::operator=(vkt::uni_ptr<GPUFramework> fw) {
        if (fw.ptr()) {
            this->fence = fw->fence;
            this->queue = fw->queue;
            this->device = fw->device;
            this->instance = fw->instance;
            this->descriptorPool = fw->descriptorPool;
            this->physicalDevice = fw->physicalDevice;
            this->commandPool = fw->commandPool;
            //this->renderPass = fw->renderPass;
            this->depthImage = fw->depthImage;
            this->pipelineCache = fw->pipelineCache;
            this->memoryProperties = fw->memoryProperties;
            this->messenger = fw->messenger;
            this->allocator = fw->allocator;
            this->queueFamilyIndex = fw->queueFamilyIndex;
            this->instanceVersion = fw->instanceVersion;
            this->vmaDepthImageAllocation = fw->vmaDepthImageAllocation;
            this->vmaDepthImageAllocationInfo = fw->vmaDepthImageAllocationInfo;
            this->physicalDevices = fw->physicalDevices;
            this->queueFamilyIndices = fw->queueFamilyIndices;
            this->instanceDispatch = fw->instanceDispatch;
            this->deviceDispatch = fw->deviceDispatch;
        };
        return *this;
    };

    VkDevice& GPUFramework::createDevice(bool isComputePrior, std::string shaderPath, bool enableAdvancedAcceleration) {

        // default device extensions
        std::vector<const char*> wantedDeviceExtensions = {
            "VK_EXT_swapchain_colorspace",
            "VK_EXT_external_memory_host",
            "VK_EXT_queue_family_foreign",
            "VK_EXT_sample_locations",
            "VK_EXT_conservative_rasterization",
            "VK_EXT_hdr_metadata",
            "VK_EXT_transform_feedback",
            "VK_EXT_extended_dynamic_state",

            // 
            "VK_EXT_descriptor_indexing",
            "VK_KHR_descriptor_update_template",
            "VK_KHR_push_descriptor",

            // 
            "VK_AMD_gcn_shader",
            "VK_AMD_shader_info",
            "VK_AMD_texture_gather_bias_lod",
            "VK_AMD_shader_image_load_store_lod",
            "VK_AMD_shader_trinary_minmax",
            "VK_AMD_draw_indirect_count",

            // Would To Be Have Multi-Vector Solution
            "VK_AMD_shader_explicit_vertex_parameter",
            "VK_NV_fragment_shader_barycentric",

            // 
            "VK_EXT_sampler_filter_minmax",
            "VK_KHR_sampler_ycbcr_conversion",
            "VK_KHR_sampler_mirror_clamp_to_edge",
            "VK_KHR_imageless_framebuffer",

            // 
            "VK_KHR_bind_memory2",
            "VK_KHR_maintenance1",
            "VK_KHR_maintenance2",
            "VK_KHR_maintenance3",
            "VK_KHR_driver_properties",
            "VK_KHR_image_format_list",
            "VK_KHR_dedicated_allocation",
            "VK_KHR_imageless_framebuffer",
            "VK_KHR_shader_draw_parameters",
            "VK_KHR_get_memory_requirements2",

            // 
            "VK_EXT_scalar_block_layout",
            "VK_KHR_storage_buffer_storage_class",
            "VK_KHR_relaxed_block_layout",
            "VK_KHR_variable_pointers",
            "VK_AMD_buffer_marker",

            // 
            "VK_EXT_subgroup_size_control",
            "VK_KHR_shader_subgroup_extended_types",

            // 
            "VK_KHR_16bit_storage",
            "VK_KHR_8bit_storage",
            "VK_AMD_gpu_shader_int16",
            "VK_AMD_gpu_shader_half_float",
            "VK_KHX_shader_explicit_arithmetic_types",
            "VK_KHR_shader_atomic_int64",
            "VK_KHR_shader_float16_int8",
            "VK_KHR_shader_float_controls",
            "VK_EXT_shader_atomic_float",

            // 
            "VK_KHR_incremental_present",
            "VK_KHR_ray_tracing",
            "VK_KHR_ray_query",
            "VK_KHR_pipeline_library",
            "VK_KHR_deferred_host_operations",
            "VK_KHR_buffer_device_address",
            "VK_KHR_vulkan_memory_model",
            "VK_KHR_shader_clock",
            "VK_KHR_swapchain",
            "VK_KHR_surface",
            "VK_KHR_display",

            // 
            "VK_NV_device_diagnostic_checkpoints",
            "VK_NV_compute_shader_derivatives",
            "VK_NV_corner_sampled_image",
            "VK_NV_shader_image_footprint",
            "VK_NV_shader_subgroup_partitioned",
            "VK_NV_shader_sm_builtins",
            "VK_NV_ray_tracing",

            //
            "VK_KHR_external_fence",
            "VK_KHR_external_fence_capabilities",
            "VK_KHR_external_fence_win32",
            //"VK_KHR_external_fence_fd",

            // 
            "VK_KHR_external_memory",
            "VK_KHR_external_memory_capabilities",
            "VK_KHR_external_memory_win32",
            //"VK_KHR_external_memory_fd",

            // 
            "VK_KHR_external_semaphore",
            "VK_KHR_external_semaphore_capabilities",
            "VK_KHR_external_semaphore_win32",
            //"VK_KHR_external_semaphore_fd",

            // 
            "VK_NV_external_memory",
            "VK_NV_external_memory_capabilities",
            "VK_NV_external_memory_win32",

            //
            "VK_NVX_image_view_handle",
            "VK_EXT_validation_cache",
            "VK_EXT_validation_features",
            "VK_EXT_validation_flags",
            "VK_EXT_debug_marker",
            "VK_EXT_debug_report",
            "VK_EXT_debug_utils",

            //
            "VK_KHR_buffer_device_address",
            //"VK_EXT_buffer_device_address",
            "VK_NVX_image_view_handle"
        };

        // default device layers
        std::vector<const char*> wantedDeviceValidationLayers = {
            "VK_LAYER_AMD_switchable_graphics"
            "VK_LAYER_KHRONOS_validation",
            "VK_LAYER_LUNARG_assistant_layer",
            "VK_LAYER_LUNARG_standard_validation",
            "VK_LAYER_LUNARG_parameter_validation",
            "VK_LAYER_LUNARG_core_validation",

            //"VK_LAYER_LUNARG_api_dump",
            //"VK_LAYER_LUNARG_object_tracker",
            //"VK_LAYER_LUNARG_device_simulation",
            "VK_LAYER_GOOGLE_threading",
            "VK_LAYER_GOOGLE_unique_objects"
            "VK_LAYER_RENDERDOC_Capture"
        };



        // use extensions
        auto deviceExtensions = std::vector<const char*>();
        auto gpuExtensions = vkh::vsEnumerateDeviceExtensionProperties(this->instanceDispatch, physicalDevice); // TODO: vkh helper for getting
        for (auto w : wantedDeviceExtensions) {
            for (auto i : gpuExtensions) {
                if (std::string(i.extensionName).compare(w) == 0) {
                    deviceExtensions.emplace_back(w); break;
                };
            };
        };

        // use layers
        auto deviceLayers = std::vector<const char*>();
        auto gpuLayers = vkh::vsEnumerateDeviceLayerProperties(this->instanceDispatch, physicalDevice); // TODO: vkh helper for getting
        for (auto w : wantedDeviceValidationLayers) {
            for (auto i : gpuLayers) {
                if (std::string(i.layerName).compare(w) == 0) {
                    deviceLayers.emplace_back(w); break;
                };
            };
        };

        // 
        this->usedDeviceExtensions = deviceExtensions;
        this->usedDeviceLayers = deviceLayers;

        // 
        this->gExtendedDynamic.pNext = &this->gAtomicFloat;
        this->gBarycentric.pNext = &this->gExtendedDynamic;
        this->gRayTracing.pNext = &this->gBarycentric;
        this->gTrasformFeedback.pNext = &this->gRayTracing;
        this->gDeviceAddress.pNext = &this->gTrasformFeedback;
        this->gTexelBufferAligment.pNext = &this->gDeviceAddress;
        this->gFloat16U8.pNext = &this->gTexelBufferAligment;
        this->gStorage8.pNext = &this->gFloat16U8;
        this->gStorage16.pNext = &this->gStorage8;
        this->gDescIndexing.pNext = &this->gStorage16;
        this->gFeatures.pNext = &this->gDescIndexing;

        // 
        vkh::vsGetPhysicalDeviceFeatures2(this->instanceDispatch, physicalDevice, this->gFeatures);
        vkh::vsGetPhysicalDeviceProperties2(this->instanceDispatch, physicalDevice, this->gProperties);
        vkh::vsGetPhysicalDeviceMemoryProperties2(this->instanceDispatch, physicalDevice, this->memoryProperties);

        // get features and queue family properties
        auto gpuQueueProps = vkh::vsGetPhysicalDeviceQueueFamilyProperties(this->instanceDispatch, physicalDevice); // TODO: vkh helper for getting

        // queue family initial
        float priority = 1.0f;
        int32_t computeFamilyIndex = -1, graphicsFamilyIndex = -1;
        auto queueCreateInfos = std::vector<vkh::VkDeviceQueueCreateInfo>();

#ifdef VKT_ENABLE_GLFW_SUPPORT
        for (auto queuefamily : gpuQueueProps) {
            graphicsFamilyIndex++;

            VkBool32 support = false;
            //vkh::handleVk(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, graphicsFamilyIndex, surface(), &support));
            const auto& surface = this->surface();
            if (surface) {
                vkh::handleVk(this->instanceDispatch->GetPhysicalDeviceSurfaceSupportKHR(physicalDevice, graphicsFamilyIndex, surface, &support));
            };
            if (queuefamily.queueFlags.eCompute && queuefamily.queueFlags.eGraphics && (support || !surface)) {
                queueCreateInfos.push_back(vkh::VkDeviceQueueCreateInfo{ .queueFamilyIndex = uint32_t(computeFamilyIndex), .queueCount = 1, .pQueuePriorities = &priority });
                queueFamilyIndices.push_back(uint32_t(graphicsFamilyIndex));
                queueCreateInfos.back().queueFamilyIndex = uint32_t(graphicsFamilyIndex);
                break;
            };
        };
#else
        for (auto queuefamily : gpuQueueProps) {
            computeFamilyIndex++;
            if (queuefamily.queueFlags.eCompute) { // TODO: vkh helper for getting, bitfeilds support
                queueCreateInfos.push_back(vkh::VkDeviceQueueCreateInfo{ .queueFamilyIndex = uint32_t(computeFamilyIndex), .queueCount = 1, .pQueuePriorities = &priority });
                queueFamilyIndices.push_back(uint32_t(computeFamilyIndex));
                queueCreateInfos.back().queueFamilyIndex = uint32_t(computeFamilyIndex);
                break;
            };
        };
#endif

        // return device with queue pointer
        const uint32_t qptr = 0;
        if ((this->usedQueueCreateInfos = queueCreateInfos).size() > 0) {
            this->queueFamilyIndex = queueFamilyIndices[qptr];
            this->deviceDispatch = std::make_shared<xvk::Device>(this->instanceDispatch, this->physicalDevice, (deviceCreate = vkh::VkDeviceCreateInfo{
                .pNext = reinterpret_cast<VkPhysicalDeviceFeatures2*>(&gFeatures),
                .queueCreateInfoCount = uint32_t(this->usedQueueCreateInfos.size()),
                .pQueueCreateInfos = reinterpret_cast<vkh::VkDeviceQueueCreateInfo*>(this->usedQueueCreateInfos.data()),
                .enabledLayerCount = uint32_t(this->usedDeviceLayers.size()),
                .ppEnabledLayerNames = this->usedDeviceLayers.data(),
                .enabledExtensionCount = uint32_t(this->usedDeviceExtensions.size()),
                .ppEnabledExtensionNames = this->usedDeviceExtensions.data(),
                //.pEnabledFeatures = &(VkPhysicalDeviceFeatures&)(gFeatures.features)
                }));
            vkh::handleVk(this->deviceDispatch->CreatePipelineCache(vkh::VkPipelineCacheCreateInfo(), nullptr, &this->pipelineCache));
            this->device = this->deviceDispatch->handle;
            vkt::vkGlobal::device = this->deviceDispatch.get_shared();
            //vkt::vkGlobal::volkDevice();
        };

        //
        auto resetFlag = vkh::VkCommandPoolCreateFlags{ .eResetCommandBuffer = 1 };
        this->deviceDispatch->GetDeviceQueue(queueFamilyIndex, 0u, &this->queue);
        vkh::handleVk(this->deviceDispatch->CreateFence(vkh::VkFenceCreateInfo{}, nullptr, &this->fence));
        vkh::handleVk(this->deviceDispatch->CreateCommandPool(vkh::VkCommandPoolCreateInfo{ .flags = resetFlag, .queueFamilyIndex = queueFamilyIndex }, nullptr, &this->commandPool));

        // REMAP WITH XVK AGAIN!
        func.vkAllocateMemory = this->deviceDispatch->vkAllocateMemory;
        func.vkBindBufferMemory = this->deviceDispatch->vkBindBufferMemory;
        func.vkBindBufferMemory2KHR = this->deviceDispatch->vkBindBufferMemory2;
        func.vkBindImageMemory = this->deviceDispatch->vkBindImageMemory;
        func.vkBindImageMemory2KHR = this->deviceDispatch->vkBindImageMemory2;
        func.vkCmdCopyBuffer = this->deviceDispatch->vkCmdCopyBuffer;
        func.vkCreateBuffer = this->deviceDispatch->vkCreateBuffer;
        func.vkCreateImage = this->deviceDispatch->vkCreateImage;
        func.vkDestroyBuffer = this->deviceDispatch->vkDestroyBuffer;
        func.vkDestroyImage = this->deviceDispatch->vkDestroyImage;
        func.vkFlushMappedMemoryRanges = this->deviceDispatch->vkFlushMappedMemoryRanges;
        func.vkFreeMemory = this->deviceDispatch->vkFreeMemory;
        func.vkGetBufferMemoryRequirements = this->deviceDispatch->vkGetBufferMemoryRequirements;
        func.vkGetBufferMemoryRequirements2KHR = this->deviceDispatch->vkGetBufferMemoryRequirements2;
        func.vkGetImageMemoryRequirements = this->deviceDispatch->vkGetImageMemoryRequirements;
        func.vkGetImageMemoryRequirements2KHR = this->deviceDispatch->vkGetImageMemoryRequirements2;
        func.vkGetPhysicalDeviceMemoryProperties = this->instanceDispatch->vkGetPhysicalDeviceMemoryProperties;
        func.vkGetPhysicalDeviceMemoryProperties2KHR = this->instanceDispatch->vkGetPhysicalDeviceMemoryProperties2;
        func.vkGetPhysicalDeviceProperties = this->instanceDispatch->vkGetPhysicalDeviceProperties;
        func.vkInvalidateMappedMemoryRanges = this->deviceDispatch->vkInvalidateMappedMemoryRanges;
        func.vkMapMemory = this->deviceDispatch->vkMapMemory;
        func.vkUnmapMemory = this->deviceDispatch->vkUnmapMemory;

        // 
        VmaAllocatorCreateInfo vma_info = {};
        vma_info.pVulkanFunctions = &func;
        vma_info.device = this->device;
        vma_info.instance = this->instance;
        vma_info.physicalDevice = this->physicalDevice;
        vma_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        vkh::handleVk(vmaCreateAllocator(&vma_info, &this->allocator));

        // Manually Create Descriptor Pool
        auto dps = std::vector<vkh::VkDescriptorPoolSize>{
            vkh::VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = 256u},
            vkh::VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, .descriptorCount = 1024u},
            vkh::VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_SAMPLER, .descriptorCount = 1024u},
            vkh::VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV, .descriptorCount = 256u},
            vkh::VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, .descriptorCount = 256u},
            vkh::VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = 1024u},
            vkh::VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = 256u},
            vkh::VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, .descriptorCount = 256u},
            vkh::VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, .descriptorCount = 256u}
        };

        //
        vkh::handleVk(this->deviceDispatch->CreateDescriptorPool(vkh::VkDescriptorPoolCreateInfo{
            .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT | VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT ,
            .maxSets = 256u, .poolSizeCount = static_cast<uint32_t>(dps.size()), .pPoolSizes = dps.data()
        }, nullptr, &this->descriptorPool));

        return this->device;
    };


    VkInstance& GPUFramework::createInstance() {
        // instance extensions
        std::vector<const char*> wantedExtensions = {
            "VK_KHR_get_physical_device_properties2",
            "VK_KHR_get_surface_capabilities2",

            "VK_EXT_direct_mode_display",
            "VK_EXT_swapchain_colorspace",

            "VK_KHR_swapchain",
            "VK_KHR_surface",
            "VK_KHR_display",

            "VK_EXT_debug_marker",
            "VK_EXT_debug_report",
            "VK_EXT_debug_utils",

            "VK_NV_device_diagnostic_checkpoints"
        };

        // instance layers
        std::vector<const char*> wantedLayers = {
            "VK_LAYER_KHRONOS_validation",
            "VK_LAYER_LUNARG_assistant_layer",
            "VK_LAYER_LUNARG_standard_validation",
            "VK_LAYER_LUNARG_parameter_validation",
            "VK_LAYER_LUNARG_core_validation",

            //"VK_LAYER_LUNARG_api_dump",
            //"VK_LAYER_LUNARG_object_tracker",
            //"VK_LAYER_LUNARG_device_simulation",
            "VK_LAYER_GOOGLE_threading",
            "VK_LAYER_GOOGLE_unique_objects"
            "VK_LAYER_RENDERDOC_Capture"
        };

        assert((instanceVersion = vkh::vsEnumerateInstanceVersion(vkGlobal::loader)) >= VK_MAKE_VERSION(1, 2, 131));

        // get required extensions
#ifdef VKT_ENABLE_GLFW_SUPPORT
#ifdef VKT_ENABLE_GLFW_LINKED
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        // add glfw extensions to list
        for (uint32_t i = 0; i < glfwExtensionCount; i++) {
            wantedExtensions.push_back(glfwExtensions[i]);
        };
#endif
#endif

        // get our needed extensions
        auto installedExtensions = vkh::vsEnumerateInstanceExtensionProperties(vkGlobal::loader);
        auto extensions = std::vector<const char*>();
        for (auto w : wantedExtensions) {
            for (auto i : installedExtensions)
            {
                if (std::string(i.extensionName).compare(w) == 0)
                {
                    extensions.emplace_back(w);
                    break;
                }
            }
        }

        // get validation layers
        auto installedLayers = vkh::vsEnumerateInstanceLayerProperties(vkGlobal::loader);
        auto layers = std::vector<const char*>();
        for (auto w : wantedLayers) {
            for (auto i : installedLayers)
            {
                if (std::string(i.layerName).compare(w) == 0)
                {
                    layers.emplace_back(w);
                    break;
                }
            }
        }

        // 
        this->usedExtensions = extensions;
        this->usedLayers = layers;

        // app info
        auto appinfo = vkh::VkApplicationInfo{};
        appinfo.pNext = nullptr;
        appinfo.pApplicationName = "VKTest";
        appinfo.apiVersion = VK_MAKE_VERSION(1, 2, 135);

        // create instance info
        auto cinstanceinfo = vkh::VkInstanceCreateInfo{};
        cinstanceinfo.pApplicationInfo = &(this->applicationInfo = appinfo); // due JabaCPP unable to access
        cinstanceinfo.enabledExtensionCount = static_cast<uint32_t>(this->usedExtensions.size());
        cinstanceinfo.ppEnabledExtensionNames = this->usedExtensions.data();
        cinstanceinfo.enabledLayerCount = static_cast<uint32_t>(this->usedLayers.size());
        cinstanceinfo.ppEnabledLayerNames = this->usedLayers.data();

        // create the "debug utils EXT" callback object
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = DebugCallback;    // global function
        debugCreateInfo.pUserData = nullptr;

        // 
        cinstanceinfo.pNext = &debugCreateInfo;

        // Dynamically Load the Vulkan library
        this->instanceDispatch = std::make_shared<xvk::Instance>(vkt::vkGlobal::loader.get(), (this->instanceCreate = cinstanceinfo));
        this->instance = this->instanceDispatch->handle;
        vkt::vkGlobal::instance = this->instanceDispatch.get_shared();

        // get physical device for application
        physicalDevices = vkh::vsEnumeratePhysicalDevices(this->instanceDispatch.get_shared());

        //
        //uint32_t count = 0u; vkh::handleVk(this->instanceDispatch->EnumeratePhysicalDevices(&count, nullptr));
        //this->physicalDevices.resize(count);
        //vkh::handleVk(this->instanceDispatch->EnumeratePhysicalDevices(&count, this->physicalDevices.data()));

        // 
#ifdef VKT_VULKAN_DEBUG
        if (CreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &reinterpret_cast<VkDebugUtilsMessengerEXT&>(messenger)) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug callback");
        }
#endif

        // 
        return instance;
    };
#endif


}; // namespace NSM
