#pragma once // #

#ifndef VKT_INLINE_H
#define VKT_INLINE_H

// 
#include "./definition.hpp"

// Here Will HEADER's ONLY! 
// Here is NOT usable for C++20 modules!!!
namespace vkt {

#ifdef VKT_OPENGL_INTEROP
#ifndef VKT_USE_GLAD
    using namespace gl;
#endif
#endif

#ifdef ENABLE_OPTIX_DENOISE
#define OPTIX_CHECK(call)                                                      \
  do {                                                                         \
    OptixResult res = call;                                                    \
    if (res != OPTIX_SUCCESS) {                                                \
      std::stringstream ss;                                                    \
      ss << "Optix call (" << #call << " ) failed with code " << res           \
         << " (" __FILE__ << ":" << __LINE__ << ")\n";                         \
      std::cerr << ss.str().c_str() << std::endl;                              \
      throw std::runtime_error(ss.str().c_str());                              \
    }                                                                          \
  } while (false)

#define CUDA_CHECK(call)                                                       \
  do {                                                                         \
    cudaError_t error = call;                                                  \
    if (error != cudaSuccess) {                                                \
      std::stringstream ss;                                                    \
      ss << "CUDA call (" << #call << " ) failed with code " << error          \
         << " (" __FILE__ << ":" << __LINE__ << ")\n";                         \
      throw std::runtime_error(ss.str().c_str());                              \
    }                                                                          \
  } while (false)

#define OPTIX_CHECK_LOG(call)                                                  \
  do {                                                                         \
    OptixResult res = call;                                                    \
    if (res != OPTIX_SUCCESS) {                                                \
      std::stringstream ss;                                                    \
      ss << "Optix call (" << #call << " ) failed with code " << res           \
         << " (" __FILE__ << ":" << __LINE__ << ")\nLog:\n"                    \
         << log << "\n";                                                       \
      throw std::runtime_error(ss.str().c_str());                              \
    }                                                                          \
  } while (false)

    static void context_log_cb(unsigned int level, const char* tag,
        const char* message, void* /*cbdata */) {
        std::cerr << "[" << std::setw(2) << level << "][" << std::setw(12) << tag
            << "]: " << message << "\n";
    }
#endif

#ifdef VKT_OPENGL_INTEROP
#ifdef VKT_USE_GLFW
    // FOR LWJGL-3 Request!
    inline void initializeGL(GLFWglproc(*glfwGetProcAddress)(const char*)) {
#ifdef VKT_USE_GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            printf("Something went wrong!\n"); exit(-1);
        }
        printf("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
#else
        glbinding::initialize(glfwGetProcAddress, false);
#endif
    };

    // FOR LWJGL-3 Request!
#ifdef VKT_GLFW_LINKED
    inline void initializeGL() {
#ifdef VKT_USE_GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            printf("Something went wrong!\n"); exit(-1);
        }
        printf("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
#else
        glbinding::initialize(glfwGetProcAddress, false);
#endif
    };
#endif
#endif
#endif

};

// 
namespace vkt {

    static inline std::string errorString(VkResult errorCode) {
        switch (errorCode) {
#define STR(r) case VK_ ##r: return #r
            STR(NOT_READY);
            STR(TIMEOUT);
            STR(EVENT_SET);
            STR(EVENT_RESET);
            STR(INCOMPLETE);
            STR(ERROR_OUT_OF_HOST_MEMORY);
            STR(ERROR_OUT_OF_DEVICE_MEMORY);
            STR(ERROR_INITIALIZATION_FAILED);
            STR(ERROR_DEVICE_LOST);
            STR(ERROR_MEMORY_MAP_FAILED);
            STR(ERROR_LAYER_NOT_PRESENT);
            STR(ERROR_EXTENSION_NOT_PRESENT);
            STR(ERROR_FEATURE_NOT_PRESENT);
            STR(ERROR_INCOMPATIBLE_DRIVER);
            STR(ERROR_TOO_MANY_OBJECTS);
            STR(ERROR_FORMAT_NOT_SUPPORTED);
            STR(ERROR_SURFACE_LOST_KHR);
            STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
            STR(SUBOPTIMAL_KHR);
            STR(ERROR_OUT_OF_DATE_KHR);
            STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
            STR(ERROR_VALIDATION_FAILED_EXT);
            STR(ERROR_INVALID_SHADER_NV);
#undef STR
        default:
            return "UNKNOWN_ERROR";
        }
    };

