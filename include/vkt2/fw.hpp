#pragma once // #

//#ifdef OS_WIN
#if (defined(_WIN32) || defined(__MINGW32__) || defined(_MSC_VER_) || defined(__MINGW64__)) 
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif

//#ifdef OS_LNX
#ifdef __linux__
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#endif

#define ENABLE_EXTENSION_GLM
#define ENABLE_EXTENSION_VMA
#define ENABLE_EXTENSION_RTX
//#define GLFW_INCLUDE_VULKAN
#include "utils.hpp"
#include "structs.hpp"
#include "core.hpp"
#include "vector.hpp"

#ifdef VKT_ENABLE_GLFW_SUPPORT
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#endif

// TODO: FULL REWRITE OF THAT "PROJECT"!!!
namespace vkt
{

    class GPUFramework {
    protected:

        // instance extensions
        std::vector<const char*> wantedExtensions = {
            "VK_KHR_get_physical_device_properties2",
            "VK_KHR_get_surface_capabilities2",
            "VK_KHR_display",
            "VK_EXT_direct_mode_display",
            "VK_EXT_swapchain_colorspace"
        };

        // default device extensions
        std::vector<const char*> wantedDeviceExtensions = {
            "VK_EXT_swapchain_colorspace",
            "VK_EXT_external_memory_host",
            "VK_EXT_sample_locations",
            "VK_EXT_conservative_rasterization",
            "VK_EXT_hdr_metadata",
            "VK_EXT_queue_family_foreign",
            "VK_EXT_sampler_filter_minmax",
            "VK_EXT_descriptor_indexing",
            "VK_EXT_scalar_block_layout",
            "VK_EXT_subgroup_size_control",

            "VK_AMD_gpu_shader_int16",
            "VK_AMD_gpu_shader_half_float",
            "VK_AMD_gcn_shader",
            "VK_AMD_buffer_marker",
            "VK_AMD_shader_info",
            "VK_AMD_texture_gather_bias_lod",
            "VK_AMD_shader_image_load_store_lod",
            "VK_AMD_shader_trinary_minmax",
            "VK_AMD_draw_indirect_count",

            "VK_KHR_16bit_storage",
            "VK_KHR_8bit_storage",
            "VK_KHR_incremental_present",
            "VK_KHR_push_descriptor",
            "VK_KHR_surface",
            "VK_KHR_swapchain",
            "VK_KHR_sampler_ycbcr_conversion",
            "VK_KHR_image_format_list",
            "VK_KHR_shader_draw_parameters",
            "VK_KHR_variable_pointers",
            "VK_KHR_dedicated_allocation",
            "VK_KHR_relaxed_block_layout",
            "VK_KHR_descriptor_update_template",
            "VK_KHR_sampler_mirror_clamp_to_edge",
            "VK_KHR_storage_buffer_storage_class",
            "VK_KHR_vulkan_memory_model",
            "VK_KHR_dedicated_allocation",
            "VK_KHR_driver_properties",
            "VK_KHR_get_memory_requirements2",
            "VK_KHR_bind_memory2",
            "VK_KHR_maintenance1",
            "VK_KHR_maintenance2",
            "VK_KHR_maintenance3",
            "VK_KHX_shader_explicit_arithmetic_types",
            "VK_KHR_shader_atomic_int64",
            "VK_KHR_shader_float16_int8",
            "VK_KHR_shader_float_controls",
            "VK_KHR_shader_clock",
            "VK_KHR_imageless_framebuffer",
            "VK_KHR_storage_buffer_storage_class",
            "VK_KHR_shader_subgroup_extended_types",

            "VK_NV_compute_shader_derivatives",
            "VK_NV_corner_sampled_image",
            "VK_NV_shader_image_footprint",
            "VK_NV_shader_subgroup_partitioned",
            "VK_NV_shader_sm_builtins",

            "VK_KHR_external_memory",
            "VK_KHR_external_memory_capabilities",
            "VK_KHR_external_memory_win32",

            "VK_KHR_external_semaphore",
            "VK_KHR_external_semaphore_capabilities",
            "VK_KHR_external_semaphore_win32",

            "VK_NV_ray_tracing",
            "VK_NVX_image_view_handle"
        };

