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
#ifndef VKT_CORE_ENABLE_XVK
#define VKT_CORE_ENABLE_XVK
#endif

//
#ifndef VKT_CORE_ENABLE_VMA
#define VKT_CORE_ENABLE_VMA
#endif

//
#ifndef ENABLE_VULKAN_HPP
#define ENABLE_VULKAN_HPP
#endif

// Force include for avoid GLAD problem...
#include "core.hpp"
#include "essential.hpp"
#include "vector.hpp"
#include "image.hpp"
#include "utils.hpp"
#include "structs.hpp"

//#define VKT_ENABLE_DEBUG

// TODO: FULL REWRITE OF THAT "PROJECT"!!!
namespace vkt
{

    // CRITICAL: NEEDS SUPPORT FOR JAVACPP!!! 
    class GPUFramework : std::enable_shared_from_this<GPUFramework> {
    protected:

        std::vector<const char*> usedDeviceExtensions = {};
        std::vector<const char*> usedExtensions = {};

        std::vector<const char*> usedDeviceLayers = {};
        std::vector<const char*> usedLayers = {};

        std::vector<vkh::VkDeviceQueueCreateInfo> usedQueueCreateInfos = {};

    public: friend GPUFramework;
        GPUFramework() { vkt::vkGlobal(); };
        GPUFramework(vkt::uni_ptr<GPUFramework> fw) { *this = fw; vkt::vkGlobal(); };
        GPUFramework& operator=(vkt::uni_ptr<GPUFramework> fw);

        // minimal features
        vkh::VkPhysicalDeviceProperties2 gProperties{};
        vkh::VkPhysicalDeviceFeatures2 gFeatures{};
        vkh::VkPhysicalDeviceMemoryProperties2 memoryProperties = {};

        // 
        vk::PhysicalDeviceShaderAtomicFloatFeaturesEXT gAtomicFloat{};
        vk::PhysicalDeviceTransformFeedbackFeaturesEXT gTrasformFeedback{};
        vk::PhysicalDeviceRayTracingFeaturesKHR gRayTracing{};
        vk::PhysicalDeviceTexelBufferAlignmentFeaturesEXT gTexelBufferAligment{};
        vk::PhysicalDevice16BitStorageFeatures gStorage16{};
        vk::PhysicalDevice8BitStorageFeatures gStorage8{};
        vk::PhysicalDeviceDescriptorIndexingFeaturesEXT gDescIndexing{};
        vk::PhysicalDeviceFloat16Int8FeaturesKHR gFloat16U8{}; // Vulkan 1.3
        vk::PhysicalDeviceBufferDeviceAddressFeatures gDeviceAddress{};
        vk::PhysicalDeviceFragmentShaderBarycentricFeaturesNV gBarycentric{};
        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT gExtendedDynamic{};

        // XVK loaded (NEW!)
        VmaVulkanFunctions func = {};
        vkt::uni_ptr<xvk::Instance> instanceDispatch = {};
        vkt::uni_ptr<xvk::Device> deviceDispatch = {};

        // JavaCPP and XVK compatible (NEW!)
        vkh::VkApplicationInfo applicationInfo = {};
        vkh::VkInstanceCreateInfo instanceCreate = {};
        vkh::VkDeviceCreateInfo deviceCreate = {};

        // 
        VkFence fence = VK_NULL_HANDLE;
        VkQueue queue = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;
        VkInstance instance = VK_NULL_HANDLE;
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkCommandPool commandPool = VK_NULL_HANDLE;
        //VkImage depthImage = VK_NULL_HANDLE;
        //VkImageView depthImageView = VK_NULL_HANDLE;
        VkPipelineCache pipelineCache = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
        vkt::ImageRegion depthImage = {};

        VmaAllocator allocator = VK_NULL_HANDLE;
        uint32_t queueFamilyIndex = 0;
        uint32_t instanceVersion = 0;

        VmaAllocation vmaDepthImageAllocation = VK_NULL_HANDLE;
        VmaAllocationInfo vmaDepthImageAllocationInfo = {};