    static inline decltype(auto) handleVk(vkh::uni_arg<VkResult> result) {
        if (result != VK_SUCCESS) { // TODO: Fix Ubuntu Issue
            //std::cerr << "ERROR: VkResult Error Code: " << std::to_string(result) << " (" << errorString(result) << ")..." << std::endl; throw (*result);

            assert(result == VK_SUCCESS);
#ifdef VKT_GLFW_LINKED
            glfwTerminate();
#endif
            exit(result);
        };
        return result;
    };


    //#ifdef USE_VULKAN
    template <typename T>
    static inline auto sgn(const T& val) { return (T(0) < val) - (val < T(0)); }

    template<class T = uint64_t>
    static inline T tiled(const T& sz, const T& gmaxtile) {
        // return (int32_t)ceil((double)sz / (double)gmaxtile);
        return sz <= 0 ? 0 : (sz / gmaxtile + sgn(sz % gmaxtile));
    }

    template <class T>
    static inline auto strided(const vkh::uni_arg<size_t>& sizeo) { return sizeof(T) * sizeo; }

    template <class T> static inline auto makeVector(const T* ptr, const size_t& size = 1) { std::vector<T>v(size); memcpy(v.data(), ptr, strided<T>(size)); return v; };

    template<class T, class Ty = T>
    std::vector<T> vector_cast(const std::vector<Ty>& Vy) {
        std::vector<T> V{}; for (auto& v : Vy) { V.push_back(reinterpret_cast<const T&>(v)); }; return std::move(V);
    };

    // 
    struct FixConstruction {
        FixConstruction(vkh::VkPipelineShaderStageCreateInfo spi = {}, vkh::VkPipelineShaderStageRequiredSubgroupSizeCreateInfoEXT sgmp = {}) : spi(spi), sgmp(sgmp) {
        }

        vkh::VkPipelineShaderStageCreateInfo spi = {};
        vkh::VkPipelineShaderStageRequiredSubgroupSizeCreateInfoEXT sgmp = {};

        // 
        operator vkh::VkPipelineShaderStageCreateInfo& () { return spi; };
        operator vkh::VkPipelineShaderStageRequiredSubgroupSizeCreateInfoEXT& () { return sgmp; };
        operator const vkh::VkPipelineShaderStageCreateInfo& () const { return spi; };
        operator const vkh::VkPipelineShaderStageRequiredSubgroupSizeCreateInfoEXT& () const { return sgmp; };
    };


    // 
#ifdef VULKAN_HPP
    template<class T>
    T handleHpp(vk::ResultValue<T> V) {
        //assert(V.result == VkResult::eSuccess);
        return std::move(V.value);
    };
#endif


    // Global initials
    class vkGlobal {
    public: // Currently, only Vulkan Loader
        static inline bool initialized = false;
        static inline std::shared_ptr<xvk::Loader> loader = {};
        static inline std::shared_ptr<xvk::Device> device = {};
        static inline std::shared_ptr<xvk::Instance> instance = {};

    #ifdef VKT_USE_GLFW
    #ifdef VKT_GLFW_LINKED
        vkGlobal(GLFWglproc(*glfwGetProcAddress)(const char*) = ::glfwGetProcAddress) 
    #else 
        vkGlobal(GLFWglproc(*glfwGetProcAddress)(const char*) = nullptr) 
    #endif
        {
            if (!initialized) {
                loader = std::make_shared<xvk::Loader>();
                if (!(initialized = (*loader)())) { std::cerr << "vulkan load failed..." << std::endl; };
    #ifdef VKT_OPENGL_INTEROP
                if (glfwGetProcAddress) {
                    vkt::initializeGL(glfwGetProcAddress);
                }
    #endif
            };
        };
    #else
        vkGlobal() {
            if (!initialized) {
                loader = std::make_shared<xvk::Loader>();
                if (!(initialized = (*loader)())) { std::cerr << "vulkan load failed..." << std::endl; };
            };
        };
    #endif
    };