        // instance layers
        std::vector<const char*> wantedLayers = {
            "VK_LAYER_LUNARG_assistant_layer",
            "VK_LAYER_LUNARG_standard_validation",
            "VK_LAYER_LUNARG_parameter_validation",
            "VK_LAYER_LUNARG_core_validation",

            //"VK_LAYER_LUNARG_api_dump",
            //"VK_LAYER_LUNARG_object_tracker",
            //"VK_LAYER_LUNARG_device_simulation",
            //"VK_LAYER_GOOGLE_threading",
            //"VK_LAYER_GOOGLE_unique_objects"
            //"VK_LAYER_RENDERDOC_Capture"
        };

        // default device layers
        std::vector<const char*> wantedDeviceValidationLayers = {
            "VK_LAYER_AMD_switchable_graphics"
        };


    public: GPUFramework() {};
        vk::Fence fence = {};
        vk::Queue queue = {};
        vk::Device device = {};
        vk::Instance instance = {};
        vk::DescriptorPool descriptorPool = {};
        vk::PhysicalDevice physicalDevice = {};
        vk::CommandPool commandPool = {};
        vk::RenderPass renderPass = {};
        vk::Image depthImage = {};
        vk::ImageView depthImageView = {};
        vk::PipelineCache pipelineCache = {};
        vk::DispatchLoaderDynamic dispatch = {};
        vk::PhysicalDeviceMemoryProperties2 memoryProperties = {};
        VmaAllocator allocator = {};
        uint32_t queueFamilyIndex = 0;
        uint32_t instanceVersion = 0;

        VmaAllocation vmaDepthImageAllocation = {};
        VmaAllocationInfo vmaDepthImageAllocationInfo = {};

        std::vector<vk::PhysicalDevice> physicalDevices = {};
        std::vector<uint32_t> queueFamilyIndices = {};

        //vk::Device createDevice(bool isComputePrior = true, std::string shaderPath = "./", bool enableAdvancedAcceleration = true);
        inline vk::PhysicalDevice& getPhysicalDevice(const uint32_t& gpuID) { physicalDevice = physicalDevices[gpuID]; return physicalDevice; };
        inline vk::PhysicalDevice& getPhysicalDevice() { if (!physicalDevice) { physicalDevice = physicalDevices[0u]; }; return physicalDevice; };

        // 
        inline vk::DispatchLoaderDynamic getDispatch() { return dispatch; };
        inline vk::Device& getDevice() { return device; };
        inline vk::Queue& getQueue() { return queue; };
        inline vk::Fence& getFence() { return fence; };
        inline vk::Instance& getInstance() { return instance; };
        inline vk::CommandPool& getCommandPool() { return commandPool; };
        inline vk::PipelineCache& getPipelineCache() { return pipelineCache; };
        inline vk::DescriptorPool& getDescriptorPool() { return descriptorPool; };
        inline vk::PhysicalDeviceMemoryProperties2& getMemoryProperties() { return memoryProperties; };
        inline VmaAllocator& getAllocator() { return allocator; };

        // 
        inline const vk::DispatchLoaderDynamic getDispatch() const { return dispatch; };
        inline const vk::PhysicalDevice& getPhysicalDevice() const { return physicalDevice; };
        inline const vk::Device& getDevice() const { return device; };
        inline const vk::Queue& getQueue() const { return queue; };
        inline const vk::Fence& getFence() const { return fence; };
        inline const vk::Instance& getInstance() const { return instance; };
        inline const vk::CommandPool& getCommandPool() const { return commandPool; };
        inline const vk::PipelineCache& getPipelineCache() const { return pipelineCache; };
        inline const vk::DescriptorPool& getDescriptorPool() const { return descriptorPool; };
        inline const vk::PhysicalDeviceMemoryProperties2& getMemoryProperties() const { return memoryProperties; };
        inline const VmaAllocator& getAllocator() const { return allocator; };