        std::vector<VkPhysicalDevice> physicalDevices = {};
        std::vector<uint32_t> queueFamilyIndices = {};

        vkt::MemoryAllocationInfo memoryAllocInfo = {};


        // 
        public: virtual GPUFramework* submitOnce(const std::function<void(VkCommandBuffer&)>& cmdFn = {}, const vkt::uni_arg<vkh::VkSubmitInfo>& smbi = vkh::VkSubmitInfo{}) {
            vkt::submitOnce(this->getDeviceDispatch(), VkQueue(*this), VkCommandPool(*this), cmdFn, smbi);
            return this;
        };

        // Async Version
        public: virtual std::future<GPUFramework*> submitOnceAsync(const std::function<void(VkCommandBuffer&)>& cmdFn = {}, const vkt::uni_arg<vkh::VkSubmitInfo>& smbi = vkh::VkSubmitInfo{}) {
            return std::async(std::launch::async | std::launch::deferred, [=, this]() {
                vkt::submitOnceAsync(this->getDeviceDispatch(), VkQueue(*this), VkCommandPool(*this), cmdFn, smbi).get();
                return this;
            });
        };

        // 
        public: virtual GPUFramework* submitCmd(const vkt::uni_arg<VkCommandBuffer>& cmds, const vkt::uni_arg<vkh::VkSubmitInfo>& smbi = vkh::VkSubmitInfo{}) {
            return this->submitCmd(std::vector<VkCommandBuffer>{ cmds }, smbi);
        };

        // 
        public: virtual GPUFramework* submitCmd(const std::vector<VkCommandBuffer>& cmds, const vkt::uni_arg<vkh::VkSubmitInfo>& smbi = vkh::VkSubmitInfo{}) {
            vkt::submitCmd(this->getDeviceDispatch(), VkQueue(*this), cmds, smbi);
            return this;
        };

        // Async Version
        public: virtual std::future<GPUFramework*> submitCmdAsync(const vkt::uni_arg<VkCommandBuffer>& cmds, const vkt::uni_arg<vkh::VkSubmitInfo>& smbi = vkh::VkSubmitInfo{}) {
            return this->submitCmdAsync(std::vector<VkCommandBuffer>{ cmds }, smbi);
        };

        // Async Version
        public: virtual std::future<GPUFramework*> submitCmdAsync(const std::vector<VkCommandBuffer>& cmds, const vkt::uni_arg<vkh::VkSubmitInfo>& smbi = vkh::VkSubmitInfo{}) {
            return std::async(std::launch::async | std::launch::deferred, [=, this]() {
                vkt::submitCmdAsync(this->getDeviceDispatch(), VkQueue(*this), cmds, smbi).get();
                return this;
            });
        };


        //VkDevice createDevice(bool isComputePrior = true, std::string shaderPath = "./", bool enableAdvancedAcceleration = true);
        inline virtual VkPhysicalDevice& getPhysicalDevice(const uint32_t& gpuID) { physicalDevice = physicalDevices[gpuID]; return physicalDevice; };
        inline virtual VkPhysicalDevice& getPhysicalDevice() { if (!physicalDevice && physicalDevices.size() > 0) { physicalDevice = physicalDevices[0u]; }; return physicalDevice; };

        // Compatible with JavaCPP directly
        inline virtual uintptr_t getInstanceCreateInfoAddress() { return (uintptr_t)(&instanceCreate); };
        inline virtual uintptr_t getDeviceCreateInfoAddress() { return (uintptr_t)(&deviceCreate); };
        inline virtual uintptr_t getInstanceCreateInfoAddress() const { return (uintptr_t)(&instanceCreate); };
        inline virtual uintptr_t getDeviceCreateInfoAddress() const { return (uintptr_t)(&deviceCreate); };