    // TODO: native image barrier in library
    struct ImageBarrierInfo {
        vkt::Instance instanceDispatch = vkGlobal::instance;
        vkt::Device deviceDispatch = vkGlobal::device;
        vkh::uni_arg<VkImage> image = {};
        vkh::uni_arg<VkImageLayout> targetLayout = VK_IMAGE_LAYOUT_GENERAL;
        vkh::uni_arg<VkImageLayout> originLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        vkh::uni_arg<vkh::VkImageSubresourceRange> subresourceRange = vkh::VkImageSubresourceRange{ {}, 0u, 1u, 0u, 1u };
    };


    //
    static inline auto polyfillStage(const vkh::VkAccessFlags& accessFlags, vkh::VkPipelineStageFlags& stageFlags, bool write = false) {
        if (accessFlags.eShaderRead || accessFlags.eShaderWrite || accessFlags.eUniformRead) {
            stageFlags.eComputeShader = 1u, stageFlags.eFragmentShader = 1u, stageFlags.eGeometryShader = 1, stageFlags.eMeshShader = 1u, stageFlags.eRayTracingShader = 1u, stageFlags.eTaskShader = 1u, stageFlags.eTessellationControlShader = 1u, stageFlags.eTessellationEvaluationShader = 1u, stageFlags.eVertexShader = 1u;
        };
        if (accessFlags.eAccelerationStructureRead || accessFlags.eAccelerationStructureWrite) {
            stageFlags.eAccelerationStructureBuild = 1u;
        };
        if (accessFlags.eAccelerationStructureRead) {
            stageFlags.eRayTracingShader = 1u;
        };
        if (accessFlags.eColorAttachmentWrite || accessFlags.eColorAttachmentRead || accessFlags.eColorAttachmentReadNonCoherent) {
            stageFlags.eColorAttachmentOutput = 1u;
        };
        if (accessFlags.eCommandPreprocessRead || accessFlags.eCommandPreprocessWrite) {
            stageFlags.eCommandPreprocess = 1u;
        };
        if (accessFlags.eConditionalRenderingRead) {
            stageFlags.eConditionalRendering = 1u;
        };
        if (accessFlags.eDepthStencilAttachmentRead || accessFlags.eDepthStencilAttachmentWrite) {
            stageFlags.eEarlyFragmentTests = 1u, stageFlags.eLateFragmentTests = 1u;
        };
        if (accessFlags.eFragmentDensityMapRead) {
            stageFlags.eFragmentDensityProcess = 1u;
        };
        if (accessFlags.eHostRead || accessFlags.eHostWrite) {
            stageFlags.eHost = 1u;
        };
        if (accessFlags.eInputAttachmentRead) {
            stageFlags.eFragmentShader = 1u;
        };
        if (accessFlags.eShadingRateImageRead) {
            stageFlags.eShadingRateImage = 1u;
        };
        if (accessFlags.eTransferRead || accessFlags.eTransferWrite) {
            stageFlags.eTransfer = 1u;
        };
        if (accessFlags.eIndirectCommandRead || accessFlags.eTransformFeedbackCounterRead) {
            stageFlags.eDrawIndirect = 1u;
        };
        if (accessFlags.eTransformFeedbackWrite || accessFlags.eTransformFeedbackCounterWrite) {
            stageFlags.eTransformFeedback = 1u;
        };
        if (accessFlags.eVertexAttributeRead || accessFlags.eIndexRead) {
            stageFlags.eVertexInput = 1u;
        };
    };