        //
        inline operator vk::PhysicalDevice&() { return physicalDevice; };
        inline operator vk::Device&() { return device; };
        inline operator vk::Queue&() { return queue; };
        inline operator vk::Fence&() { return fence; };
        inline operator vk::Instance&() { return instance; };
        inline operator vk::CommandPool&() { return commandPool; };
        inline operator vk::PipelineCache&() { return pipelineCache; };
        inline operator vk::DescriptorPool&() { return descriptorPool; };
        inline operator vk::PhysicalDeviceMemoryProperties2& () { return memoryProperties; };
        inline operator VmaAllocator&() { return allocator; };

        //
        inline operator const vk::PhysicalDevice&() const { return physicalDevice; };
        inline operator const vk::Device&() const { return device; };
        inline operator const vk::Queue&() const { return queue; };
        inline operator const vk::Fence&() const { return fence; };
        inline operator const vk::Instance&() const { return instance; };
        inline operator const vk::CommandPool&() const { return commandPool; };
        inline operator const vk::PipelineCache&() const { return pipelineCache; };
        inline operator const vk::DescriptorPool&() const { return descriptorPool; };
        inline operator const vk::PhysicalDeviceMemoryProperties2& () const { return memoryProperties; };
        inline operator const VmaAllocator&() const { return allocator; };

        // 
        inline void submitCommandWithSync(const vk::CommandBuffer & cmdBuf) {
            // submit command
            vk::SubmitInfo sbmi = {};
            sbmi.commandBufferCount = 1;//cmdBuffers.size();
            sbmi.pCommandBuffers = &cmdBuf;

            // submit commands
            auto fence = getFence(); {
                getQueue().submit(sbmi, fence);
                device.waitForFences({ fence }, true, INT32_MAX);
            };
            device.resetFences({ 1, &fence });
        }

#ifdef VKT_ENABLE_GLFW_SUPPORT
        struct SurfaceWindow {
            SurfaceFormat surfaceFormat = {};
            vk::Extent2D surfaceSize = vk::Extent2D{ 0u, 0u };
            vk::SurfaceKHR surface = {};
            GLFWwindow* window = nullptr;
        } applicationWindow = {};
#endif

    public:
        inline vk::Instance& createInstance() {

#ifdef VOLK_H_
            volkInitialize();
#endif

            // 
            assert((instanceVersion = vk::enumerateInstanceVersion()) >= VK_MAKE_VERSION(1, 2, 131));

            // get required extensions
#ifdef VKT_ENABLE_GLFW_SUPPORT
            uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            // add glfw extensions to list
            for (uint32_t i = 0; i < glfwExtensionCount; i++) {
                wantedExtensions.push_back(glfwExtensions[i]);
            };
#endif

            // get our needed extensions
            auto installedExtensions = vk::enumerateInstanceExtensionProperties();
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
            auto installedLayers = vk::enumerateInstanceLayerProperties();
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

            // app info
            auto appinfo = vk::ApplicationInfo{};
            appinfo.pNext = nullptr;
            appinfo.pApplicationName = "VKTest";
            appinfo.apiVersion = VK_MAKE_VERSION(1, 2, 131);

            // create instance info
            auto cinstanceinfo = vk::InstanceCreateInfo{};
            cinstanceinfo.pApplicationInfo = &appinfo;
            cinstanceinfo.enabledExtensionCount = extensions.size();
            cinstanceinfo.ppEnabledExtensionNames = extensions.data();
            cinstanceinfo.enabledLayerCount = layers.size();
            cinstanceinfo.ppEnabledLayerNames = layers.data();

            // 
            instance = vk::createInstance(cinstanceinfo);

            // get physical device for application
            physicalDevices = instance.enumeratePhysicalDevices();

            // 
            return instance;
        };

