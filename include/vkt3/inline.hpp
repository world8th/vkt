#pragma once // #

#ifndef VKT_INLINE_H
#define VKT_INLINE_H

// 
#include "./definition.hpp"

// Here Will HEADER's ONLY! 
// Here is NOT usable for C++20 modules!!!
namespace vkt {

#ifdef ENABLE_OPENGL_INTEROP
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

    //#ifdef USE_VULKAN
    template <typename T>
    static inline auto sgn(const T& val) { return (T(0) < val) - (val < T(0)); }

    template<class T = uint64_t>
    static inline T tiled(const T& sz, const T& gmaxtile) {
        // return (int32_t)ceil((double)sz / (double)gmaxtile);
        return sz <= 0 ? 0 : (sz / gmaxtile + sgn(sz % gmaxtile));
    }

    template <class T>
    static inline auto strided(const vkt::uni_arg<size_t>& sizeo) { return sizeof(T) * sizeo; }

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

    #ifdef VKT_ENABLE_GLFW_SUPPORT
    #ifdef VKT_ENABLE_GLFW_LINKED
        vkGlobal(GLFWglproc(*glfwGetProcAddress)(const char*) = ::glfwGetProcAddress) 
    #else 
        vkGlobal(GLFWglproc(*glfwGetProcAddress)(const char*) = nullptr) 
    #endif
        {
            if (!initialized) {
                loader = std::make_shared<xvk::Loader>();
                if (!(initialized = (*loader)())) { std::cerr << "vulkan load failed..." << std::endl; };
    #ifdef ENABLE_OPENGL_INTEROP
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

    // 
    struct VmaMemoryInfo {
        VmaMemoryUsage memUsage = VMA_MEMORY_USAGE_GPU_ONLY;
        std::shared_ptr<xvk::Instance> instanceDispatch = vkGlobal::instance;
        std::shared_ptr<xvk::Device> deviceDispatch = vkGlobal::device;


        // Resolve JavaCPP Problem...
        VmaMemoryInfo& setDeviceDispatch(std::shared_ptr<xvk::Device> ptr) { deviceDispatch = ptr; return *this; };
        VmaMemoryInfo& setInstanceDispatch(std::shared_ptr<xvk::Instance> ptr) { instanceDispatch = ptr; return *this; };

        // Resolve JavaCPP Problem...
        std::shared_ptr<xvk::Device>& getDeviceDispatch() { return deviceDispatch; };
        std::shared_ptr<xvk::Instance>& getInstanceDispatch() { return instanceDispatch; };
    };

    // TODO: Add XVK support
    struct MemoryAllocationInfo { //
        uint32_t glID = 0u, glMemory = 0u;      // 0U
#ifdef VKT_WIN32_DETECTED
        HANDLE handle = {};                     // 8U
#endif

        // Required for dispatch load (and for XVK)
        VkInstance instance = {};               // 16U
        VkPhysicalDevice physicalDevice = {};   // 24U
        VkDevice device = {};                   // 32U
        void* pMapped = nullptr;                // 40U

        // 
        VkDeviceMemory memory = {};             // 48U
        VkDeviceSize offset = 0ull;             // 56U
        VkDeviceSize range = 0ull;              // 64U
        VkDeviceSize reqSize = 0ull;            // 72U

        // 
        VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;    // 80U
        VmaMemoryUsage memUsage = VMA_MEMORY_USAGE_GPU_ONLY;        // 88U

        // 
        vkh::VkPhysicalDeviceMemoryProperties memoryProperties = {};

        // 
        std::vector<uint32_t> queueFamilyIndices = {};
        vkt::uni_ptr<xvk::Instance> instanceDispatch = vkGlobal::instance;
        vkt::uni_ptr<xvk::Device> deviceDispatch = vkGlobal::device;

        // 
        uint32_t getMemoryType(const uint32_t& memoryTypeBitsRequirement, const vkh::VkMemoryPropertyFlags& requiredProperties = { .eDeviceLocal = 1 }) const {
            const uint32_t memoryCount = memoryProperties.memoryTypeCount;
            for (uint32_t memoryIndex = 0; memoryIndex < memoryCount; ++memoryIndex) {
                const uint32_t memoryTypeBits = (1 << memoryIndex);
                const bool isRequiredMemoryType = memoryTypeBitsRequirement & memoryTypeBits;
                const auto properties = VkMemoryPropertyFlags(memoryProperties.memoryTypes[memoryIndex].propertyFlags);
                const bool hasRequiredProperties = (properties & VkMemoryPropertyFlags(requiredProperties)) == VkMemoryPropertyFlags(requiredProperties);
                if (isRequiredMemoryType && hasRequiredProperties) return static_cast<uint32_t>(memoryIndex);
            };
            return 0u;
        }
    };

    // TODO: native image barrier in library
    struct ImageBarrierInfo {
        vkt::uni_ptr<xvk::Instance> instanceDispatch = vkGlobal::instance;
        vkt::uni_ptr<xvk::Device> deviceDispatch = vkGlobal::device;
        vkt::uni_arg<VkImage> image = {};
        vkt::uni_arg<VkImageLayout> targetLayout = VK_IMAGE_LAYOUT_GENERAL;
        vkt::uni_arg<VkImageLayout> originLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        vkt::uni_arg<vkh::VkImageSubresourceRange> subresourceRange = vkh::VkImageSubresourceRange{ {}, 0u, 1u, 0u, 1u };
    };

    //
    static inline auto imageBarrier(const vkt::uni_arg<VkCommandBuffer>& cmd = VkCommandBuffer{}, const vkt::uni_arg<ImageBarrierInfo>& info = ImageBarrierInfo{}) {
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

        // Put barrier on top
        const auto srcStageMask = vkh::VkPipelineStageFlags{ .eBottomOfPipe = 1 };
        const auto dstStageMask = vkh::VkPipelineStageFlags{ .eTopOfPipe = 1 };
        const auto dependencyFlags = vkh::VkDependencyFlags{};
        auto srcMask = vkh::VkAccessFlags{};
        auto dstMask = vkh::VkAccessFlags{};

        //
        typedef VkImageLayout il;
        typedef VkAccessFlagBits afb;

        // Is it me, or are these the same?
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
        imageMemoryBarriers.srcAccessMask = srcMask;
        imageMemoryBarriers.dstAccessMask = dstMask;

        //
        (info->deviceDispatch->vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, {},
                                                    0u, nullptr,
                                                    0u, nullptr,
                                                    1u, imageMemoryBarriers));

        //
        return result;
    };

};

#endif