    //
    static inline auto imageBarrier(const vkh::uni_arg<VkCommandBuffer>& cmd = VkCommandBuffer{}, const vkh::uni_arg<ImageBarrierInfo>& info = ImageBarrierInfo{}) {
        VkResult result = VK_SUCCESS; // planned to complete
        if (*info->originLayout == *info->targetLayout) { return result; }; // no need transfering more

        //
        vkh::VkImageMemoryBarrier imageMemoryBarriers = {};
        imageMemoryBarriers.srcQueueFamilyIndex = ~0U;
        imageMemoryBarriers.dstQueueFamilyIndex = ~0U;
        imageMemoryBarriers.oldLayout = info->originLayout;
        imageMemoryBarriers.newLayout = info->targetLayout;
        imageMemoryBarriers.subresourceRange = reinterpret_cast<const VkImageSubresourceRange&>(*info->subresourceRange);
        imageMemoryBarriers.image = info->image;

        // 
        auto srcStageMask = vkh::VkPipelineStageFlags{ .eBottomOfPipe = 1 };
        auto dstStageMask = vkh::VkPipelineStageFlags{ .eTopOfPipe = 1 };

        // 
        const auto dependencyFlags = vkh::VkDependencyFlags{};
        auto srcMask = vkh::VkAccessFlags{};
        auto dstMask = vkh::VkAccessFlags{};

        //
        typedef VkImageLayout il;
        typedef VkAccessFlagBits afb;

        // 
        switch (info->originLayout) {
            case VK_IMAGE_LAYOUT_UNDEFINED:                         break;
            case VK_IMAGE_LAYOUT_GENERAL:                           srcMask.eTransferWrite = 1u; break;
            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:          srcMask.eColorAttachmentWrite = 1u; break;
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:  srcMask.eDepthStencilAttachmentWrite = 1u; break;
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:   srcMask.eDepthStencilAttachmentRead = 1u; break;
            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:          srcMask.eShaderRead = 1u; break;
            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:              srcMask.eTransferRead = 1u; break;
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:              srcMask.eTransferWrite = 1u; break;
            case VK_IMAGE_LAYOUT_PREINITIALIZED:                    srcMask.eTransferWrite = 1u, srcMask.eHostWrite = 1u; break; srcMask.eMemoryRead = 1u; break;
            case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:                   srcMask.eMemoryRead = 1u; break;
        };

        //
        switch (info->targetLayout) {
            case VK_IMAGE_LAYOUT_UNDEFINED:                         break;
            case VK_IMAGE_LAYOUT_GENERAL:                           dstMask.eTransferWrite = 1u; break;
            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:          dstMask.eColorAttachmentWrite = 1u; break;
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:  dstMask.eDepthStencilAttachmentWrite = 1u; break;
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:   dstMask.eDepthStencilAttachmentRead = 1u; break;
            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:          dstMask.eShaderRead = 1u; break;
            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:              dstMask.eTransferRead = 1u; break;
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:              dstMask.eTransferWrite = 1u; break;
            case VK_IMAGE_LAYOUT_PREINITIALIZED:                    dstMask.eTransferWrite = 1u; break;
            case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:                   dstMask.eMemoryRead = 1u; break;
        };

        // assign access masks
        polyfillStage(imageMemoryBarriers.srcAccessMask = srcMask, srcStageMask);
        polyfillStage(imageMemoryBarriers.dstAccessMask = dstMask, dstStageMask);

        //
        info->deviceDispatch->vkCmdPipelineBarrier(cmd, srcStageMask, dstStageMask, {},
                                                    0u, nullptr,
                                                    0u, nullptr,
                                                    1u, imageMemoryBarriers);

        //
        return result;
    };