        // TODO: REMAKE MAKING
        inline vk::Device createDevice(bool isComputePrior = true, std::string shaderPath = "", bool enableAdvancedAcceleration = false) {

            // use extensions
            auto deviceExtensions = std::vector<const char*>();
            auto gpuExtensions = physicalDevice.enumerateDeviceExtensionProperties();
            for (auto w : wantedDeviceExtensions) {
                for (auto i : gpuExtensions) {
                    if (std::string(i.extensionName).compare(w) == 0) {
                        deviceExtensions.emplace_back(w); break;
                    };
                };
            };

            // use layers
            auto layers = std::vector<const char*>();
            auto deviceValidationLayers = std::vector<const char*>();
            auto gpuLayers = physicalDevice.enumerateDeviceLayerProperties();
            for (auto w : wantedLayers) {
                for (auto i : gpuLayers) {
                    if (std::string(i.layerName).compare(w) == 0) {
                        layers.emplace_back(w); break;
                    };
                };
            };

            // minimal features
            auto gTexelBufferAligment = vk::PhysicalDeviceTexelBufferAlignmentFeaturesEXT{};
            auto gStorage16 = vk::PhysicalDevice16BitStorageFeatures{};
            auto gStorage8 = vk::PhysicalDevice8BitStorageFeatures{};
            auto gDescIndexing = vk::PhysicalDeviceDescriptorIndexingFeaturesEXT{};
            auto gFloat16U8 = vk::PhysicalDeviceFloat16Int8FeaturesKHR{}; // Vulkan 1.3
            auto gFeatures = vk::PhysicalDeviceFeatures2{};
            //auto gConsertvative = vk::PhysicalDeviceConservativeRasterizationPropertiesEXT{};

            // 
            gFloat16U8.pNext = &gTexelBufferAligment;
            gStorage8.pNext = &gFloat16U8;
            gStorage16.pNext = &gStorage8;
            gDescIndexing.pNext = &gStorage16;
            gFeatures.pNext = &gDescIndexing;

            // 
            vkGetPhysicalDeviceFeatures2(physicalDevice, &(VkPhysicalDeviceFeatures2&)gFeatures);
            this->memoryProperties = physicalDevice.getMemoryProperties2();

            // get features and queue family properties
            //auto gpuFeatures = gpu.getFeatures();
            auto gpuQueueProps = physicalDevice.getQueueFamilyProperties();

            // queue family initial
            float priority = 1.0f;
            uint32_t computeFamilyIndex = -1, graphicsFamilyIndex = -1;
            auto queueCreateInfos = std::vector<vk::DeviceQueueCreateInfo>();

#ifdef VKT_ENABLE_GLFW_SUPPORT
            for (auto queuefamily : gpuQueueProps) {
                graphicsFamilyIndex++;
                if (queuefamily.queueFlags & (vk::QueueFlagBits::eCompute) && queuefamily.queueFlags & (vk::QueueFlagBits::eGraphics) && physicalDevice.getSurfaceSupportKHR(graphicsFamilyIndex, surface())) {
                    queueCreateInfos.push_back(vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags()).setQueueFamilyIndex(graphicsFamilyIndex).setQueueCount(1).setPQueuePriorities(&priority));
                    queueFamilyIndices.push_back(graphicsFamilyIndex);
                    break;
                };
            };
#else
            for (auto queuefamily : gpuQueueProps) {
                computeFamilyIndex++;
                if (queuefamily.queueFlags & (vk::QueueFlagBits::eCompute)) {
                    queueCreateInfos.push_back(vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags()).setQueueFamilyIndex(computeFamilyIndex).setQueueCount(1).setPQueuePriorities(&priority));
                    queueFamilyIndices.push_back(computeFamilyIndex);
                    break;
                };
            };
#endif

            // return device with queue pointer
            const uint32_t qptr = 0;
            if (queueCreateInfos.size() > 0) {
                this->queueFamilyIndex = queueFamilyIndices[qptr];
                this->device = this->physicalDevice.createDevice(vkh::VkDeviceCreateInfo{
                    .pNext = &gFeatures,
                    .queueCreateInfoCount = uint32_t(queueCreateInfos.size()),
                    .pQueueCreateInfos = reinterpret_cast<::VkDeviceQueueCreateInfo*>(queueCreateInfos.data()),
                    .enabledLayerCount = uint32_t(layers.size()),
                    .ppEnabledLayerNames = layers.data(),
                    .enabledExtensionCount = uint32_t(deviceExtensions.size()),
                    .ppEnabledExtensionNames = deviceExtensions.data(),
                    //.pEnabledFeatures = &(VkPhysicalDeviceFeatures&)(gFeatures.features)
                });
                this->pipelineCache = this->device.createPipelineCache(vk::PipelineCacheCreateInfo());
            };
            
