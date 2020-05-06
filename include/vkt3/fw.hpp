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

// 
#define ENABLE_EXTENSION_GLM
#define ENABLE_EXTENSION_VMA
#define ENABLE_EXTENSION_RTX
#define GLFW_INCLUDE_VULKAN

// 
#if defined(VKT_ENABLE_GLFW_SUPPORT) || defined(ENABLE_OPENGL_INTEROP)
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#endif

// Force include for avoid GLAD problem...
#include <vkt2/utils.hpp>
#include <vkt2/structs.hpp>
#include <vkt2/core.hpp>
#include <vkt2/vector.hpp>
#include <vkt2/image.hpp>

//#define VKT_ENABLE_DEBUG

// TODO: FULL REWRITE OF THAT "PROJECT"!!!
namespace vkt
{

    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }

    VkResult CreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pCallback) {

        // Note: It seems that static_cast<...> doesn't work. Use the C-style forced cast.
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pCallback);
        }
        else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    std::vector<const char*> GetRequiredExtensions() {
#ifdef VKT_ENABLE_GLFW_SUPPORT
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
#else
        std::vector<const char*> extensions{};
#endif

        // also want the "debug utils"
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        return extensions;
    }



    class GPUFramework : std::enable_shared_from_this<GPUFramework> {
    protected:

        std::vector<const char*> usedDeviceExtensions = {};
        std::vector<const char*> usedExtensions = {};

        std::vector<const char*> usedDeviceLayers = {};
        std::vector<const char*> usedLayers = {};

        std::vector<VkDeviceQueueCreateInfo> usedQueueCreateInfos = {};


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

        // default device extensions
        std::vector<const char*> wantedDeviceExtensions = {
            "VK_EXT_swapchain_colorspace",
            "VK_EXT_external_memory_host",
            "VK_EXT_queue_family_foreign",
            "VK_EXT_swapchain_colorspace",
            "VK_EXT_sample_locations",
            "VK_EXT_conservative_rasterization",
            "VK_EXT_hdr_metadata",
            "VK_EXT_transform_feedback",

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

            // 
            "VK_KHR_incremental_present",
            "VK_KHR_ray_tracing",
            "VK_KHR_pipeline_library",
            "VK_KHR_deferred_host_operations",
            "VK_KHR_buffer_device_address",
            "VK_KHR_vulkan_memory_model",
            "VK_KHR_shader_float16_int8",
            "VK_KHR_shader_float_controls",
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
            "VK_KHR_external_memory",
            "VK_KHR_external_memory_capabilities",
            "VK_KHR_external_memory_win32",

            // 
            "VK_KHR_external_semaphore",
            "VK_KHR_external_semaphore_capabilities",
            "VK_KHR_external_semaphore_win32",

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

        // instance layers
        std::vector<const char*> wantedLayers = {
            //"VK_LAYER_KHRONOS_validation", // Still Validation SPAM by LancER

            //"VK_LAYER_LUNARG_assistant_layer",
            //"VK_LAYER_LUNARG_standard_validation",
            //"VK_LAYER_LUNARG_parameter_validation",
            //"VK_LAYER_LUNARG_core_validation",

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


    public: friend GPUFramework;
        GPUFramework() {

#ifdef VOLK_H_
            volkInitialize();
#endif
        };

        //GPUFramework(const GPUFramework& fw) { *this = fw; };
        //GPUFramework(GPUFramework* fw) { *this = fw; };
        GPUFramework(vkt::uni_ptr<GPUFramework> fw) {
            *this = fw; 
        };

        // 
        GPUFramework& operator=(vkt::uni_ptr<GPUFramework> fw) {
            if (fw.ptr()) {
                this->fence = fw->fence;
                this->queue = fw->queue;
                this->device = fw->device;
                this->instance = fw->instance;
                this->descriptorPool = fw->descriptorPool;
                this->physicalDevice = fw->physicalDevice;
                this->commandPool = fw->commandPool;
                this->renderPass = fw->renderPass;
                this->depthImage = fw->depthImage;
                this->depthImageView = fw->depthImageView;
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

        // minimal features
        VkPhysicalDeviceTransformFeedbackFeaturesEXT gTrasformFeedback{};
        VkPhysicalDeviceRayTracingFeaturesKHR gRayTracing{};
        VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT gTexelBufferAligment{};
        VkPhysicalDevice16BitStorageFeatures gStorage16{};
        VkPhysicalDevice8BitStorageFeatures gStorage8{};
        VkPhysicalDeviceDescriptorIndexingFeaturesEXT gDescIndexing{};
        VkPhysicalDeviceFloat16Int8FeaturesKHR gFloat16U8{}; // Vulkan 1.3
        VkPhysicalDeviceFeatures2 gFeatures{};
        VkPhysicalDeviceBufferDeviceAddressFeatures gDeviceAddress{};

        // XVK loaded (NEW!)
        xvk::Instance instanceDispatch = {};
        xvk::Device deviceDispatch = {};
        xvk::Loader vulkanLoader = {};

        // JavaCPP and XVK compatible (NEW!)
        vkh::VkApplicationInfo applicationInfo = {};
        vkh::VkInstanceCreateInfo instanceCreate = {};
        vkh::VkDeviceCreateInfo deviceCreate = {};

        // 
        VkFence fence = {};
        VkQueue queue = {};
        VkDevice device = {};
        VkInstance instance = {};
        VkDescriptorPool descriptorPool = {};
        VkPhysicalDevice physicalDevice = {};
        VkCommandPool commandPool = {};
        VkRenderPass renderPass = {};
        VkImage depthImage = {};
        VkImageView depthImageView = {};
        VkPipelineCache pipelineCache = {};
        VkPhysicalDeviceMemoryProperties2 memoryProperties = {};
        VkDebugUtilsMessengerEXT messenger = {};

        VmaAllocator allocator = {};
        uint32_t queueFamilyIndex = 0;
        uint32_t instanceVersion = 0;

        VmaAllocation vmaDepthImageAllocation = {};
        VmaAllocationInfo vmaDepthImageAllocationInfo = {};

        std::vector<VkPhysicalDevice> physicalDevices = {};
        std::vector<uint32_t> queueFamilyIndices = {};

        //VkDevice createDevice(bool isComputePrior = true, std::string shaderPath = "./", bool enableAdvancedAcceleration = true);
        inline VkPhysicalDevice& getPhysicalDevice(const uint32_t& gpuID) { physicalDevice = physicalDevices[gpuID]; return physicalDevice; };
        inline VkPhysicalDevice& getPhysicalDevice() { if (!physicalDevice) { physicalDevice = physicalDevices[0u]; }; return physicalDevice; };

        // Compatible with JavaCPP directly
        inline uintptr_t getInstanceCreateInfoAddress() { return (uintptr_t)(&instanceCreate); };
        inline uintptr_t getDeviceCreateInfoAddress() { return (uintptr_t)(&deviceCreate); };
        inline uintptr_t getInstanceCreateInfoAddress() const { return (uintptr_t)(&instanceCreate); };
        inline uintptr_t getDeviceCreateInfoAddress() const { return (uintptr_t)(&deviceCreate); };

        // 
        //inline VkDispatchLoaderDynamic getDispatch() { return dispatch; };
        inline VkDevice& getDevice() { return device; };
        inline VkQueue& getQueue() { return queue; };
        inline VkFence& getFence() { return fence; };
        inline VkInstance& getInstance() { return instance; };
        inline VkCommandPool& getCommandPool() { return commandPool; };
        inline VkPipelineCache& getPipelineCache() { return pipelineCache; };
        inline VkDescriptorPool& getDescriptorPool() { return descriptorPool; };
        inline VkPhysicalDeviceMemoryProperties2& getMemoryProperties() { return memoryProperties; };
        inline VmaAllocator& getAllocator() { return allocator; };

        // 
        //inline const VkDispatchLoaderDynamic getDispatch() const { return dispatch; };
        inline const VkPhysicalDevice& getPhysicalDevice() const { return physicalDevice; };
        inline const VkDevice& getDevice() const { return device; };
        inline const VkQueue& getQueue() const { return queue; };
        inline const VkFence& getFence() const { return fence; };
        inline const VkInstance& getInstance() const { return instance; };
        inline const VkCommandPool& getCommandPool() const { return commandPool; };
        inline const VkPipelineCache& getPipelineCache() const { return pipelineCache; };
        inline const VkDescriptorPool& getDescriptorPool() const { return descriptorPool; };
        inline const VkPhysicalDeviceMemoryProperties2& getMemoryProperties() const { return memoryProperties; };
        inline const VmaAllocator& getAllocator() const { return allocator; };

        //
        inline operator VkPhysicalDevice&() { return physicalDevice; };
        inline operator VkDevice&() { return device; };
        inline operator VkQueue&() { return queue; };
        inline operator VkFence&() { return fence; };
        inline operator VkInstance&() { return instance; };
        inline operator VkCommandPool&() { return commandPool; };
        inline operator VkPipelineCache&() { return pipelineCache; };
        inline operator VkDescriptorPool&() { return descriptorPool; };
        inline operator VkPhysicalDeviceMemoryProperties2& () { return memoryProperties; };
        inline operator VmaAllocator&() { return allocator; };

        //
        inline operator const VkPhysicalDevice&() const { return physicalDevice; };
        inline operator const VkDevice&() const { return device; };
        inline operator const VkQueue&() const { return queue; };
        inline operator const VkFence&() const { return fence; };
        inline operator const VkInstance&() const { return instance; };
        inline operator const VkCommandPool&() const { return commandPool; };
        inline operator const VkPipelineCache&() const { return pipelineCache; };
        inline operator const VkDescriptorPool&() const { return descriptorPool; };
        inline operator const VkPhysicalDeviceMemoryProperties2& () const { return memoryProperties; };
        inline operator const VmaAllocator&() const { return allocator; };

        // Deprecated!
        /*inline void submitCommandWithSync(const VkCommandBuffer & cmdBuf) {
            // submit command
            VkSubmitInfo sbmi = {};
            sbmi.commandBufferCount = 1;//cmdBuffers.size();
            sbmi.pCommandBuffers = &cmdBuf;

            // submit commands
            auto fence = getFence(); {
                getQueue().submit(sbmi, fence);
                device.waitForFences({ fence }, true, INT32_MAX);
            };
            device.resetFences({ 1, &fence });
        }*/

#ifdef VKT_ENABLE_GLFW_SUPPORT
        struct SurfaceWindow {
            SurfaceFormat surfaceFormat = {};
            VkExtent2D surfaceSize = VkExtent2D{ 0u, 0u };
            VkSurfaceKHR surface = {};
            GLFWwindow* window = nullptr;
        } applicationWindow = {};
#endif

    public:
        inline VkImageView getDepthImageView() const {
            return depthImageView;
        };

        inline VkImage getDepthImage() const {
            return depthImage;
        };

        inline VkInstance& createInstance() {
            // 
            assert((instanceVersion = VkenumerateInstanceVersion()) >= VK_MAKE_VERSION(1, 2, 131));

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
            auto installedExtensions = VkenumerateInstanceExtensionProperties();
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
            auto installedLayers = VkenumerateInstanceLayerProperties();
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
            auto appinfo = VkApplicationInfo{};
            appinfo.pNext = nullptr;
            appinfo.pApplicationName = "VKTest";
            appinfo.apiVersion = VK_MAKE_VERSION(1, 2, 135);

            // create instance info
            auto cinstanceinfo = VkInstanceCreateInfo{};
            cinstanceinfo.pApplicationInfo = &(this->applicationInfo = appinfo); // due JabaCPP unable to access
            cinstanceinfo.enabledExtensionCount = static_cast<uint32_t>(this->usedExtensions.size());
            cinstanceinfo.ppEnabledExtensionNames = this->usedExtensions.data();
            cinstanceinfo.enabledLayerCount = static_cast<uint32_t>(this->usedLayers.size());
            cinstanceinfo.ppEnabledLayerNames = this->usedLayers.data();

            // create the "debug utils EXT" callback object
            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
            debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            debugCreateInfo.messageSeverity =
                VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                //VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            debugCreateInfo.messageType =
                VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            debugCreateInfo.pfnUserCallback = DebugCallback;    // global function
            debugCreateInfo.pUserData = nullptr;

            // 
#ifdef VKT_ENABLE_DEBUG
            //instance = VkcreateInstance(cinstanceinfo.setPNext(&debugCreateInfo));
#else
            //instance = VkcreateInstance(cinstanceinfo);
#endif

            // Dynamically Load the Vulkan library
            if (!vulkanLoader()) { return -1; };
            instanceDispatch = xvk::Instance(&vulkanLoader, (this->instanceCreate = cinstanceinfo));
            instance = instanceDispatch.handle;

            // get physical device for application
            //physicalDevices = instance.enumeratePhysicalDevices();
            

            // 
#ifdef VKT_ENABLE_DEBUG
            if (CreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &reinterpret_cast<VkDebugUtilsMessengerEXT&>(messenger)) != VK_SUCCESS) {
                throw std::runtime_error("failed to set up debug callback");
            }
#endif

            // 
            return instance;
        };

        // New constructor for clarify physical device
        inline VkDevice createDevice(const VkPhysicalDevice& physicalDevice, bool isComputePrior = true, std::string shaderPath = "", bool enableAdvancedAcceleration = false) {
            this->physicalDevice = physicalDevice; return this->createDevice(isComputePrior, shaderPath, enableAdvancedAcceleration);
        };

        //
        inline VkDevice createDevice(bool isComputePrior = true, std::string shaderPath = "", bool enableAdvancedAcceleration = false) {

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
            auto deviceLayers = std::vector<const char*>();
            auto gpuLayers = physicalDevice.enumerateDeviceLayerProperties();
            for (auto w : wantedLayers) {
                for (auto i : gpuLayers) {
                    if (std::string(i.layerName).compare(w) == 0) {
                        deviceLayers.emplace_back(w); break;
                    };
                };
            };

            // 
            this->usedDeviceExtensions = deviceExtensions;
            this->usedDeviceLayers = deviceLayers;

            //auto gConsertvative = VkPhysicalDeviceConservativeRasterizationPropertiesEXT{};

            // 
            gTrasformFeedback.pNext = &gRayTracing;
            gDeviceAddress.pNext = &gTrasformFeedback;
            gTexelBufferAligment.pNext = &gDeviceAddress;
            gFloat16U8.pNext = &gTexelBufferAligment;
            gStorage8.pNext = &gFloat16U8;
            gStorage16.pNext = &gStorage8;
            gDescIndexing.pNext = &gStorage16;
            gFeatures.pNext = &gDescIndexing;

            // 
            vkGetPhysicalDeviceFeatures2(physicalDevice, &reinterpret_cast<VkPhysicalDeviceFeatures2&>(gFeatures));
            //physicalDevice.getFeatures2(&(VkPhysicalDeviceFeatures2&)gFeatures);
            this->memoryProperties = physicalDevice.getMemoryProperties2();

            // get features and queue family properties
            //auto gpuFeatures = gpu.getFeatures();
            auto gpuQueueProps = physicalDevice.getQueueFamilyProperties();

            // queue family initial
            float priority = 1.0f;
            uint32_t computeFamilyIndex = -1, graphicsFamilyIndex = -1;
            auto queueCreateInfos = std::vector<VkDeviceQueueCreateInfo>();

#ifdef VKT_ENABLE_GLFW_SUPPORT
            for (auto queuefamily : gpuQueueProps) {
                graphicsFamilyIndex++;
                if (queuefamily.queueFlags & (VkQueueFlagBits::eCompute) && queuefamily.queueFlags & (VkQueueFlagBits::eGraphics) && physicalDevice.getSurfaceSupportKHR(graphicsFamilyIndex, surface())) {
                    queueCreateInfos.push_back(VkDeviceQueueCreateInfo(VkDeviceQueueCreateFlags()).setQueueFamilyIndex(graphicsFamilyIndex).setQueueCount(1).setPQueuePriorities(&priority));
                    queueFamilyIndices.push_back(graphicsFamilyIndex);
                    break;
                };
            };
#else
            for (auto queuefamily : gpuQueueProps) {
                computeFamilyIndex++;
                if (queuefamily.queueFlags & (VkQueueFlagBits::eCompute)) {
                    queueCreateInfos.push_back(VkDeviceQueueCreateInfo(VkDeviceQueueCreateFlags()).setQueueFamilyIndex(computeFamilyIndex).setQueueCount(1).setPQueuePriorities(&priority));
                    queueFamilyIndices.push_back(computeFamilyIndex);
                    break;
                };
            };
#endif

            // return device with queue pointer
            const uint32_t qptr = 0;
            if ((this->usedQueueCreateInfos = queueCreateInfos).size() > 0) {
                this->queueFamilyIndex = queueFamilyIndices[qptr];
                this->deviceDispatch = xvk::Device(&this->instanceDispatch, this->physicalDevice, (deviceCreate = vkh::VkDeviceCreateInfo{
                    .pNext = &gFeatures,
                    .queueCreateInfoCount = uint32_t(this->usedQueueCreateInfos.size()),
                    .pQueueCreateInfos = reinterpret_cast<::VkDeviceQueueCreateInfo*>(this->usedQueueCreateInfos.data()),
                    .enabledLayerCount = uint32_t(this->usedDeviceLayers.size()),
                    .ppEnabledLayerNames = this->usedDeviceLayers.data(),
                    .enabledExtensionCount = uint32_t(this->usedDeviceExtensions.size()),
                    .ppEnabledExtensionNames = this->usedDeviceExtensions.data(),
                    //.pEnabledFeatures = &(VkPhysicalDeviceFeatures&)(gFeatures.features)
                }));
                this->pipelineCache = this->device.createPipelineCache(VkPipelineCacheCreateInfo());
            };

            // TODO: XVK version 
            //this->queue = this->device.getQueue(queueFamilyIndex, 0); // 
            //this->fence = this->device.createFence(VkFenceCreateInfo().setFlags({}));
            //this->commandPool = this->device.createCommandPool(VkCommandPoolCreateInfo(VkCommandPoolCreateFlags(VkCommandPoolCreateFlagBits::eResetCommandBuffer), queueFamilyIndex));
            
            //this->dispatch = VkDispatchLoaderDynamic(this->instance, this->device); // 
            //this->dispatch = VkDispatchLoaderDynamic(this->instance, vkGetInstanceProcAddr, this->device, vkGetDeviceProcAddr); // 

            // 
            VmaAllocatorCreateInfo vma_info = {};
            vma_info.device = this->device;
            vma_info.instance = this->instance;
            vma_info.physicalDevice = this->physicalDevice;
            vma_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
            vmaCreateAllocator(&vma_info, &this->allocator);

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

            // TODO: XVK version 
            //this->descriptorPool = device.createDescriptorPool(vkh::VkDescriptorPoolCreateInfo{
            //    .maxSets = 256u, .poolSizeCount = static_cast<uint32_t>(dps.size()), .pPoolSizes = dps.data()
            //});

            return device;
        };

#ifdef VKT_ENABLE_GLFW_SUPPORT
        // create window and surface for this application (multi-window not supported)
        inline SurfaceWindow& createWindowSurface(GLFWwindow * window, uint32_t WIDTH, uint32_t HEIGHT, std::string title = "TestApp") {
            applicationWindow.window = window;
            applicationWindow.surfaceSize = VkExtent2D{ WIDTH, HEIGHT };
            auto result = glfwCreateWindowSurface((VkInstance&)(instance), applicationWindow.window, nullptr, (VkSurfaceKHR*)& applicationWindow.surface);
            if (result != VK_SUCCESS) { glfwTerminate(); exit(result); };
            return applicationWindow;
        }

        // create window and surface for this application (multi-window not supported)
        inline SurfaceWindow& createWindowSurface(uint32_t WIDTH, uint32_t HEIGHT, std::string title = "TestApp") {
            applicationWindow.window = glfwCreateWindow(WIDTH, HEIGHT, title.c_str(), nullptr, nullptr);
            applicationWindow.surfaceSize = VkExtent2D{ WIDTH, HEIGHT };
            auto result = glfwCreateWindowSurface((VkInstance&)(instance), applicationWindow.window, nullptr, (VkSurfaceKHR*)& applicationWindow.surface);
            if (result != VK_SUCCESS) { glfwTerminate(); exit(result); };
            return applicationWindow;
        }

        // getters
        inline VkSurfaceKHR surface() const { return applicationWindow.surface; }
        inline GLFWwindow* window() const { return applicationWindow.window; }
        inline const SurfaceFormat& format() const { return applicationWindow.surfaceFormat; }
        inline const VkExtent2D& size() const { return applicationWindow.surfaceSize; }

        // setters
        inline void format(SurfaceFormat format) { applicationWindow.surfaceFormat = format; }
        inline void size(const VkExtent2D & size) { applicationWindow.surfaceSize = size; }

        // 
        inline SurfaceFormat& getSurfaceFormat(VkPhysicalDevice gpu)
        {
            auto surfaceFormats = gpu.getSurfaceFormatsKHR(applicationWindow.surface);

            const std::vector<VkFormat> preferredFormats = { VkFormat::eR16G16B16A16Unorm, VkFormat::eA2B10G10R10UnormPack32, VkFormat::eA2R10G10B10UintPack32, VkFormat::eR8G8B8A8Srgb, VkFormat::eB8G8R8A8Srgb, VkFormat::eA8B8G8R8SrgbPack32, VkFormat::eR8G8B8A8Unorm, VkFormat::eB8G8R8A8Unorm, VkFormat::eA8B8G8R8UnormPack32 };

            VkFormat surfaceColorFormat =
                surfaceFormats.size() == 1 &&
                surfaceFormats[0].format == VkFormat::eUndefined
                ? VkFormat::eR8G8B8A8Srgb
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
            auto formatProperties = gpu.getFormatProperties(surfaceColorFormat);

            // only if these depth formats
            std::vector<VkFormat> depthFormats = {
                VkFormat::eD32SfloatS8Uint, VkFormat::eD32Sfloat,
                VkFormat::eD24UnormS8Uint, VkFormat::eD16UnormS8Uint,
                VkFormat::eD16Unorm };

            // choice supported depth format
            VkFormat surfaceDepthFormat = depthFormats[0];
            for (auto format : depthFormats) {
                auto depthFormatProperties = gpu.getFormatProperties(format);
                if (depthFormatProperties.optimalTilingFeatures & VkFormatFeatureFlagBits::eDepthStencilAttachment) {
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

        inline VkRenderPass& createRenderPass()
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
        inline void updateSwapchainFramebuffer(std::vector<Framebuffer> & swapchainBuffers, VkSwapchainKHR & swapchain, VkRenderPass & renderpass)
        {
            // The swapchain handles allocating frame images.
            auto& surfaceFormats = getSurfaceFormat(this->physicalDevice);
            auto  gpuMemoryProps = physicalDevice.getMemoryProperties();

            // 
            auto imageInfoVK = VkImageCreateInfo{};
            imageInfoVK.initialLayout = VkImageLayout::eUndefined;
            imageInfoVK.sharingMode = VkSharingMode::eExclusive;
            imageInfoVK.flags = {};
            imageInfoVK.pNext = nullptr;
            imageInfoVK.arrayLayers = 1;
            imageInfoVK.extent = VkExtent3D{ applicationWindow.surfaceSize.width, applicationWindow.surfaceSize.height, 1u };
            imageInfoVK.format = { surfaceFormats.depthFormat };
            imageInfoVK.imageType = VkImageType::e2D;
            imageInfoVK.mipLevels = 1;
            imageInfoVK.samples = VkSampleCountFlagBits::e1;
            imageInfoVK.tiling = VkImageTiling::eOptimal;
            imageInfoVK.usage = VkImageUsageFlagBits::eDepthStencilAttachment|VkImageUsageFlagBits::eTransferSrc;

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
            depthImageView = device.createImageView(VkImageViewCreateInfo{{}, depthImage, VkImageViewType::e2D, surfaceFormats.depthFormat, VkComponentMapping(), VkImageSubresourceRange{VkImageAspectFlagBits::eDepth, 0, 1, 0, 1} });

            // 
            auto swapchainImages = device.getSwapchainImagesKHR(swapchain);
            swapchainBuffers.resize(swapchainImages.size());
            for (int i = 0; i < swapchainImages.size(); i++)
            { // create framebuffers
                std::array<VkImageView, 2> views = {}; // predeclare views
                views[0] = device.createImageView(VkImageViewCreateInfo{ {}, swapchainImages[i], VkImageViewType::e2D, surfaceFormats.colorFormat, VkComponentMapping(), VkImageSubresourceRange{VkImageAspectFlagBits::eColor, 0, 1, 0, 1} }); // color view
                views[1] = depthImageView; // depth view
                swapchainBuffers[i].frameBuffer = device.createFramebuffer(VkFramebufferCreateInfo{ {}, renderpass, uint32_t(views.size()), views.data(), applicationWindow.surfaceSize.width, applicationWindow.surfaceSize.height, 1u });
            };
        }

        inline std::vector<Framebuffer> createSwapchainFramebuffer(VkSwapchainKHR swapchain, VkRenderPass renderpass) {
            // framebuffers vector
            std::vector<Framebuffer> swapchainBuffers = {};
            updateSwapchainFramebuffer(swapchainBuffers, swapchain, renderpass);
            for (int i = 0; i < swapchainBuffers.size(); i++)
            { // create semaphore
                VkSemaphoreTypeCreateInfo timeline = {};
                timeline.semaphoreType = VkSemaphoreType::eTimeline;
                timeline.initialValue = i;

                // 
                //swapchainBuffers[i].semaphore = device.createSemaphore(VkSemaphoreCreateInfo());
                swapchainBuffers[i].drawSemaphore = device.createSemaphore(VkSemaphoreCreateInfo());
                swapchainBuffers[i].computeSemaphore = device.createSemaphore(VkSemaphoreCreateInfo());
                swapchainBuffers[i].presentSemaphore = device.createSemaphore(VkSemaphoreCreateInfo());
                swapchainBuffers[i].waitFence = device.createFence(VkFenceCreateInfo().setFlags(VkFenceCreateFlagBits::eSignaled));
                ///swapchainBuffers[i].timeline = device.createSemaphore(VkSemaphoreCreateInfo().setPNext(&timeline));

                // 
                //device.signalSemaphore(VkSemaphoreSignalInfo().setSemaphore(swapchainBuffers[i].timeline).setValue(i));
            };
            return swapchainBuffers;
        }

        // create swapchain template
        inline VkSwapchainKHR createSwapchain()
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
            auto presentMode = VkPresentModeKHR::eImmediate;
            std::vector<VkPresentModeKHR> priorityModes = { VkPresentModeKHR::eImmediate, VkPresentModeKHR::eMailbox, VkPresentModeKHR::eFifoRelaxed, VkPresentModeKHR::eFifo, VkPresentModeKHR::eImmediate };

            bool found = false; // 
            for (auto pm : priorityModes) {
                if (found) break;
                for (auto sfm : surfacePresentModes) { if (pm == sfm) { presentMode = pm; found = true; break; } }
            };

            // swapchain info
            auto swapchainCreateInfo = VkSwapchainCreateInfoKHR();
            swapchainCreateInfo.surface = surface;
            swapchainCreateInfo.minImageCount = std::min(surfaceCapabilities.maxImageCount, 3u);
            swapchainCreateInfo.imageFormat = formats.colorFormat;
            swapchainCreateInfo.imageColorSpace = formats.colorSpace;
            swapchainCreateInfo.imageExtent = applicationWindow.surfaceSize;
            swapchainCreateInfo.imageArrayLayers = 1;
            swapchainCreateInfo.imageUsage = VkImageUsageFlagBits::eColorAttachment;
            swapchainCreateInfo.imageSharingMode = VkSharingMode::eExclusive;
            swapchainCreateInfo.preTransform = VkSurfaceTransformFlagBitsKHR::eIdentity;
            swapchainCreateInfo.compositeAlpha = VkCompositeAlphaFlagBitsKHR::eOpaque;
            swapchainCreateInfo.presentMode = presentMode;
            swapchainCreateInfo.clipped = true;

            // create swapchain
            return device.createSwapchainKHR(swapchainCreateInfo, nullptr);
        }
#endif
    };

}; // namespace NSM