        // 
        //inline VkDispatchLoaderDynamic getDispatch() { return dispatch; };
        inline virtual VkDevice& getDevice() { return device; };
        inline virtual VkQueue& getQueue() { return queue; };
        inline virtual VkFence& getFence() { return fence; };
        inline virtual VkInstance& getInstance() { return instance; };
        inline virtual VkCommandPool& getCommandPool() { return commandPool; };
        inline virtual VkPipelineCache& getPipelineCache() { return pipelineCache; };
        inline virtual VkDescriptorPool& getDescriptorPool() { return descriptorPool; };
        inline virtual VkPhysicalDeviceMemoryProperties2& getMemoryProperties() { return memoryProperties; };
        inline virtual VmaAllocator& getAllocator() { return allocator; };

        // 
        //inline const VkDispatchLoaderDynamic getDispatch() const { return dispatch; };
        inline virtual const VkPhysicalDevice& getPhysicalDevice() const { return physicalDevice; };
        inline virtual const VkDevice& getDevice() const { return device; };
        inline virtual const VkQueue& getQueue() const { return queue; };
        inline virtual const VkFence& getFence() const { return fence; };
        inline virtual const VkInstance& getInstance() const { return instance; };
        inline virtual const VkCommandPool& getCommandPool() const { return commandPool; };
        inline virtual const VkPipelineCache& getPipelineCache() const { return pipelineCache; };
        inline virtual const VkDescriptorPool& getDescriptorPool() const { return descriptorPool; };
        inline virtual const VkPhysicalDeviceMemoryProperties2& getMemoryProperties() const { return memoryProperties; };
        inline virtual const VmaAllocator& getAllocator() const { return allocator; };

        // 
        inline virtual vkt::uni_ptr<xvk::Device>& getDeviceDispatch() { return deviceDispatch; };
        inline virtual vkt::uni_ptr<xvk::Instance>& getInstanceDispatch() { return instanceDispatch; };

        // 
        inline virtual const VkDevice& getDeviceDispatch() const { return device; };
        inline virtual const VkInstance& getInstanceDispatch() const { return instance; };

        // 
        inline virtual operator VkPhysicalDevice&() { return physicalDevice; };
        inline virtual operator VkDevice&() { return device; };
        inline virtual operator VkQueue&() { return queue; };
        inline virtual operator VkFence&() { return fence; };
        inline virtual operator VkInstance&() { return instance; };
        inline virtual operator VkCommandPool&() { return commandPool; };
        inline virtual operator VkPipelineCache&() { return pipelineCache; };
        inline virtual operator VkDescriptorPool&() { return descriptorPool; };
        inline virtual operator VkPhysicalDeviceMemoryProperties2& () { return memoryProperties; };
        inline virtual operator VmaAllocator&() { return allocator; };

        //
        inline virtual operator const VkPhysicalDevice&() const { return physicalDevice; };
        inline virtual operator const VkDevice&() const { return device; };
        inline virtual operator const VkQueue&() const { return queue; };
        inline virtual operator const VkFence&() const { return fence; };
        inline virtual operator const VkInstance&() const { return instance; };
        inline virtual operator const VkCommandPool&() const { return commandPool; };
        inline virtual operator const VkPipelineCache&() const { return pipelineCache; };
        inline virtual operator const VkDescriptorPool&() const { return descriptorPool; };
        inline virtual operator const VkPhysicalDeviceMemoryProperties2& () const { return memoryProperties; };
        inline virtual operator const VmaAllocator&() const { return allocator; };

#ifdef VKT_ENABLE_GLFW_SUPPORT
        struct SurfaceWindow {
            SurfaceFormat surfaceFormat = {};
            VkExtent2D surfaceSize = VkExtent2D{ 0u, 0u };
            VkSurfaceKHR surface = VK_NULL_HANDLE;
            VkSwapchainKHR swapchain = VK_NULL_HANDLE;
            VkRenderPass renderPass = VK_NULL_HANDLE;
            GLFWwindow* window = nullptr;
            GLFWwindow* opengl = nullptr;
            std::vector<Framebuffer> swapchainBuffers = {};
        } applicationWindow = {};

        inline virtual VkSurfaceKHR& getSurface() { return applicationWindow.surface; };
        inline virtual VkRenderPass& getRenderPass() { return applicationWindow.renderPass; };
        inline virtual VkSwapchainKHR& getSwapchain() { return applicationWindow.swapchain; };