            this->queue = this->device.getQueue(queueFamilyIndex, 0); // 
            this->fence = this->device.createFence(vk::FenceCreateInfo().setFlags({}));
            this->commandPool = this->device.createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer), queueFamilyIndex));
            this->dispatch = vk::DispatchLoaderDynamic(this->instance, this->device); // 

            VmaAllocatorCreateInfo vma_info = {};
            vma_info.device = this->device;
            vma_info.instance = this->instance;
            vma_info.physicalDevice = this->physicalDevice;
            vmaCreateAllocator(&vma_info, &this->allocator);

            // Manually Create Descriptor Pool
            auto dps = std::vector<vkh::VkDescriptorPoolSize>{
                vkh::VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = 256u},
                vkh::VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, .descriptorCount = 1024u},
                vkh::VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_SAMPLER, .descriptorCount = 1024u},
                vkh::VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV, .descriptorCount = 256u},
                vkh::VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = 1024u},
                vkh::VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = 256u},
                vkh::VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, .descriptorCount = 256u},
                vkh::VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, .descriptorCount = 256u}
            };

            this->descriptorPool = device.createDescriptorPool(vkh::VkDescriptorPoolCreateInfo{
                .maxSets = 256u, .poolSizeCount = static_cast<uint32_t>(dps.size()), .pPoolSizes = dps.data()
            });

            return device;
        };