    //
    static inline auto polyfillAccess(const vkh::VkPipelineStageFlags& stageFlags, vkh::VkAccessFlags& accessFlags, bool write = false) {
        if (write) {
            if (stageFlags.eAccelerationStructureBuild) { accessFlags.eAccelerationStructureWrite = 1; };
            if (stageFlags.eBottomOfPipe || stageFlags.eTopOfPipe) { accessFlags.eMemoryWrite = 1; };
            if (stageFlags.eColorAttachmentOutput) { accessFlags.eColorAttachmentWrite = 1; };
            if (stageFlags.eCommandPreprocess) { accessFlags.eCommandPreprocessWrite = 1; };
            if (stageFlags.eComputeShader || stageFlags.eFragmentShader || stageFlags.eGeometryShader || stageFlags.eMeshShader || stageFlags.eRayTracingShader || stageFlags.eTaskShader | stageFlags.eTessellationControlShader | stageFlags.eTessellationEvaluationShader | stageFlags.eVertexShader) { accessFlags.eShaderWrite = 1; };
            if (stageFlags.eHost) { accessFlags.eHostWrite = 1; };
            if (stageFlags.eTransfer) { accessFlags.eTransferWrite = 1; };
            if (stageFlags.eTransformFeedback) { accessFlags.eTransformFeedbackWrite = 1; accessFlags.eTransformFeedbackCounterWrite = 1; };
            if (stageFlags.eEarlyFragmentTests || stageFlags.eLateFragmentTests) { accessFlags.eDepthStencilAttachmentWrite = 1; };
        } else {
            if (stageFlags.eAccelerationStructureBuild) { accessFlags.eAccelerationStructureRead = 1; };
            if (stageFlags.eBottomOfPipe || stageFlags.eTopOfPipe) { accessFlags.eMemoryRead = 1; };
            if (stageFlags.eColorAttachmentOutput) { accessFlags.eColorAttachmentRead = 1; };
            if (stageFlags.eCommandPreprocess) { accessFlags.eCommandPreprocessRead = 1; };
            if (stageFlags.eComputeShader || stageFlags.eFragmentShader || stageFlags.eGeometryShader || stageFlags.eMeshShader || stageFlags.eRayTracingShader || stageFlags.eTaskShader | stageFlags.eTessellationControlShader | stageFlags.eTessellationEvaluationShader | stageFlags.eVertexShader) { accessFlags.eShaderRead = 1; accessFlags.eUniformRead = 1; };
            if (stageFlags.eFragmentShader) { accessFlags.eInputAttachmentRead = 1; };
            if (stageFlags.eHost) { accessFlags.eHostRead = 1; };
            if (stageFlags.eTransfer) { accessFlags.eTransferRead = 1; };
            //if (stageFlags.eTransformFeedback) { accessFlags.eTransformFeedbackCounterRead = 1; };
            if (stageFlags.eDrawIndirect) { accessFlags.eTransformFeedbackCounterRead = 1; };
            if (stageFlags.eFragmentDensityProcess) { accessFlags.eFragmentDensityMapRead = 1; };
            if (stageFlags.eConditionalRendering) { accessFlags.eConditionalRenderingRead = 1; };
            if (stageFlags.eDrawIndirect) { accessFlags.eIndirectCommandRead = 1; };
            if (stageFlags.eShadingRateImage) { accessFlags.eShadingRateImageRead = 1; };
            if (stageFlags.eVertexInput) { accessFlags.eVertexAttributeRead = 1; accessFlags.eIndexRead = 1; };
            if (stageFlags.eEarlyFragmentTests || stageFlags.eLateFragmentTests) { accessFlags.eDepthStencilAttachmentRead = 1; };
            if (stageFlags.eRayTracingShader) { accessFlags.eAccelerationStructureRead = 1; };
        };
    };

    //
    struct MemoryBarrierInfo {
        vkt::Instance instanceDispatch = vkGlobal::instance;
        vkt::Device deviceDispatch = vkGlobal::device;
        FLAGS(VkPipelineStage) srcStageMask = {}; bool srcRead = false; bool srcWrite = true;
        FLAGS(VkPipelineStage) dstStageMask = {}; bool dstRead = true; bool dstWrite = false;
    };