        inline virtual const VkSurfaceKHR& getSurface() const { return applicationWindow.surface; };
        inline virtual const VkRenderPass& getRenderPass() const { return applicationWindow.renderPass; };
        inline virtual const VkSwapchainKHR& getSwapchain() const { return applicationWindow.swapchain; };
#endif

    public:

        inline virtual void loadXVK() {
            if (instance && !instanceDispatch) { instanceDispatch = std::make_shared<xvk::Instance>(vkGlobal::loader.get(), instance); }
            if (device   && !deviceDispatch)   {   deviceDispatch = std::make_shared<xvk::Device  >(vkGlobal::instance.get(), device); }
            if (!vkGlobal::instance) { vkGlobal::instance = instanceDispatch.get_shared(); }
            if (!vkGlobal::device  ) { vkGlobal::device   =   deviceDispatch.get_shared(); }
        };

        inline virtual uintptr_t memoryAllocationInfoPtr() {
            memoryAllocInfo.device = device;
            memoryAllocInfo.memoryProperties = this->getMemoryProperties().memoryProperties;
            memoryAllocInfo.instanceDispatch = this->getInstanceDispatch();
            memoryAllocInfo.deviceDispatch = this->getDeviceDispatch();
            memoryAllocInfo.instance = this->getInstance();
            memoryAllocInfo.device = this->getDevice();
            return uintptr_t(&memoryAllocInfo);
        };

        inline virtual MemoryAllocationInfo& memoryAllocationInfo() {
            memoryAllocInfo.device = device;
            memoryAllocInfo.memoryProperties = this->getMemoryProperties().memoryProperties;
            memoryAllocInfo.instanceDispatch = this->getInstanceDispatch();
            memoryAllocInfo.deviceDispatch = this->getDeviceDispatch();
            memoryAllocInfo.instance = this->getInstance();
            memoryAllocInfo.device = this->getDevice();
            return memoryAllocInfo;
        };

        inline virtual VkImageView& getDepthImageView() {
            return depthImage.getImageView();
        };

        inline virtual VkImage& getDepthImage() {
            return depthImage.getImage();
        };

        virtual VkInstance& createInstance();

        // New constructor for clarify physical device
        inline virtual VkDevice& createDevice(const VkPhysicalDevice& physicalDevice, bool isComputePrior = true, std::string shaderPath = "", bool enableAdvancedAcceleration = false) {
            this->physicalDevice = physicalDevice; return this->createDevice(isComputePrior, shaderPath, enableAdvancedAcceleration);
        };

        //
        virtual VkDevice& createDevice(bool isComputePrior = true, std::string shaderPath = "", bool enableAdvancedAcceleration = false);

#ifdef VKT_ENABLE_GLFW_SUPPORT
        // create window and surface for this application (multi-window not supported)
        inline virtual SurfaceWindow& createWindowSurface(GLFWwindow* window, uint32_t WIDTH, uint32_t HEIGHT) {
            applicationWindow.window = window;
            applicationWindow.surfaceSize = VkExtent2D{ WIDTH, HEIGHT };
            glfwMakeContextCurrent(nullptr); // CONTEXT-REQUIRED!
#ifdef VKT_ENABLE_GLFW_SURFACE
#ifdef VKT_ENABLE_GLFW_LINKED
            vkh::handleVk(glfwCreateWindowSurface((VkInstance&)(instance), applicationWindow.window, nullptr, (VkSurfaceKHR*)&applicationWindow.surface));
#endif
#endif
            glfwMakeContextCurrent(window); // CONTEXT-REQUIRED!
            return applicationWindow;
        };

        // create window and surface for this application (multi-window not supported)
        inline virtual SurfaceWindow& createWindowSurface(SurfaceWindow& applicationWindow) {
            applicationWindow.surfaceSize = VkExtent2D{ applicationWindow.surfaceSize.width, applicationWindow.surfaceSize.height };
            glfwMakeContextCurrent(nullptr); // CONTEXT-REQUIRED!
#ifdef VKT_ENABLE_GLFW_SURFACE
#ifdef VKT_ENABLE_GLFW_LINKED
            vkh::handleVk(glfwCreateWindowSurface((VkInstance&)(instance), applicationWindow.window, nullptr, (VkSurfaceKHR*)&applicationWindow.surface));
#endif
#endif
            glfwMakeContextCurrent(applicationWindow.window); // CONTEXT-REQUIRED!
            return applicationWindow;
        };