#ifdef VKT_ENABLE_GLFW_SUPPORT
        // create window and surface for this application (multi-window not supported)
        inline SurfaceWindow& createWindowSurface(GLFWwindow * window, uint32_t WIDTH, uint32_t HEIGHT, std::string title = "TestApp") {
            applicationWindow.window = window;
            applicationWindow.surfaceSize = vk::Extent2D{ WIDTH, HEIGHT };
            auto result = glfwCreateWindowSurface((VkInstance&)(instance), applicationWindow.window, nullptr, (VkSurfaceKHR*)& applicationWindow.surface);
            if (result != VK_SUCCESS) { glfwTerminate(); exit(result); };
            return applicationWindow;
        }

        // create window and surface for this application (multi-window not supported)
        inline SurfaceWindow& createWindowSurface(uint32_t WIDTH, uint32_t HEIGHT, std::string title = "TestApp") {
            applicationWindow.window = glfwCreateWindow(WIDTH, HEIGHT, title.c_str(), nullptr, nullptr);
            applicationWindow.surfaceSize = vk::Extent2D{ WIDTH, HEIGHT };
            auto result = glfwCreateWindowSurface((VkInstance&)(instance), applicationWindow.window, nullptr, (VkSurfaceKHR*)& applicationWindow.surface);
            if (result != VK_SUCCESS) { glfwTerminate(); exit(result); };
            return applicationWindow;
        }

        // getters
        inline vk::SurfaceKHR surface() const { return applicationWindow.surface; }
        inline GLFWwindow* window() const { return applicationWindow.window; }
        inline const SurfaceFormat& format() const { return applicationWindow.surfaceFormat; }
        inline const vk::Extent2D& size() const { return applicationWindow.surfaceSize; }

        // setters
        inline void format(SurfaceFormat format) { applicationWindow.surfaceFormat = format; }
        inline void size(const vk::Extent2D & size) { applicationWindow.surfaceSize = size; }

        // 
        inline SurfaceFormat& getSurfaceFormat(vk::PhysicalDevice gpu)
        {
            auto surfaceFormats = gpu.getSurfaceFormatsKHR(applicationWindow.surface);

            const std::vector<vk::Format> preferredFormats = { vk::Format::eR8G8B8A8Unorm, vk::Format::eB8G8R8A8Unorm };

            vk::Format surfaceColorFormat =
                surfaceFormats.size() == 1 &&
                surfaceFormats[0].format == vk::Format::eUndefined
                ? vk::Format::eR8G8B8A8Unorm
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
            vk::ColorSpaceKHR surfaceColorSpace = surfaceFormats[surfaceFormatID].colorSpace;

            // get format properties?
            auto formatProperties = gpu.getFormatProperties(surfaceColorFormat);

            // only if these depth formats
            std::vector<vk::Format> depthFormats = {
                vk::Format::eD32SfloatS8Uint, vk::Format::eD32Sfloat,
                vk::Format::eD24UnormS8Uint, vk::Format::eD16UnormS8Uint,
                vk::Format::eD16Unorm };

            // choice supported depth format
            vk::Format surfaceDepthFormat = depthFormats[0];
            for (auto format : depthFormats) {
                auto depthFormatProperties = gpu.getFormatProperties(format);
                if (depthFormatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment) {
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

        inline vk::RenderPass& createRenderPass()
        { // TODO: Render Pass V2
            auto formats = applicationWindow.surfaceFormat;
            auto render_pass_helper = vkh::VsRenderPassCreateInfoHelper();

            render_pass_helper.addColorAttachment(vkh::VkAttachmentDescription{
                .format = VkFormat(formats.colorFormat),
                .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
            });

            render_pass_helper.setDepthStencilAttachment(vkh::VkAttachmentDescription{
                .format = VkFormat(formats.depthFormat),
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
            });

            render_pass_helper.addSubpassDependency(vkh::VkSubpassDependency{
                .srcSubpass = VK_SUBPASS_EXTERNAL,
                .dstSubpass = 0u,
                .srcStageMask = {.eColorAttachmentOutput = 1, .eTransfer = 1, .eBottomOfPipe = 1, },
                .dstStageMask = {.eColorAttachmentOutput = 1, },
                .srcAccessMask = {.eColorAttachmentWrite = 1 },
                .dstAccessMask = {.eColorAttachmentRead = 1, .eColorAttachmentWrite = 1 },
                .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
            });

            render_pass_helper.addSubpassDependency(vkh::VkSubpassDependency{
                .srcSubpass = 0u,
                .dstSubpass = VK_SUBPASS_EXTERNAL,
                .srcStageMask = {.eColorAttachmentOutput = 1 },
                .dstStageMask = {.eTopOfPipe = 1, .eColorAttachmentOutput = 1, .eTransfer = 1 },
                .srcAccessMask = {.eColorAttachmentRead = 1, .eColorAttachmentWrite = 1 },
                .dstAccessMask = {.eColorAttachmentRead = 1, .eColorAttachmentWrite = 1 },
                .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
            });

            return (renderPass = device.createRenderPass(render_pass_helper));
        }

        // update swapchain framebuffer
        inline void updateSwapchainFramebuffer(std::vector<Framebuffer> & swapchainBuffers, vk::SwapchainKHR & swapchain, vk::RenderPass & renderpass)
        {
            // The swapchain handles allocating frame images.
            auto& surfaceFormats = getSurfaceFormat(this->physicalDevice);
            auto  gpuMemoryProps = physicalDevice.getMemoryProperties();

            // 
            auto imageInfoVK = vk::ImageCreateInfo{};
            imageInfoVK.initialLayout = vk::ImageLayout::eUndefined;
            imageInfoVK.sharingMode = vk::SharingMode::eExclusive;
            imageInfoVK.flags = {};
            imageInfoVK.pNext = nullptr;
            imageInfoVK.arrayLayers = 1;
            imageInfoVK.extent = vk::Extent3D{ applicationWindow.surfaceSize.width, applicationWindow.surfaceSize.height, 1u };
            imageInfoVK.format = { surfaceFormats.depthFormat };
            imageInfoVK.imageType = vk::ImageType::e2D;
            imageInfoVK.mipLevels = 1;
            imageInfoVK.samples = vk::SampleCountFlagBits::e1;
            imageInfoVK.tiling = vk::ImageTiling::eOptimal;
            imageInfoVK.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment|vk::ImageUsageFlagBits::eTransferSrc;

            // 
            VmaAllocationCreateInfo allocCreateInfo = {};
            allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

            // 
            auto image_info = vkh::VkImageCreateInfo{
                .imageType = VK_IMAGE_TYPE_2D,
                .format = VkFormat(surfaceFormats.depthFormat),
                .extent = {applicationWindow.surfaceSize.width, applicationWindow.surfaceSize.height, 1u},
                .usage = { .eDepthStencilAttachment = 1 }
            };
            vmaCreateImage(this->allocator, (VkImageCreateInfo*)&image_info, &allocCreateInfo, &reinterpret_cast<VkImage&>(depthImage), &vmaDepthImageAllocation, &vmaDepthImageAllocationInfo);
            depthImageView = device.createImageView(vk::ImageViewCreateInfo{{}, depthImage, vk::ImageViewType::e2D, surfaceFormats.depthFormat, vk::ComponentMapping(), vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1} });

            // 
            auto swapchainImages = device.getSwapchainImagesKHR(swapchain);
            swapchainBuffers.resize(swapchainImages.size());
            for (int i = 0; i < swapchainImages.size(); i++)
            { // create framebuffers
                std::array<vk::ImageView, 2> views = {}; // predeclare views
                views[0] = device.createImageView(vk::ImageViewCreateInfo{ {}, swapchainImages[i], vk::ImageViewType::e2D, surfaceFormats.colorFormat, vk::ComponentMapping(), vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1} }); // color view
                views[1] = depthImageView; // depth view
                swapchainBuffers[i].frameBuffer = device.createFramebuffer(vk::FramebufferCreateInfo{ {}, renderpass, uint32_t(views.size()), views.data(), applicationWindow.surfaceSize.width, applicationWindow.surfaceSize.height, 1u });
            };
        }

        inline std::vector<Framebuffer> createSwapchainFramebuffer(vk::SwapchainKHR swapchain, vk::RenderPass renderpass) {
            // framebuffers vector
            std::vector<Framebuffer> swapchainBuffers = {};
            updateSwapchainFramebuffer(swapchainBuffers, swapchain, renderpass);
            for (int i = 0; i < swapchainBuffers.size(); i++)
            { // create semaphore
                swapchainBuffers[i].semaphore = device.createSemaphore(vk::SemaphoreCreateInfo());
                swapchainBuffers[i].waitFence = device.createFence(vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled));
            };
            return swapchainBuffers;
        }

        // create swapchain template
        inline vk::SwapchainKHR createSwapchain()
        {
            auto& formats = getSurfaceFormat(this->physicalDevice);
            auto& surface = applicationWindow.surface;
            auto surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
            auto surfacePresentModes = physicalDevice.getSurfacePresentModesKHR(surface);

            // check the surface width/height.
            if (!(surfaceCapabilities.currentExtent.width == -1 ||
                surfaceCapabilities.currentExtent.height == -1))
            {
                applicationWindow.surfaceSize = surfaceCapabilities.currentExtent;
            }

            // get supported present mode, but prefer mailBox
            auto presentMode = vk::PresentModeKHR::eImmediate;
            std::vector<vk::PresentModeKHR> priorityModes = { vk::PresentModeKHR::eImmediate, vk::PresentModeKHR::eMailbox, vk::PresentModeKHR::eFifoRelaxed, vk::PresentModeKHR::eFifo };

            bool found = false; // 
            for (auto pm : priorityModes) {
                if (found) break;
                for (auto sfm : surfacePresentModes) { if (pm == sfm) { presentMode = pm; found = true; break; } }
            };

            // swapchain info
            auto swapchainCreateInfo = vk::SwapchainCreateInfoKHR();
            swapchainCreateInfo.surface = surface;
            swapchainCreateInfo.minImageCount = std::min(surfaceCapabilities.maxImageCount, 3u);
            swapchainCreateInfo.imageFormat = formats.colorFormat;
            swapchainCreateInfo.imageColorSpace = formats.colorSpace;
            swapchainCreateInfo.imageExtent = applicationWindow.surfaceSize;
            swapchainCreateInfo.imageArrayLayers = 1;
            swapchainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
            swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
            swapchainCreateInfo.preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
            swapchainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
            swapchainCreateInfo.presentMode = presentMode;
            swapchainCreateInfo.clipped = true;

            // create swapchain
            return device.createSwapchainKHR(swapchainCreateInfo, nullptr);
        }
#endif
    };

}; // namespace NSM