    //
    static inline auto memoryBarrier(const vkh::uni_arg<VkCommandBuffer>& cmd = VkCommandBuffer{}, const vkh::uni_arg<MemoryBarrierInfo>& info = MemoryBarrierInfo{}) {
        vkh::VkMemoryBarrier memoryBarrier = {};

        // 
        if (info->srcWrite) { polyfillAccess(info->srcStageMask, memoryBarrier.srcAccessMask, true); };
        if (info->srcRead) { polyfillAccess(info->srcStageMask, memoryBarrier.srcAccessMask, false); };
        if (info->dstWrite) { polyfillAccess(info->dstStageMask, memoryBarrier.dstAccessMask, true); };
        if (info->dstRead) { polyfillAccess(info->dstStageMask, memoryBarrier.dstAccessMask, false); };

        // 
        info->deviceDispatch->vkCmdPipelineBarrier(cmd, info->srcStageMask, info->dstStageMask, {},
            1u, memoryBarrier,
            0u, nullptr,
            0u, nullptr);
    };

#ifdef VKH_USE_XVK
    inline auto& vsEnumeratePhysicalDevices(vkt::Instance instance, std::vector<VkPhysicalDevice>& data) {
        uint32_t count = 0u; vkt::handleVk(instance->EnumeratePhysicalDevices(&count, nullptr)); data.resize(count);
        handleVk(instance->EnumeratePhysicalDevices(&count, data.data()));
        return data;
    };

    inline auto& vsEnumerateDeviceExtensionProperties(vkt::Instance instance, const VkPhysicalDevice& physicalDevice, std::vector<VkExtensionProperties>& data, const std::string& layerName = std::string("")) {
        uint32_t count = 0u; vkt::handleVk(instance->vkEnumerateDeviceExtensionProperties(physicalDevice, layerName.c_str(), &count, nullptr)); data.resize(count);
        vkt::handleVk(instance->vkEnumerateDeviceExtensionProperties(physicalDevice, layerName.c_str(), &count, data.data()));
        return data;
    };

    inline auto& vsEnumerateDeviceLayerProperties(vkt::Instance instance, const VkPhysicalDevice& physicalDevice, std::vector<VkLayerProperties>& data) {
        uint32_t count = 0u; vkt::handleVk(instance->EnumerateDeviceLayerProperties(physicalDevice, &count, nullptr)); data.resize(count);
        vkt::handleVk(instance->EnumerateDeviceLayerProperties(physicalDevice, &count, data.data()));
        return data;
    };