        // 
        inline virtual SurfaceWindow& getAppObject() {
            return applicationWindow;
        };

        // create window and surface for this application (multi-window not supported)
        inline virtual SurfaceWindow& createWindowSurface(uint32_t WIDTH, uint32_t HEIGHT, std::string title = "TestApp") {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
            //applicationWindow.opengl = glfwCreateWindow(WIDTH, HEIGHT, title.c_str(), nullptr, nullptr);
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            applicationWindow.window = glfwCreateWindow(WIDTH, HEIGHT, title.c_str(), nullptr, nullptr);
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

            // Make Surface
            return this->createWindowSurface(applicationWindow.window, WIDTH, HEIGHT);
        };

        //
        inline virtual SurfaceWindow& createWindowOnly(uint32_t WIDTH, uint32_t HEIGHT, std::string title = "TestApp") {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            applicationWindow.window = glfwCreateWindow(WIDTH, HEIGHT, title.c_str(), nullptr, nullptr);
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
            return applicationWindow;
        };

        // getters
        inline virtual VkSurfaceKHR surface() const { return applicationWindow.surface; }
        inline virtual GLFWwindow* window() const { return applicationWindow.window; }
        inline virtual const SurfaceFormat& format() const { return applicationWindow.surfaceFormat; }
        inline virtual const VkExtent2D& size() const { return applicationWindow.surfaceSize; }

        // setters
        inline virtual void format(SurfaceFormat format) { applicationWindow.surfaceFormat = format; }
        inline virtual void size(const VkExtent2D & size) { applicationWindow.surfaceSize = size; }

        // 
        virtual SurfaceFormat& getSurfaceFormat(VkPhysicalDevice gpu);

        // 
        virtual VkRenderPass& createRenderPass();
        
        // update swapchain framebuffer
        virtual void updateSwapchainFramebuffer(std::vector<Framebuffer>& swapchainBuffers, VkSwapchainKHR& swapchain, VkRenderPass& renderpass);

        // 
        virtual std::vector<Framebuffer>& createSwapchainFramebuffer(VkSwapchainKHR& swapchain, VkRenderPass& renderpass);


        // 
        inline virtual void updateSwapchainFramebuffer(std::vector<Framebuffer>& swapchainBuffers) {
            return updateSwapchainFramebuffer(swapchainBuffers, applicationWindow.swapchain, applicationWindow.renderPass);
        }

        // 
        inline virtual void updateSwapchainFramebuffer() {
            return updateSwapchainFramebuffer(applicationWindow.swapchainBuffers, applicationWindow.swapchain, applicationWindow.renderPass);
        }

        // 
        inline virtual std::vector<Framebuffer>& createSwapchainFramebuffer() {
            return createSwapchainFramebuffer(applicationWindow.swapchain, applicationWindow.renderPass);
        }


        // 
        inline virtual void updateSwapchainFramebuffer(std::vector<Framebuffer>& swapchainBuffers, VkRenderPass& renderPass) {
            return updateSwapchainFramebuffer(swapchainBuffers, applicationWindow.swapchain, renderPass);
        }

        // 
        inline virtual void updateSwapchainFramebuffer(VkRenderPass& renderPass) {
            return updateSwapchainFramebuffer(applicationWindow.swapchainBuffers, applicationWindow.swapchain, renderPass);
        }

        // 
        inline virtual std::vector<Framebuffer>& createSwapchainFramebuffer(VkRenderPass& renderPass) {
            return createSwapchainFramebuffer(applicationWindow.swapchain, renderPass);
        }

        // create swapchain template
        virtual VkSwapchainKHR createSwapchain();
#endif
    };



}; // namespace NSM