    inline auto& vsGetPhysicalDeviceQueueFamilyProperties(vkt::Instance instance, const VkPhysicalDevice& physicalDevice, std::vector<vkh::VkQueueFamilyProperties>& data) {
        uint32_t count = 0u; (instance->GetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr)); data.resize(count);
        (instance->GetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, reinterpret_cast<::VkQueueFamilyProperties*>(data.data())));
        return data;
    };

    inline auto& vsEnumerateInstanceExtensionProperties(vkh::uni_ptr<xvk::Loader> loader, std::vector<VkExtensionProperties>& data, const std::string& layerName = std::string("")) {
        uint32_t count = 0u; vkt::handleVk(loader->vkEnumerateInstanceExtensionProperties(layerName.c_str(), &count, nullptr)); data.resize(count);
        vkt::handleVk(loader->vkEnumerateInstanceExtensionProperties(layerName.c_str(), &count, data.data()));
        return data;
    };

    inline auto& vsEnumerateInstanceLayerProperties(vkh::uni_ptr<xvk::Loader> loader, std::vector<VkLayerProperties>& data) {
        uint32_t count = 0u; vkt::handleVk(loader->vkEnumerateInstanceLayerProperties(&count, nullptr)); data.resize(count);
        vkt::handleVk(loader->vkEnumerateInstanceLayerProperties(&count, data.data()));
        return data;
    };

    inline auto vsEnumerateInstanceVersion(vkh::uni_ptr<xvk::Loader> loader) {
        uint32_t version = 0u; vkt::handleVk(loader->vkEnumerateInstanceVersion(&version)); return version;
    };

    inline auto& vsGetPhysicalDeviceSurfaceFormatsKHR(vkt::Instance instance, const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface, std::vector<VkSurfaceFormatKHR>& data) { // TODO: V2
        uint32_t count = 0u; vkt::handleVk(instance->GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, nullptr)); data.resize(count);
        vkt::handleVk(instance->GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, reinterpret_cast<VkSurfaceFormatKHR*>(data.data())));
        return data;
    };

    inline auto vsGetPhysicalDeviceFormatProperties(vkt::Instance instance, const VkPhysicalDevice& physicalDevice, const VkFormat& format) {
        VkFormatProperties props = {}; (instance->GetPhysicalDeviceFormatProperties(physicalDevice, format, &props)); return props;
    };


    inline auto vsGetPhysicalDeviceMemoryProperties(vkt::Instance instance, const VkPhysicalDevice& physicalDevice) {
        VkPhysicalDeviceMemoryProperties props = {}; (instance->GetPhysicalDeviceMemoryProperties(physicalDevice, &reinterpret_cast<::VkPhysicalDeviceMemoryProperties&>(props))); return props;
    };

    inline auto vsGetPhysicalDeviceMemoryProperties2(vkt::Instance instance, const VkPhysicalDevice& physicalDevice, vkh::VkPhysicalDeviceMemoryProperties2& props) {
        (instance->GetPhysicalDeviceMemoryProperties2(physicalDevice, &reinterpret_cast<::VkPhysicalDeviceMemoryProperties2&>(props))); return props;
    };


    inline auto vsGetPhysicalDeviceProperties(vkt::Instance instance, const VkPhysicalDevice& physicalDevice) {
        VkPhysicalDeviceProperties props = {}; instance->GetPhysicalDeviceProperties(physicalDevice, &reinterpret_cast<::VkPhysicalDeviceProperties&>(props)); return props;
    };

    inline auto vsGetPhysicalDeviceProperties2(vkt::Instance instance, const VkPhysicalDevice& physicalDevice, vkh::VkPhysicalDeviceProperties2& props) {
        instance->GetPhysicalDeviceProperties2(physicalDevice, &reinterpret_cast<::VkPhysicalDeviceProperties2&>(props)); return props;
    };


    inline auto vsGetPhysicalDeviceFeatures(vkt::Instance instance, const VkPhysicalDevice& physicalDevice) {
        VkPhysicalDeviceProperties props = {}; instance->GetPhysicalDeviceFeatures(physicalDevice, &reinterpret_cast<::VkPhysicalDeviceFeatures&>(props)); return props;
    };

    inline auto vsGetPhysicalDeviceFeatures2(vkt::Instance instance, const VkPhysicalDevice& physicalDevice, vkh::VkPhysicalDeviceFeatures2& props) {
        instance->GetPhysicalDeviceFeatures2(physicalDevice, &reinterpret_cast<::VkPhysicalDeviceFeatures2&>(props)); return props;
    };


    inline auto vsGetSwapchainImagesKHR(vkt::Device device, const VkSwapchainKHR& swapchain) { // TODO: V2
        uint32_t count = 0u; device->GetSwapchainImagesKHR(swapchain, &count, nullptr);
        std::vector<VkImage> data(count);
        handleVk(device->GetSwapchainImagesKHR(swapchain, &count, reinterpret_cast<VkImage*>(data.data())));
        return data;
    };

    inline auto vsGetPhysicalDeviceSurfaceCapabilitiesKHR(vkt::Instance instance, const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface) {
        VkSurfaceCapabilitiesKHR props = {}; handleVk(instance->GetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &reinterpret_cast<::VkSurfaceCapabilitiesKHR&>(props))); return props;
    };

    inline auto vsGetPhysicalDeviceSurfacePresentModesKHR(vkt::Instance instance, const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface) { // TODO: V2
        uint32_t count = 0u; vkt::handleVk(instance->GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, nullptr));
        std::vector<VkPresentModeKHR> data(count);
        handleVk(instance->GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, reinterpret_cast<::VkPresentModeKHR*>(data.data())));
        return data;
    };
#endif

};

#endif
