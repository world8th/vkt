#pragma once // #

#include <vkt3/core.hpp>
#include <vkh/enums.hpp>
#include <vkh/bitfields.hpp>

namespace vkh { // TODO: Coverage ALL of MOST and Common USING Vulkan Structures
                // TODO: WIP FULL C++20 SUPPORT
                // TODO: WIP FULL Vulkan-HPP COMPATIBILITY (but currently can be accessed by casting hacks, for PRO users only)

    // Structures should be packed accurately as Vulkan.H and Vulkan.HPP
    #pragma pack(push, 8) // BUT Vulkan Should PACK ONLY BY ONE BYTE

    //inline ::NAME* operator &() { return reinterpret_cast<::NAME*>((void*)this); } \
    //inline const ::NAME* operator &() const { return reinterpret_cast<const ::NAME*>((const void*)this); } \

    // 
    #define STRUCT_OPERATORS(NAME)\
        inline operator ::NAME&() { return reinterpret_cast<::NAME&>(*this); };\
        inline operator const ::NAME&() const { return reinterpret_cast<const ::NAME&>(*this); };\
        inline operator ::NAME*() { return reinterpret_cast<::NAME*>((void*)this); };\
        inline operator const ::NAME*() const { return reinterpret_cast<const ::NAME*>((const void*)this); };\
        inline operator NAME*() { return reinterpret_cast<NAME*>(this); };\
        inline operator const NAME*() const { return reinterpret_cast<const NAME*>(this); };\
        inline ::NAME& vk() { return reinterpret_cast<::NAME&>(*this); };\
        inline const ::NAME& vk() const { return reinterpret_cast<const ::NAME&>(*this); };\
        inline ::NAME* operator &() { return reinterpret_cast<::NAME*>((void*)this); }; \
        inline const ::NAME* operator &() const { return reinterpret_cast<const ::NAME*>((const void*)this); }; \
        inline NAME& operator =( vkt::uni_arg<::NAME> info ) { memcpy(this, &(*info), sizeof(NAME)); return *this; };\
        inline NAME& operator =( vkt::uni_arg<NAME> info ) { memcpy(this, &(*info), sizeof(NAME)); return *this; }; \
        inline NAME& also(const std::function<NAME&(NAME&)>& fn) { return fn(*this); };\
        inline static NAME create(const std::function<NAME&(NAME&)>& fn = {}) { auto data = NAME{}; return (fn ? fn(data) : data); };

#ifdef VULKAN_HPP
    #define VK_HPP_STRUCT_OPERATORS(NAME,VKNAME)\
        inline static VKNAME cpp() { const auto data = NAME{}; return reinterpret_cast<const VKNAME&>(data); };\
        inline operator VKNAME&() { return reinterpret_cast<VKNAME&>(*this); };\
        inline operator const VKNAME&() const { return reinterpret_cast<const VKNAME&>(*this); };\
        inline operator VKNAME*() { return reinterpret_cast<VKNAME*>(this); };\
        inline operator const VKNAME*() const { return reinterpret_cast<const VKNAME*>(this); };\
        inline VKNAME* operator->() { return reinterpret_cast<VKNAME*>(this); };\
        inline const VKNAME* operator->() const { return reinterpret_cast<const VKNAME*>(this); };\
        inline VKNAME& hpp() { return reinterpret_cast<VKNAME&>(*this); };\
        inline const VKNAME& hpp() const { return reinterpret_cast<const VKNAME&>(*this); };\
        inline NAME& operator =( vkt::uni_arg<VKNAME> info ) { memcpy(this, &(*info), sizeof(NAME)); return *this; };
#else
        #define VK_HPP_STRUCT_OPERATORS(NAME,VKNAME) // Not Vulkan HPP Support
#endif

    #define STRUCT_TYPE_COMPATIBLE(NAME,T)\
        inline NAME& operator=(const vkt::uni_arg<T>& V) { memcpy(this, &(*V), sizeof(T)); return *this; };\
        inline operator T&() { return reinterpret_cast<T&>(*this); };\
        inline operator const T&() const { return reinterpret_cast<const T&>(*this); };

    #define FLAGS(NAME) vkt::union_t<vkh::NAME##Flags, ::NAME##Flags, ::NAME##FlagBits>
    #define FLAGF(NAME,FIX) vkt::union_t<vkh::NAME##Flags##FIX, ::NAME##Flags##FIX, ::NAME##FlagBits##FIX>


    // GLM-Compatible
    typedef struct VkOffset3D {
        int32_t    x = 0;
        int32_t    y = 0;
        int32_t    z = 0;

        // 
        STRUCT_OPERATORS(VkOffset3D)
        VK_HPP_STRUCT_OPERATORS(VkOffset3D,vk::Offset3D)
        STRUCT_TYPE_COMPATIBLE(VkOffset3D,glm::ivec3)
    } VkOffset3D;

    // GLM-Compatible
    typedef struct VkOffset2D {
        int32_t    x = 0;
        int32_t    y = 0;

        // 
        STRUCT_OPERATORS(VkOffset2D)
        VK_HPP_STRUCT_OPERATORS(VkOffset2D,vk::Offset2D)
        STRUCT_TYPE_COMPATIBLE(VkOffset2D,glm::ivec2)
    } VkOffset2D;

    // GLM-Compatible
    typedef struct VkExtent2D {
        uint32_t    width  = 1u;
        uint32_t    height = 1u;

        // 
        STRUCT_OPERATORS(VkExtent2D)
        VK_HPP_STRUCT_OPERATORS(VkExtent2D,vk::Extent2D)
        STRUCT_TYPE_COMPATIBLE(VkExtent2D,glm::uvec2)
    } VkExtent2D;

    // GLM-Compatible
    typedef struct VkExtent3D {
        uint32_t    width  = 1u;
        uint32_t    height = 1u;
        uint32_t    depth  = 1u;

        // 
        STRUCT_OPERATORS(VkExtent3D)
        VK_HPP_STRUCT_OPERATORS(VkExtent3D,vk::Extent3D)
        STRUCT_TYPE_COMPATIBLE(VkExtent3D,glm::uvec3)
    } VkExtent3D;

    // GLM-Compatible
    typedef struct VkTransformMatrixKHR {
        float matrix[3][4] = {
            {1.f,0.f,0.f,0.f},
            {0.f,1.f,0.f,0.f},
            {0.f,0.f,1.f,0.f}
        };

        // 
        STRUCT_OPERATORS(VkTransformMatrixKHR)
        VK_HPP_STRUCT_OPERATORS(VkTransformMatrixKHR,vk::TransformMatrixKHR)
        STRUCT_TYPE_COMPATIBLE(VkTransformMatrixKHR,glm::mat3x4)
    } VkTransformMatrixKHR;

    // 
    typedef struct VkRect2D {
        VkOffset2D offset = { 0,0 };
        VkExtent2D extent = { 1,1 };

        // 
        STRUCT_OPERATORS(VkRect2D)
        VK_HPP_STRUCT_OPERATORS(VkRect2D,vk::Rect2D)
    } VkRect2D;

    // 
    typedef struct VkViewport {
        float    x          = -1.f;
        float    y          = -1.f;
        float    width      =  2.f;
        float    height     =  2.f;
        float    minDepth   =  0.f;
        float    maxDepth   =  1.f;

        // 
        STRUCT_OPERATORS(VkViewport)
        VK_HPP_STRUCT_OPERATORS(VkViewport,vk::Viewport)
    } VkViewport;

    // 
    typedef struct VkVertexInputBindingDescription {
        uint32_t             binding    = 0u;
        uint32_t             stride     = 16u;
        VkVertexInputRate    inputRate  = VK_VERTEX_INPUT_RATE_VERTEX;

        // 
        STRUCT_OPERATORS(VkVertexInputBindingDescription)
        VK_HPP_STRUCT_OPERATORS(VkVertexInputBindingDescription,vk::VertexInputBindingDescription)
    } VkVertexInputBindingDescription;

    // 
    typedef struct VkVertexInputAttributeDescription {
        uint32_t    location = 0u;
        uint32_t    binding  = 0u;
        VkFormat    format   = VK_FORMAT_R32G32B32_SFLOAT;
        uint32_t    offset   = 0u;

        // 
        STRUCT_OPERATORS(VkVertexInputAttributeDescription)
        VK_HPP_STRUCT_OPERATORS(VkVertexInputAttributeDescription,vk::VertexInputAttributeDescription)
    } VkVertexInputAttributeDescription;

    //
    typedef struct VkDescriptorPoolSize {
        VkDescriptorType    type            = VK_DESCRIPTOR_TYPE_SAMPLER;
        uint32_t            descriptorCount = 1u;

        // 
        STRUCT_OPERATORS(VkDescriptorPoolSize)
        VK_HPP_STRUCT_OPERATORS(VkDescriptorPoolSize,vk::DescriptorPoolSize)
    } VkDescriptorPoolSize;

    // 
    typedef struct VkDescriptorBufferInfo {
        VkBuffer        buffer  = VK_NULL_HANDLE;
        VkDeviceSize    offset  = 0u;
        VkDeviceSize    range   = VK_WHOLE_SIZE;

        // 
        STRUCT_OPERATORS(VkDescriptorBufferInfo)
        VK_HPP_STRUCT_OPERATORS(VkDescriptorBufferInfo,vk::DescriptorBufferInfo)
    } VkDescriptorBufferInfo;

    // 
    typedef struct VkStridedDeviceAddressRegionKHR {
        VkDeviceAddress    deviceAddress = 0ull;
        VkDeviceSize       stride        = 32ull;
        VkDeviceSize       size          = 32ull;

        // 
        STRUCT_OPERATORS(VkStridedDeviceAddressRegionKHR)
        VK_HPP_STRUCT_OPERATORS(VkStridedDeviceAddressRegionKHR,vk::StridedDeviceAddressRegionKHR)
    } VkStridedDeviceAddressRegionKHR;

    

    // 
    typedef struct VkDescriptorImageInfo {
        VkSampler        sampler     = VK_NULL_HANDLE;
        VkImageView      imageView   = VK_NULL_HANDLE;
        VkImageLayout    imageLayout = VK_IMAGE_LAYOUT_GENERAL;

        // 
        STRUCT_OPERATORS(VkDescriptorImageInfo)
        VK_HPP_STRUCT_OPERATORS(VkDescriptorImageInfo,vk::DescriptorImageInfo)
    } VkDescriptorImageInfo;

    // TODO: Buffer Copy v2 support
    typedef struct VkBufferCopy {
        VkDeviceSize  srcOffset = 0u;
        VkDeviceSize  dstOffset = 0u;
        VkDeviceSize  size      = 16u;

        // 
        STRUCT_OPERATORS(VkBufferCopy)
        VK_HPP_STRUCT_OPERATORS(VkBufferCopy, vk::BufferCopy)
    } VkBufferCopy;

    // 
    typedef struct VkWriteDescriptorSet {
        VkStructureType                     sType              = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        const void*                         pNext              = nullptr;
        VkDescriptorSet                     dstSet             = 0u;
        uint32_t                            dstBinding         = 0u;
        uint32_t                            dstArrayElement    = 0u;
        uint32_t                            descriptorCount    = 1u;
        VkDescriptorType                    descriptorType     = VK_DESCRIPTOR_TYPE_SAMPLER;
        const vkh::VkDescriptorImageInfo*   pImageInfo         = nullptr;
        const vkh::VkDescriptorBufferInfo*  pBufferInfo        = nullptr;
        const VkBufferView*                 pTexelBufferView   = nullptr;

        // 
        STRUCT_OPERATORS(VkWriteDescriptorSet)
        VK_HPP_STRUCT_OPERATORS(VkWriteDescriptorSet,vk::WriteDescriptorSet)
    } VkWriteDescriptorSet;

    // 
    inline constexpr auto VsDefaultDescriptorPoolFlags = VkDescriptorPoolCreateFlags{};

    // 
    typedef struct VkDescriptorPoolCreateInfo {
        VkStructureType                     sType            = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        const void*                         pNext            = nullptr;
        VkDescriptorPoolCreateFlags         flags            = {};
        uint32_t                            maxSets          = 256u;
        uint32_t                            poolSizeCount    = 0u;
        const vkh::VkDescriptorPoolSize*    pPoolSizes       = nullptr;

        // 
        STRUCT_OPERATORS(VkDescriptorPoolCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkDescriptorPoolCreateInfo,vk::DescriptorPoolCreateInfo)
    } VkDescriptorPoolCreateInfo;

    //
    inline constexpr auto VsDefaultBufferViewFlags = VkBufferViewCreateFlags{};
    inline constexpr auto VsDefaultBufferFlags = VkBufferCreateFlags{};
    inline constexpr auto VsDefaultBufferUsage = VkBufferUsageFlags{};

    // 
    typedef struct VkBufferViewCreateInfo {
        VkStructureType            sType    = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
        const void*                pNext    = nullptr;
        VkBufferViewCreateFlags    flags    = {};
        VkBuffer                   buffer   = VK_NULL_HANDLE;
        VkFormat                   format   = VK_FORMAT_R32G32B32A32_SFLOAT;
        VkDeviceSize               offset   = 0u;
        VkDeviceSize               range    = 16u;

        // 
        STRUCT_OPERATORS(VkBufferViewCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkBufferViewCreateInfo,vk::BufferViewCreateInfo)
    } VkBufferViewCreateInfo;

    //
    typedef struct VkBufferCreateInfo {
        VkStructureType     sType                   = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        const void*         pNext                   = nullptr;
        FLAGS(VkBufferCreate) flags = {};
        VkDeviceSize        size                    = 4u;
        FLAGS(VkBufferUsage) usage                  = VsDefaultBufferUsage;
        VkSharingMode       sharingMode             = VK_SHARING_MODE_EXCLUSIVE;
        uint32_t            queueFamilyIndexCount   = 0u;
        const uint32_t*     pQueueFamilyIndices     = nullptr;

        // 
        VkBufferCreateInfo& setQueueFamilyIndices(const std::vector<uint32_t>& V = {}) { pQueueFamilyIndices = V.data(); queueFamilyIndexCount = static_cast<uint32_t>(V.size()); return *this; };

        // 
        STRUCT_OPERATORS(VkBufferCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkBufferCreateInfo,vk::BufferCreateInfo)
    } VkBufferCreateInfo;

    //
    inline constexpr auto VsDefaultImageFlags = VkImageCreateFlags{};
    inline constexpr auto VsDefaultImageUsage = VkImageUsageFlags{};

    //
    typedef struct VkImageCreateInfo {
        VkStructureType          sType                  = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        const void*              pNext                  = nullptr;
        FLAGS(VkImageCreate)     flags                  = {};
        VkImageType              imageType              = VK_IMAGE_TYPE_2D;
        VkFormat                 format                 = VK_FORMAT_R8G8B8A8_UNORM;
        VkExtent3D               extent                 = {1u,1u,1u};
        uint32_t                 mipLevels              = 1u;
        uint32_t                 arrayLayers            = 1u;
        VkSampleCountFlagBits    samples                = VK_SAMPLE_COUNT_1_BIT;
        VkImageTiling            tiling                 = VK_IMAGE_TILING_OPTIMAL;
        FLAGS(VkImageUsage)      usage                  = VsDefaultImageUsage;
        VkSharingMode            sharingMode            = VK_SHARING_MODE_EXCLUSIVE;
        uint32_t                 queueFamilyIndexCount  = 0u;
        const uint32_t*          pQueueFamilyIndices    = nullptr;
        VkImageLayout            initialLayout          = VK_IMAGE_LAYOUT_UNDEFINED;

        // 
        VkImageCreateInfo& setQueueFamilyIndices(const std::vector<uint32_t>& V = {}) { pQueueFamilyIndices = V.data(); queueFamilyIndexCount = static_cast<uint32_t>(V.size()); return *this; };

        // 
        STRUCT_OPERATORS(VkImageCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkImageCreateInfo,vk::ImageCreateInfo)
    } VkImageCreateInfo;

    //
    inline constexpr auto VsDefaultSamplerFlags = VkSamplerCreateFlags{};

    // 
    typedef struct VkSamplerCreateInfo {
        VkStructureType         sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        const void*             pNext                   = nullptr;
        VkSamplerCreateFlags    flags                   = {}; // TODO: FLAGS
        VkFilter                magFilter               = VK_FILTER_NEAREST;
        VkFilter                minFilter               = VK_FILTER_NEAREST;
        VkSamplerMipmapMode     mipmapMode              = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        VkSamplerAddressMode    addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkSamplerAddressMode    addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkSamplerAddressMode    addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        float                   mipLodBias              = 0.f;
        VkBool32                anisotropyEnable        = false;
        float                   maxAnisotropy           = 0.f;
        VkBool32                compareEnable           = false;
        VkCompareOp             compareOp               = VK_COMPARE_OP_ALWAYS;
        float                   minLod                  = 0.f;
        float                   maxLod                  = 0.f;
        VkBorderColor           borderColor             = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        VkBool32                unnormalizedCoordinates = false;
        
        // 
        STRUCT_OPERATORS(VkSamplerCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkSamplerCreateInfo,vk::SamplerCreateInfo)
    } VkSamplerCreateInfo;

    //
    inline constexpr auto VsDefaultAspectMask = VkImageAspectFlags{ .eColor = 1 };

    // 
    typedef struct VkImageSubresourceRange {
        FLAGS(VkImageAspect) aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        uint32_t              baseMipLevel   = 0u;
        uint32_t              levelCount     = 1u;
        uint32_t              baseArrayLayer = 0u;
        uint32_t              layerCount     = 1u;
        
        // 
        STRUCT_OPERATORS(VkImageSubresourceRange)
        VK_HPP_STRUCT_OPERATORS(VkImageSubresourceRange,vk::ImageSubresourceRange)
    } VkImageSubresourceRange;

    // 
    typedef struct VkImageSubresourceLayers {
        FLAGS(VkImageAspect) aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        uint32_t              mipLevel       = 0u;
        uint32_t              baseArrayLayer = 0u;
        uint32_t              layerCount     = 1u;
        
        // 
        STRUCT_OPERATORS(VkImageSubresourceLayers)
        VK_HPP_STRUCT_OPERATORS(VkImageSubresourceLayers,vk::ImageSubresourceLayers)
    } VkImageSubresourceLayers;



    // 
    typedef struct VkComponentMapping {
        VkComponentSwizzle r = VK_COMPONENT_SWIZZLE_R;
        VkComponentSwizzle g = VK_COMPONENT_SWIZZLE_G;
        VkComponentSwizzle b = VK_COMPONENT_SWIZZLE_B;
        VkComponentSwizzle a = VK_COMPONENT_SWIZZLE_A;
        
        // 
        STRUCT_OPERATORS(VkComponentMapping)
        VK_HPP_STRUCT_OPERATORS(VkComponentMapping,vk::ComponentMapping)
    } VkComponentMapping;

    //
    inline constexpr auto VsDefaultComponents = VkComponentMapping{};
    inline constexpr auto VsDefaultCullMode = VkCullModeFlags{};

    //
    typedef struct VkImageViewCreateInfo {
        VkStructureType            sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        const void*                pNext            = nullptr;
        VkImageViewCreateFlags     flags            = 0u;
        VkImage                    image            = VK_NULL_HANDLE;
        VkImageViewType            viewType         = VK_IMAGE_VIEW_TYPE_2D;
        VkFormat                   format           = VK_FORMAT_R8G8B8A8_UNORM;
        VkComponentMapping         components       = VsDefaultComponents;
        VkImageSubresourceRange    subresourceRange = VkImageSubresourceRange{ .aspectMask = VsDefaultAspectMask };;

        // 
        STRUCT_OPERATORS(VkImageViewCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkImageViewCreateInfo,vk::ImageViewCreateInfo)
    } VkImageViewCreateInfo;

    // 
    typedef struct VkBufferImageCopy {
        VkDeviceSize                bufferOffset        = 0u;
        uint32_t                    bufferRowLength     = 1u;
        uint32_t                    bufferImageHeight   = 1u;
        VkImageSubresourceLayers    imageSubresource    = {};
        VkOffset3D                  imageOffset         = {};
        VkExtent3D                  imageExtent         = {};
        
        // 
        STRUCT_OPERATORS(VkBufferImageCopy)
        VK_HPP_STRUCT_OPERATORS(VkBufferImageCopy,vk::BufferImageCopy)
    } VkBufferImageCopy;

    // 
    typedef struct VkImageCopy {
        VkImageSubresourceLayers    srcSubresource  = {};
        VkOffset3D                  srcOffset       = {};
        VkImageSubresourceLayers    dstSubresource  = {};
        VkOffset3D                  dstOffset       = {};
        VkExtent3D                  extent          = {};
        
        // 
        STRUCT_OPERATORS(VkImageCopy)
        VK_HPP_STRUCT_OPERATORS(VkImageCopy,vk::ImageCopy)
    } VkImageCopy;

    // 
    typedef struct VkPipelineInputAssemblyStateCreateInfo {
        VkStructureType                                 sType                   = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        const void*                                     pNext                   = nullptr;
        VkPipelineInputAssemblyStateCreateFlags         flags                   = 0u; // TODO: FLAGS
        VkPrimitiveTopology                             topology                = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        VkBool32                                        primitiveRestartEnable  = false;

        // 
        STRUCT_OPERATORS(VkPipelineInputAssemblyStateCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkPipelineInputAssemblyStateCreateInfo,vk::PipelineInputAssemblyStateCreateInfo)
    } VkPipelineInputAssemblyStateCreateInfo;

    //
    typedef struct VkDescriptorSetAllocateInfo {
        VkStructureType                 sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        const void*                     pNext              = nullptr;
        VkDescriptorPool                descriptorPool     = VK_NULL_HANDLE;
        uint32_t                        descriptorSetCount = 1u;
        const VkDescriptorSetLayout*    pSetLayouts        = nullptr;

        // 
        VkDescriptorSetAllocateInfo& setSetLayouts(const std::vector<VkDescriptorSetLayout>& V = {}) { pSetLayouts = V.data(); descriptorSetCount = static_cast<uint32_t>(V.size()); return *this; };

        // 
        STRUCT_OPERATORS(VkDescriptorSetAllocateInfo)
        VK_HPP_STRUCT_OPERATORS(VkDescriptorSetAllocateInfo,vk::DescriptorSetAllocateInfo)
    } VkDescriptorSetAllocateInfo;

    // 
    typedef struct VkPipelineVertexInputStateCreateInfo {
        VkStructureType                                 sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        const void*                                     pNext                           = nullptr;
        VkPipelineVertexInputStateCreateFlags           flags                           = 0u;
        uint32_t                                        vertexBindingDescriptionCount   = 0u;
        const VkVertexInputBindingDescription*          pVertexBindingDescriptions      = nullptr;
        uint32_t                                        vertexAttributeDescriptionCount = 0u;
        const VkVertexInputAttributeDescription*        pVertexAttributeDescriptions    = nullptr;

        // 
        VkPipelineVertexInputStateCreateInfo& setVertexBindingDescriptions(const std::vector<VkVertexInputBindingDescription>& V = {}) { pVertexBindingDescriptions = V.data(); vertexBindingDescriptionCount = static_cast<uint32_t>(V.size()); return *this; };
        VkPipelineVertexInputStateCreateInfo& setVertexAttributeDescriptions(const std::vector<VkVertexInputAttributeDescription>& V = {}) { pVertexAttributeDescriptions = V.data(); vertexAttributeDescriptionCount = static_cast<uint32_t>(V.size()); return *this; };
        
        // 
        STRUCT_OPERATORS(VkPipelineVertexInputStateCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkPipelineVertexInputStateCreateInfo,vk::PipelineVertexInputStateCreateInfo)
    } VkPipelineVertexInputStateCreateInfo;

    // 
    typedef struct VkPipelineTessellationStateCreateInfo {
        VkStructureType                           sType                 = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        const void*                               pNext                 = nullptr;
        VkPipelineTessellationStateCreateFlags    flags                 = 0u; // TODO: FLAGS
        uint32_t                                  patchControlPoints    = 0u;

        // 
        STRUCT_OPERATORS(VkPipelineTessellationStateCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkPipelineTessellationStateCreateInfo,vk::PipelineTessellationStateCreateInfo)
    } VkPipelineTessellationStateCreateInfo;

    // 
    typedef struct VkPipelineViewportStateCreateInfo {
        VkStructureType                       sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        const void*                           pNext         = nullptr;
        VkPipelineViewportStateCreateFlags    flags         = 0u; // TODO: FLAGS
        uint32_t                              viewportCount = 1u;
        const ::VkViewport*                   pViewports    = nullptr;
        uint32_t                              scissorCount  = 1u;
        const ::VkRect2D*                     pScissors     = nullptr;

        // 
        VkPipelineViewportStateCreateInfo& setViewports(const std::vector<VkViewport>& V = {}) { pViewports = reinterpret_cast<::VkViewport*>((void*)V.data()); viewportCount = static_cast<uint32_t>(V.size()); return *this; };
        VkPipelineViewportStateCreateInfo& setScissors(const std::vector<VkRect2D>& V = {}) { pScissors = reinterpret_cast<::VkRect2D*>((void*)V.data()); scissorCount = static_cast<uint32_t>(V.size()); return *this; };

        // 
        STRUCT_OPERATORS(VkPipelineViewportStateCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkPipelineViewportStateCreateInfo,vk::PipelineViewportStateCreateInfo)
    } VkPipelineViewportStateCreateInfo;

    // 
    typedef struct VkPipelineRasterizationStateCreateInfo {
        VkStructureType                            sType                    = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        const void*                                pNext                    = nullptr;
        VkPipelineRasterizationStateCreateFlags    flags                    = 0u; // TODO: FLAGS
        VkBool32                                   depthClampEnable         = false;
        VkBool32                                   rasterizerDiscardEnable  = false;
        VkPolygonMode                              polygonMode              = VK_POLYGON_MODE_FILL;
        FLAGS(VkCullMode)                          cullMode                 = VsDefaultCullMode;
        VkFrontFace                                frontFace                = VK_FRONT_FACE_COUNTER_CLOCKWISE ;
        VkBool32                                   depthBiasEnable          = false;
        float                                      depthBiasConstantFactor  = 0.f;
        float                                      depthBiasClamp           = 0.f;
        float                                      depthBiasSlopeFactor     = 0.f;
        float                                      lineWidth                = 1.f;

        // 
        STRUCT_OPERATORS(VkPipelineRasterizationStateCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkPipelineRasterizationStateCreateInfo,vk::PipelineRasterizationStateCreateInfo)
    } VkPipelineRasterizationStateCreateInfo;

    //
    typedef struct VkPipelineMultisampleStateCreateInfo {
        VkStructureType                          sType                  = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        const void*                              pNext                  = nullptr;
        VkPipelineMultisampleStateCreateFlags    flags                  = 0u; // TODO: FLAGS
        VkSampleCountFlagBits                    rasterizationSamples   = VK_SAMPLE_COUNT_1_BIT;
        VkBool32                                 sampleShadingEnable    = false;
        float                                    minSampleShading       = 0.f;
        const VkSampleMask*                      pSampleMask            = nullptr;
        VkBool32                                 alphaToCoverageEnable  = false;
        VkBool32                                 alphaToOneEnable       = false;

        // 
        STRUCT_OPERATORS(VkPipelineMultisampleStateCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkPipelineMultisampleStateCreateInfo,vk::PipelineMultisampleStateCreateInfo)
    } VkPipelineMultisampleStateCreateInfo;

    // 
    typedef struct VkStencilOpState {
        VkStencilOp    failOp       = VK_STENCIL_OP_KEEP;
        VkStencilOp    passOp       = VK_STENCIL_OP_KEEP;
        VkStencilOp    depthFailOp  = VK_STENCIL_OP_KEEP;
        VkCompareOp    compareOp    = VK_COMPARE_OP_ALWAYS;
        uint32_t       compareMask  = 0u;
        uint32_t       writeMask    = 0u;
        uint32_t       reference    = 0u;

        // 
        STRUCT_OPERATORS(VkStencilOpState)
        VK_HPP_STRUCT_OPERATORS(VkStencilOpState,vk::StencilOpState)
    } VkStencilOpState;

    // 
    typedef struct VkPipelineDepthStencilStateCreateInfo {
        VkStructureType                           sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        const void*                               pNext                 = nullptr;
        VkPipelineDepthStencilStateCreateFlags    flags                 = 0u; // TODO: FLAGS
        VkBool32                                  depthTestEnable       = false;
        VkBool32                                  depthWriteEnable      = false;
        VkCompareOp                               depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
        VkBool32                                  depthBoundsTestEnable = false;
        VkBool32                                  stencilTestEnable     = false;
        VkStencilOpState                          front                 = {};
        VkStencilOpState                          back                  = {};
        float                                     minDepthBounds        = 0.f;
        float                                     maxDepthBounds        = 1.f;

        // 
        STRUCT_OPERATORS(VkPipelineDepthStencilStateCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkPipelineDepthStencilStateCreateInfo,vk::PipelineDepthStencilStateCreateInfo)
    } VkPipelineDepthStencilStateCreateInfo;

    //
    inline constexpr auto VsDefaultColorComponentFlags = VkColorComponentFlags{.eR = 1, .eG = 1, .eB = 1, .eA = 1};

    // 
    typedef struct VkPipelineColorBlendAttachmentState {
        VkBool32                 blendEnable         = false;
        VkBlendFactor            srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        VkBlendFactor            dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        VkBlendOp                colorBlendOp        = VK_BLEND_OP_ADD;
        VkBlendFactor            srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        VkBlendFactor            dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        VkBlendOp                alphaBlendOp        = VK_BLEND_OP_ADD;
        FLAGS(VkColorComponent) colorWriteMask       = VsDefaultColorComponentFlags;

        // 
        STRUCT_OPERATORS(VkPipelineColorBlendAttachmentState)
        VK_HPP_STRUCT_OPERATORS(VkPipelineColorBlendAttachmentState,vk::PipelineColorBlendAttachmentState)
    } VkPipelineColorBlendAttachmentState;

    // 
    typedef struct VkPipelineColorBlendStateCreateInfo {
        VkStructureType                               sType            = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        const void*                                   pNext            = nullptr;
        VkPipelineColorBlendStateCreateFlags          flags            = 0u; // TODO: FLAGS
        VkBool32                                      logicOpEnable    = false;
        VkLogicOp                                     logicOp          = VK_LOGIC_OP_SET;
        uint32_t                                      attachmentCount  = 0u;
        const VkPipelineColorBlendAttachmentState*    pAttachments     = nullptr;
        
        // TODO: optional GLM support
        glm::vec4                                     blendConstants   = {1.f,1.f,1.f,1.f};
        //float                                       blendConstants[4];

        // 
        VkPipelineColorBlendStateCreateInfo& setAttachments(const std::vector<VkPipelineColorBlendAttachmentState>& V = {}) { pAttachments = V.data(); attachmentCount = static_cast<uint32_t>(V.size()); return *this; };
        
        // 
        STRUCT_OPERATORS(VkPipelineColorBlendStateCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkPipelineColorBlendStateCreateInfo,vk::PipelineColorBlendStateCreateInfo)
    } VkPipelineColorBlendStateCreateInfo;

    // 
    typedef struct VkPipelineDynamicStateCreateInfo {
        VkStructureType                      sType              = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        const void*                          pNext              = nullptr;
        VkPipelineDynamicStateCreateFlags    flags              = 0u; // TODO: FLAGS
        uint32_t                             dynamicStateCount  = 0u;
        const VkDynamicState*                pDynamicStates     = nullptr;

        // 
        VkPipelineDynamicStateCreateInfo& setDynamicStates(const std::vector<VkDynamicState>& V = {}) { pDynamicStates = V.data(); dynamicStateCount = static_cast<uint32_t>(V.size()); return *this; };

        // 
        STRUCT_OPERATORS(VkPipelineDynamicStateCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkPipelineDynamicStateCreateInfo,vk::PipelineDynamicStateCreateInfo)
    } VkPipelineDynamicStateCreateInfo;

    //
    inline constexpr auto VsDefaultPipelineFlags = VkPipelineCreateFlags{};
    inline constexpr auto VsDefaultPipelineShaderStageFlags = VkPipelineShaderStageCreateFlags{};
    inline constexpr auto VsDefaultFramebufferFlags = VkFramebufferCreateFlags{};
    inline constexpr auto VsDefaultGeometryFlags = VkGeometryFlagsKHR{};
    inline constexpr auto VsDefaultBuildAccelerationStructureFlags = VkBuildAccelerationStructureFlagsKHR{};
    inline constexpr auto VsDefaultShaderModuleFlags = VkShaderModuleCreateFlags{};
    inline constexpr auto VsDefaultShaderStageFlags = VkShaderStageFlags{};

    //
    typedef struct VkPipelineShaderStageCreateInfo {
        VkStructureType                     sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        const void*                         pNext               = nullptr;
        FLAGS(VkPipelineShaderStageCreate)  flags               = VsDefaultPipelineShaderStageFlags; // TODO: FLAGS
        VkShaderStageFlagBits               stage               = VK_SHADER_STAGE_COMPUTE_BIT;
        VkShaderModule                      module              = VK_NULL_HANDLE;
        const char*                         pName               = "main";
        const VkSpecializationInfo*         pSpecializationInfo = nullptr; // TODO: NATIVE

        // 
        STRUCT_OPERATORS(VkPipelineShaderStageCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkPipelineShaderStageCreateInfo,vk::PipelineShaderStageCreateInfo)
    } VkPipelineShaderStageCreateInfo;

    //
    typedef struct VkGraphicsPipelineCreateInfo {
        VkStructureType                                  sType                  = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        const void*                                      pNext                  = nullptr;
        FLAGS(VkPipelineCreate)                          flags                  = VsDefaultPipelineFlags; // TODO: FLAGS
        uint32_t                                         stageCount             = 0u;
        const ::VkPipelineShaderStageCreateInfo*           pStages                = nullptr;
        const ::VkPipelineVertexInputStateCreateInfo*      pVertexInputState      = nullptr;
        const ::VkPipelineInputAssemblyStateCreateInfo*    pInputAssemblyState    = nullptr;
        const ::VkPipelineTessellationStateCreateInfo*     pTessellationState     = nullptr;
        const ::VkPipelineViewportStateCreateInfo*         pViewportState         = nullptr;
        const ::VkPipelineRasterizationStateCreateInfo*    pRasterizationState    = nullptr;
        const ::VkPipelineMultisampleStateCreateInfo*      pMultisampleState      = nullptr;
        const ::VkPipelineDepthStencilStateCreateInfo*     pDepthStencilState     = nullptr;
        const ::VkPipelineColorBlendStateCreateInfo*       pColorBlendState       = nullptr;
        const ::VkPipelineDynamicStateCreateInfo*          pDynamicState          = nullptr;
        VkPipelineLayout                                 layout                 = VK_NULL_HANDLE;
        VkRenderPass                                     renderPass             = VK_NULL_HANDLE;
        uint32_t                                         subpass                = 0u;
        VkPipeline                                       basePipelineHandle     = VK_NULL_HANDLE;
        int32_t                                          basePipelineIndex      = 0;

        // 
        VkGraphicsPipelineCreateInfo& setStages(const std::vector<VkPipelineShaderStageCreateInfo>& V = {}) { pStages = reinterpret_cast<::VkPipelineShaderStageCreateInfo*>((void*)V.data()); stageCount = static_cast<uint32_t>(V.size()); return *this; };

        // 
        STRUCT_OPERATORS(VkGraphicsPipelineCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkGraphicsPipelineCreateInfo,vk::GraphicsPipelineCreateInfo)
    } VkGraphicsPipelineCreateInfo;

    // Vulkan 1.3
    typedef struct VkPhysicalDeviceSubgroupSizeControlPropertiesEXT {
        VkStructureType            sType                        = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES_EXT;
        void*                      pNext                        = nullptr;
        uint32_t                   minSubgroupSize              = 0u;//16u;
        uint32_t                   maxSubgroupSize              = 0u;//64u;
        uint32_t                   maxComputeWorkgroupSubgroups = 0u;
        FLAGS(VkShaderStage)       requiredSubgroupSizeStages   = VsDefaultShaderStageFlags;

        // 
        STRUCT_OPERATORS(VkPhysicalDeviceSubgroupSizeControlPropertiesEXT)
        VK_HPP_STRUCT_OPERATORS(VkPhysicalDeviceSubgroupSizeControlPropertiesEXT,vk::PhysicalDeviceSubgroupSizeControlPropertiesEXT)
    } VkPhysicalDeviceSubgroupSizeControlPropertiesEXT;

    // Vulkan 1.3
    typedef struct VkPipelineShaderStageRequiredSubgroupSizeCreateInfoEXT {
        VkStructureType         sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO_EXT;
        void*                   pNext = nullptr;
        uint32_t                requiredSubgroupSize = 32u; // RDNA 2

        // 
        STRUCT_OPERATORS(VkPipelineShaderStageRequiredSubgroupSizeCreateInfoEXT)
        VK_HPP_STRUCT_OPERATORS(VkPipelineShaderStageRequiredSubgroupSizeCreateInfoEXT,vk::PipelineShaderStageRequiredSubgroupSizeCreateInfoEXT)
    } VkPipelineShaderStageRequiredSubgroupSizeCreateInfoEXT;

    //
    typedef struct VkComputePipelineCreateInfo {
        VkStructureType                    sType                = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        const void*                        pNext                = nullptr;
        FLAGS(VkPipelineCreate)            flags                = VsDefaultPipelineFlags; // TODO: FLAGS
        VkPipelineShaderStageCreateInfo    stage                = {};
        VkPipelineLayout                   layout               = VK_NULL_HANDLE;
        VkPipeline                         basePipelineHandle   = VK_NULL_HANDLE;
        int32_t                            basePipelineIndex    = 0;

        // 
        STRUCT_OPERATORS(VkComputePipelineCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkComputePipelineCreateInfo,vk::ComputePipelineCreateInfo)
    } VkComputePipelineCreateInfo;


    //
    typedef struct VkDescriptorSetLayoutBinding {
        uint32_t              binding               = 0u;
        VkDescriptorType      descriptorType        = VK_DESCRIPTOR_TYPE_SAMPLER;
        uint32_t              descriptorCount       = 1u;
        FLAGS(VkShaderStage)  stageFlags            = VsDefaultShaderStageFlags;
        const VkSampler*      pImmutableSamplers    = nullptr;

        // 
        STRUCT_OPERATORS(VkDescriptorSetLayoutBinding)
        VK_HPP_STRUCT_OPERATORS(VkDescriptorSetLayoutBinding,vk::DescriptorSetLayoutBinding)
    } VkDescriptorSetLayoutBinding;

    // 
    typedef struct VkPushConstantRange {
        FLAGS(VkShaderStage)  stageFlags = VsDefaultShaderStageFlags;
        uint32_t              offset     = 0u;
        uint32_t              size       = 16u;

        // 
        STRUCT_OPERATORS(VkPushConstantRange)
        VK_HPP_STRUCT_OPERATORS(VkPushConstantRange, vk::PushConstantRange)
    } VkPushConstantRange;

    // 
    inline constexpr auto VsDefaultDescriptorSetLayoutFlags = VkDescriptorSetLayoutCreateFlags{};
    inline constexpr auto VsDefaultPipelineLayoutFlags = VkPipelineLayoutCreateFlags{};

    // 
    typedef struct VkPipelineLayoutCreateInfo {
        VkStructureType                 sType                   = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        const void*                     pNext                   = nullptr;
        VkPipelineLayoutCreateFlags     flags                   = VsDefaultPipelineLayoutFlags;
        uint32_t                        setLayoutCount          = 0u;
        const VkDescriptorSetLayout*    pSetLayouts             = nullptr;
        uint32_t                        pushConstantRangeCount  = 0u;
        const VkPushConstantRange*      pPushConstantRanges     = nullptr; // TODO: NATIVE
        VkPipelineLayoutCreateInfo& setSetLayouts(const std::vector<VkDescriptorSetLayout>& V = {}) { pSetLayouts = V.data(); setLayoutCount = static_cast<uint32_t>(V.size()); return *this; };
        VkPipelineLayoutCreateInfo& setPushConstantRanges(const std::vector<VkPushConstantRange>& V = {}) { pPushConstantRanges = V.data(); pushConstantRangeCount = static_cast<uint32_t>(V.size()); return *this; };

        // 
        STRUCT_OPERATORS(VkPipelineLayoutCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkPipelineLayoutCreateInfo,vk::PipelineLayoutCreateInfo)
    } VkPipelineLayoutCreateInfo;

    // TODO: Correct Type of Flags Pointer
    typedef struct VkDescriptorSetLayoutBindingFlagsCreateInfo {
        VkStructureType                       sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
        const void*                           pNext         = nullptr;
        uint32_t                              bindingCount  = 0u;
        const ::VkDescriptorBindingFlags*     pBindingFlags = nullptr;

        // 
        VkDescriptorSetLayoutBindingFlagsCreateInfo& setBindingFlags(const std::vector<::VkDescriptorBindingFlags>& V = {}) { pBindingFlags = V.data(); bindingCount = static_cast<uint32_t>(V.size()); return *this; };

        // 
        STRUCT_OPERATORS(VkDescriptorSetLayoutBindingFlagsCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkDescriptorSetLayoutBindingFlagsCreateInfo,vk::DescriptorSetLayoutBindingFlagsCreateInfo)
    } VkDescriptorSetLayoutBindingFlagsCreateInfo;

    // 
    typedef struct VkDescriptorUpdateTemplateEntry {
        uint32_t            dstBinding      = 0u;
        uint32_t            dstArrayElement = 0u;
        uint32_t            descriptorCount = 1u;
        VkDescriptorType    descriptorType  = VK_DESCRIPTOR_TYPE_SAMPLER;
        size_t              offset          = 0u;
        size_t              stride          = 8u;

        // 
        STRUCT_OPERATORS(VkDescriptorUpdateTemplateEntry)
        VK_HPP_STRUCT_OPERATORS(VkDescriptorUpdateTemplateEntry,vk::DescriptorUpdateTemplateEntry)
    } VkDescriptorUpdateTemplateEntry;

    // 
    typedef struct VkDescriptorUpdateTemplateCreateInfo {
        VkStructureType                           sType                         = VK_STRUCTURE_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_CREATE_INFO;
        const void*                               pNext                         = nullptr;
        VkDescriptorUpdateTemplateCreateFlags     flags                         = 0u; // TODO: FLAGS
        uint32_t                                  descriptorUpdateEntryCount    = 0u;
        const VkDescriptorUpdateTemplateEntry*    pDescriptorUpdateEntries      = nullptr;
        VkDescriptorUpdateTemplateType            templateType                  = VK_DESCRIPTOR_UPDATE_TEMPLATE_TYPE_DESCRIPTOR_SET;
        VkDescriptorSetLayout                     descriptorSetLayout           = VK_NULL_HANDLE;
        VkPipelineBindPoint                       pipelineBindPoint             = VK_PIPELINE_BIND_POINT_COMPUTE;
        VkPipelineLayout                          pipelineLayout                = VK_NULL_HANDLE;
        uint32_t                                  set                           = 0u;

        // 
        VkDescriptorUpdateTemplateCreateInfo& setDescriptorUpdateEntries(const std::vector<VkDescriptorUpdateTemplateEntry>& V = {}) { pDescriptorUpdateEntries = V.data(); descriptorUpdateEntryCount = static_cast<uint32_t>(V.size()); return *this; };

        // 
        STRUCT_OPERATORS(VkDescriptorUpdateTemplateCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkDescriptorUpdateTemplateCreateInfo,vk::DescriptorUpdateTemplateCreateInfo)
    } VkDescriptorUpdateTemplateCreateInfo;

    // 
    typedef struct VkDescriptorSetLayoutCreateInfo {
        VkStructureType                        sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        const void*                            pNext        = nullptr;
        FLAGS(VkDescriptorSetLayoutCreate)     flags        = VsDefaultDescriptorSetLayoutFlags;
        uint32_t                               bindingCount = 0u;
        const VkDescriptorSetLayoutBinding*    pBindings    = nullptr;

        // 
        VkDescriptorSetLayoutCreateInfo& setBindings(const std::vector<VkDescriptorSetLayoutBinding>& V = {}) { pBindings = V.data(); bindingCount = static_cast<uint32_t>(V.size()); return *this; };

        // 
        STRUCT_OPERATORS(VkDescriptorSetLayoutCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkDescriptorSetLayoutCreateInfo,vk::DescriptorSetLayoutCreateInfo)
    } VkDescriptorSetLayoutCreateInfo;

    // 
    typedef struct VkAttachmentDescription {
        VkAttachmentDescriptionFlags    flags           = 0u; // TODO: FLAGS
        VkFormat                        format          = VK_FORMAT_R8G8B8A8_UNORM;
        VkSampleCountFlagBits           samples         = VK_SAMPLE_COUNT_1_BIT;
        VkAttachmentLoadOp              loadOp          = VK_ATTACHMENT_LOAD_OP_CLEAR;
        VkAttachmentStoreOp             storeOp         = VK_ATTACHMENT_STORE_OP_STORE;
        VkAttachmentLoadOp              stencilLoadOp   = VK_ATTACHMENT_LOAD_OP_CLEAR;
        VkAttachmentStoreOp             stencilStoreOp  = VK_ATTACHMENT_STORE_OP_STORE;
        VkImageLayout                   initialLayout   = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageLayout                   finalLayout     = VK_IMAGE_LAYOUT_UNDEFINED;

        // 
        STRUCT_OPERATORS(VkAttachmentDescription)
        VK_HPP_STRUCT_OPERATORS(VkAttachmentDescription,vk::AttachmentDescription)
    } VkAttachmentDescription;

    // 
    typedef struct VkSubpassDescription {
        VkSubpassDescriptionFlags       flags                   = 0u; // TODO: FLAGS
        VkPipelineBindPoint             pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        uint32_t                        inputAttachmentCount    = 0u;
        const VkAttachmentReference*    pInputAttachments       = nullptr; // TODO: NATIVE
        uint32_t                        colorAttachmentCount    = 0u;
        const VkAttachmentReference*    pColorAttachments       = nullptr; // TODO: NATIVE
        const VkAttachmentReference*    pResolveAttachments     = nullptr; // TODO: NATIVE
        const VkAttachmentReference*    pDepthStencilAttachment = nullptr; // TODO: NATIVE
        uint32_t                        preserveAttachmentCount = 0u;
        const uint32_t*                 pPreserveAttachments    = nullptr;

        // 
        VkSubpassDescription& setColorAttachments(const std::vector<VkAttachmentReference>& V = {}) { pColorAttachments = V.data(); colorAttachmentCount = static_cast<uint32_t>(V.size()); return *this; };
        VkSubpassDescription& setInputAttachments(const std::vector<VkAttachmentReference>& V = {}) { pInputAttachments = V.data(); inputAttachmentCount = static_cast<uint32_t>(V.size()); return *this; };
        VkSubpassDescription& setPreserveAttachments(const std::vector<uint32_t>& V = {}) { pPreserveAttachments = V.data(); preserveAttachmentCount = static_cast<uint32_t>(V.size()); return *this; };

        // 
        STRUCT_OPERATORS(VkSubpassDescription)
        VK_HPP_STRUCT_OPERATORS(VkSubpassDescription,vk::SubpassDescription)
    } VkSubpassDescription;

    // 
    inline constexpr auto VsDefaultAccessFlags = VkAccessFlags{};
    inline constexpr auto VsDefaultDependencyFlags = VkDependencyFlags{};
    inline constexpr auto VsDefaultPipelineStageFlags = VkPipelineStageFlags{};

    // 
    typedef struct VkSubpassDependency {
        uint32_t               srcSubpass      = 0u;
        uint32_t               dstSubpass      = 0u;
        FLAGS(VkPipelineStage) srcStageMask    = VsDefaultPipelineStageFlags;
        FLAGS(VkPipelineStage) dstStageMask    = VsDefaultPipelineStageFlags;
        FLAGS(VkAccess)        srcAccessMask   = VsDefaultAccessFlags;
        FLAGS(VkAccess)        dstAccessMask   = VsDefaultAccessFlags;
        FLAGS(VkDependency)    dependencyFlags = VsDefaultDependencyFlags;

        // 
        STRUCT_OPERATORS(VkSubpassDependency)
        VK_HPP_STRUCT_OPERATORS(VkSubpassDependency,vk::SubpassDependency)
    } VkSubpassDependency;

    // 
    typedef struct VkRenderPassCreateInfo {
        VkStructureType                   sType             = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        const void*                       pNext             = nullptr;
        VkRenderPassCreateFlags           flags             = 0u; // TODO: FLAGS
        uint32_t                          attachmentCount   = 0u;
        const VkAttachmentDescription*    pAttachments      = nullptr;
        uint32_t                          subpassCount      = 0u;
        const VkSubpassDescription*       pSubpasses        = nullptr;
        uint32_t                          dependencyCount   = 0u;
        const VkSubpassDependency*        pDependencies     = nullptr;

        // 
        VkRenderPassCreateInfo& setAttachments(const std::vector<VkAttachmentDescription>& V = {}) { pAttachments = V.data(); attachmentCount = static_cast<uint32_t>(V.size()); return *this; };
        VkRenderPassCreateInfo& setSubpasses(const std::vector<VkSubpassDescription>& V = {}) { pSubpasses = V.data(); subpassCount = static_cast<uint32_t>(V.size()); return *this; };
        VkRenderPassCreateInfo& setDependencies(const std::vector<VkSubpassDependency>& V = {}) { pDependencies = V.data(); dependencyCount = static_cast<uint32_t>(V.size()); return *this; };

        // 
        STRUCT_OPERATORS(VkRenderPassCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkRenderPassCreateInfo,vk::RenderPassCreateInfo)
    } VkRenderPassCreateInfo;

    // 
    typedef struct VkFramebufferCreateInfo {
        VkStructureType             sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        const void*                 pNext           = nullptr;
        VkFramebufferCreateFlags    flags           = VsDefaultFramebufferFlags; // TODO: FLAGS
        VkRenderPass                renderPass      = VK_NULL_HANDLE;
        uint32_t                    attachmentCount = 0u;
        const VkImageView*          pAttachments    = nullptr;
        uint32_t                    width           = 1u;
        uint32_t                    height          = 1u;
        uint32_t                    layers          = 1u;

        // 
        VkFramebufferCreateInfo& setAttachments(const std::vector<VkImageView>& V = {}) { pAttachments = V.data(); attachmentCount = static_cast<uint32_t>(V.size()); return *this; };

        // 
        STRUCT_OPERATORS(VkFramebufferCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkFramebufferCreateInfo,vk::FramebufferCreateInfo)
    } VkFramebufferCreateInfo;

    // 
    typedef struct VkMemoryBarrier {
        VkStructureType     sType       = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        const void*         pNext       = nullptr;
        FLAGS(VkAccess) srcAccessMask   = VsDefaultAccessFlags;
        FLAGS(VkAccess) dstAccessMask   = VsDefaultAccessFlags;

        // 
        STRUCT_OPERATORS(VkMemoryBarrier)
        VK_HPP_STRUCT_OPERATORS(VkMemoryBarrier, vk::MemoryBarrier)
    } VkMemoryBarrier;

    // 
    inline constexpr auto VsDefaultMemoryHeapFlags = VkMemoryHeapFlags{};
    inline constexpr auto VsDefaultMemoryPropertyFlags = VkMemoryPropertyFlags{};
    inline constexpr auto VsDefaultMemoryAllocateFlags = VkMemoryAllocateFlags{};

    //
    typedef struct VkMemoryType {
        FLAGS(VkMemoryProperty) propertyFlags   = VsDefaultMemoryPropertyFlags;
        uint32_t                    heapIndex   = 0u;

        // 
        STRUCT_OPERATORS(VkMemoryType)
        VK_HPP_STRUCT_OPERATORS(VkMemoryType, vk::MemoryType)
    } VkMemoryType;

    // 
    typedef struct VkMemoryHeap {
        VkDeviceSize        size  = 0u;
        FLAGS(VkMemoryHeap) flags = VsDefaultMemoryHeapFlags;

        // 
        STRUCT_OPERATORS(VkMemoryHeap)
        VK_HPP_STRUCT_OPERATORS(VkMemoryHeap, vk::MemoryHeap)
    } VkMemoryHeap;

    // 
    typedef struct VkPhysicalDeviceMemoryProperties {
        uint32_t     memoryTypeCount                  = 0u;
        VkMemoryType memoryTypes[VK_MAX_MEMORY_TYPES] = {};
        uint32_t     memoryHeapCount                  = 0u;
        VkMemoryHeap memoryHeaps[VK_MAX_MEMORY_HEAPS] = {};

        // 
        STRUCT_OPERATORS(VkPhysicalDeviceMemoryProperties)
        VK_HPP_STRUCT_OPERATORS(VkPhysicalDeviceMemoryProperties, vk::PhysicalDeviceMemoryProperties)
    } VkPhysicalDeviceMemoryProperties;

    // 
    typedef struct VkBufferDeviceAddressInfo {
        VkStructureType    sType  = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        const void*        pNext  = nullptr;
        VkBuffer           buffer = {};

        // 
        operator VkBuffer&() { return buffer; };
        operator const VkBuffer&() const { return buffer; };

        // 
        void operator=(const VkBuffer& buffer) { this->buffer = buffer; };

        // 
        STRUCT_OPERATORS(VkBufferDeviceAddressInfo)
        VK_HPP_STRUCT_OPERATORS(VkBufferDeviceAddressInfo, vk::BufferDeviceAddressInfo)
    } VkBufferDeviceAddressInfo;

    // 
    typedef struct VkAccelerationStructureDeviceAddressInfoKHR {
        VkStructureType               sType                 = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
        const void*                   pNext                 = nullptr;
        VkAccelerationStructureKHR    accelerationStructure = {};

        // 
        operator VkAccelerationStructureKHR&() { return accelerationStructure; };
        operator const VkAccelerationStructureKHR&() const { return accelerationStructure; };

        // 
        void operator=(const VkAccelerationStructureKHR& accelerationStructure) { this->accelerationStructure = accelerationStructure; };

        // 
        STRUCT_OPERATORS(VkAccelerationStructureDeviceAddressInfoKHR)
        VK_HPP_STRUCT_OPERATORS(VkAccelerationStructureDeviceAddressInfoKHR, vk::AccelerationStructureDeviceAddressInfoKHR)
    } VkAccelerationStructureDeviceAddressInfoKHR;

    // 
    typedef union VkDeviceOrHostAddressKHR {
        VkDeviceAddress    deviceAddress;
        void*              hostAddress;

        // 
        void operator=(const VkDeviceAddress& deviceAddress) { this->deviceAddress = deviceAddress; };
        //void operator=(void* const& hostAddress) { this->hostAddress = hostAddress; };

        // 
        operator VkDeviceAddress&() { return deviceAddress; };
        operator const VkDeviceAddress&() const { return deviceAddress; };

        // 
        operator void*&() { return hostAddress; };
        operator void* const&() const { return hostAddress; };

        // 
        STRUCT_OPERATORS(VkDeviceOrHostAddressKHR)
        VK_HPP_STRUCT_OPERATORS(VkDeviceOrHostAddressKHR, vk::DeviceOrHostAddressKHR)
    } VkDeviceOrHostAddressKHR;

    // 
    typedef union VkDeviceOrHostAddressConstKHR  {
        VkDeviceAddress    deviceAddress;
        const void*        hostAddress;

        // 
        VkDeviceOrHostAddressConstKHR& operator=(const VkDeviceAddress& deviceAddress) { this->deviceAddress = deviceAddress; return *this; };
        //VkDeviceOrHostAddressConstKHR& operator=(const void* const& hostAddress) { this->hostAddress = hostAddress; return *this; };

        // 
        operator VkDeviceAddress&() { return deviceAddress; };
        operator const VkDeviceAddress&() const { return deviceAddress; };

        // 
        operator void const*& () { return hostAddress; };
        operator const void* const&() const { return hostAddress; };

        // 
        STRUCT_OPERATORS(VkDeviceOrHostAddressConstKHR)
        VK_HPP_STRUCT_OPERATORS(VkDeviceOrHostAddressConstKHR, vk::DeviceOrHostAddressConstKHR)
    } VkDeviceOrHostAddressConstKHR;

    // VK_KHR_pipeline_library
    typedef struct VkPipelineLibraryCreateInfoKHR {
        VkStructureType      sType          = VK_STRUCTURE_TYPE_PIPELINE_LIBRARY_CREATE_INFO_KHR;
        const void*          pNext          = nullptr;
        uint32_t             libraryCount   = 0u;
        const VkPipeline*    pLibraries     = nullptr;

        // 
        VkPipelineLibraryCreateInfoKHR& setLibraries(const std::vector<VkPipeline>& libraries = {}){
            this->libraryCount = static_cast<uint32_t>(libraries.size());
            this->pLibraries = libraries.data();
        };

        // Beta
        STRUCT_OPERATORS(VkPipelineLibraryCreateInfoKHR)
        VK_HPP_STRUCT_OPERATORS(VkPipelineLibraryCreateInfoKHR, vk::PipelineLibraryCreateInfoKHR)
    } VkPipelineLibraryCreateInfoKHR;

    // 
    typedef struct VkAccelerationStructureGeometryTrianglesDataKHR {
        VkStructureType                  sType          = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
        const void*                      pNext          = nullptr;
        VkFormat                         vertexFormat   = VK_FORMAT_R32G32B32_SFLOAT;
        VkDeviceOrHostAddressConstKHR    vertexData     = {};
        VkDeviceSize                     vertexStride   = 16u;
        uint32_t                         maxVertex      = 0u;
        VkIndexType                      indexType      = VK_INDEX_TYPE_NONE_KHR;
        VkDeviceOrHostAddressConstKHR    indexData      = {};
        VkDeviceOrHostAddressConstKHR    transformData  = {};

        // 
        STRUCT_OPERATORS(VkAccelerationStructureGeometryTrianglesDataKHR)
        VK_HPP_STRUCT_OPERATORS(VkAccelerationStructureGeometryTrianglesDataKHR, vk::AccelerationStructureGeometryTrianglesDataKHR)
    } VkAccelerationStructureGeometryTrianglesDataKHR;

    // 
    typedef struct VkAccelerationStructureGeometryInstancesDataKHR {
        VkStructureType                  sType            = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
        const void*                      pNext            = nullptr;
        VkBool32                         arrayOfPointers  = false;
        VkDeviceOrHostAddressConstKHR    data             = {};

        // 
        STRUCT_OPERATORS(VkAccelerationStructureGeometryInstancesDataKHR)
        VK_HPP_STRUCT_OPERATORS(VkAccelerationStructureGeometryInstancesDataKHR, vk::AccelerationStructureGeometryInstancesDataKHR)
    } VkAccelerationStructureGeometryInstancesDataKHR;

    // 
    typedef struct VkAccelerationStructureGeometryAabbsDataKHR {
        VkStructureType                  sType   = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR;
        const void*                      pNext   = nullptr;
        VkDeviceOrHostAddressConstKHR    data    = {};
        VkDeviceSize                     stride  = 24u;

        // 
        STRUCT_OPERATORS(VkAccelerationStructureGeometryAabbsDataKHR)
        VK_HPP_STRUCT_OPERATORS(VkAccelerationStructureGeometryAabbsDataKHR, vk::AccelerationStructureGeometryAabbsDataKHR)
    } VkAccelerationStructureGeometryAabbsDataKHR;

    // 
    typedef union VkAccelerationStructureGeometryDataKHR {
        // now indefined (GCC)
        ::VkAccelerationStructureGeometryTrianglesDataKHR  triangles;
        ::VkAccelerationStructureGeometryInstancesDataKHR  instances;
        ::VkAccelerationStructureGeometryAabbsDataKHR      aabbs    ;

        // 
        VkAccelerationStructureGeometryDataKHR& operator=(const VkAccelerationStructureGeometryTrianglesDataKHR& triangles) { this->triangles = triangles; return *this; };
        VkAccelerationStructureGeometryDataKHR& operator=(const VkAccelerationStructureGeometryInstancesDataKHR& instances) { this->instances = instances; return *this; };
        VkAccelerationStructureGeometryDataKHR& operator=(const VkAccelerationStructureGeometryAabbsDataKHR& aabbs) { this->aabbs = aabbs; return *this; };

        // 
        operator VkAccelerationStructureGeometryTrianglesDataKHR& () { return reinterpret_cast<VkAccelerationStructureGeometryTrianglesDataKHR&>(triangles); };
        operator VkAccelerationStructureGeometryInstancesDataKHR& () { return reinterpret_cast<VkAccelerationStructureGeometryInstancesDataKHR&>(instances); };
        operator VkAccelerationStructureGeometryAabbsDataKHR& () { return reinterpret_cast<VkAccelerationStructureGeometryAabbsDataKHR&>(aabbs); };

        // 
        operator const VkAccelerationStructureGeometryTrianglesDataKHR& () const { return reinterpret_cast<const VkAccelerationStructureGeometryTrianglesDataKHR&>(triangles); };
        operator const VkAccelerationStructureGeometryInstancesDataKHR& () const { return reinterpret_cast<const VkAccelerationStructureGeometryInstancesDataKHR&>(instances); };
        operator const VkAccelerationStructureGeometryAabbsDataKHR& () const { return reinterpret_cast<const VkAccelerationStructureGeometryAabbsDataKHR&>(aabbs); };

        // 
        STRUCT_OPERATORS(VkAccelerationStructureGeometryDataKHR)
        VK_HPP_STRUCT_OPERATORS(VkAccelerationStructureGeometryDataKHR, vk::AccelerationStructureGeometryDataKHR)
    } VkAccelerationStructureGeometryDataKHR;

    //
    typedef struct VkAccelerationStructureGeometryKHR {
        VkStructureType                           sType         = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        const void*                               pNext         = nullptr;
        VkGeometryTypeKHR                         geometryType  = {};
        VkAccelerationStructureGeometryDataKHR    geometry      = VkAccelerationStructureGeometryDataKHR{ .triangles = {} };
        FLAGF(VkGeometry, KHR)                    flags         = VsDefaultGeometryFlags;

        // TODO: type depending of `VkAccelerationStructureGeometryDataKHR`
        VkAccelerationStructureGeometryKHR& operator=(const VkAccelerationStructureGeometryTrianglesDataKHR& triangles) { this->geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR; this->geometry.triangles = triangles; return *this; };
        VkAccelerationStructureGeometryKHR& operator=(const VkAccelerationStructureGeometryInstancesDataKHR& instances) { this->geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR; this->geometry.instances = instances; return *this; };
        VkAccelerationStructureGeometryKHR& operator=(const VkAccelerationStructureGeometryAabbsDataKHR& aabbs) { this->geometryType = VK_GEOMETRY_TYPE_AABBS_KHR; this->geometry.aabbs = aabbs; return *this; };

        // 
        operator VkAccelerationStructureGeometryDataKHR& () { return geometry; };
        operator VkAccelerationStructureGeometryTrianglesDataKHR& () { return geometry; };
        operator VkAccelerationStructureGeometryInstancesDataKHR& () { return geometry; };
        operator VkAccelerationStructureGeometryAabbsDataKHR& () { return geometry; };

        // 
        operator const VkAccelerationStructureGeometryDataKHR& () const { return geometry; };
        operator const VkAccelerationStructureGeometryTrianglesDataKHR& () const { return geometry; };
        operator const VkAccelerationStructureGeometryInstancesDataKHR& () const { return geometry; };
        operator const VkAccelerationStructureGeometryAabbsDataKHR& () const { return geometry; };

        // 
        STRUCT_OPERATORS(VkAccelerationStructureGeometryKHR)
        VK_HPP_STRUCT_OPERATORS(VkAccelerationStructureGeometryKHR, vk::AccelerationStructureGeometryKHR)
    } VkAccelerationStructureGeometryKHR;

    // 
    typedef struct VkAccelerationStructureBuildRangeInfoKHR {
        uint32_t    primitiveCount  = 1u;
        uint32_t    primitiveOffset = 0u;
        uint32_t    firstVertex     = 0u;
        uint32_t    transformOffset = 0u;

        // 
        STRUCT_OPERATORS(VkAccelerationStructureBuildRangeInfoKHR)
        VK_HPP_STRUCT_OPERATORS(VkAccelerationStructureBuildRangeInfoKHR, vk::AccelerationStructureBuildRangeInfoKHR)
    } VkAccelerationStructureBuildRangeInfoKHR;

    // 
    typedef struct VkAccelerationStructureBuildGeometryInfoKHR {
        VkStructureType                                     sType                    = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
        const void*                                         pNext                    = nullptr;
        VkAccelerationStructureTypeKHR                      type                     = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
        FLAGF(VkBuildAccelerationStructure, KHR)            flags                    = VsDefaultBuildAccelerationStructureFlags;
        VkBuildAccelerationStructureModeKHR                 mode                     = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
        VkAccelerationStructureKHR                          srcAccelerationStructure = VK_NULL_HANDLE;
        VkAccelerationStructureKHR                          dstAccelerationStructure = VK_NULL_HANDLE;
        uint32_t                                            geometryCount            = 1u;
        const ::VkAccelerationStructureGeometryKHR*         pGeometries              = nullptr;
        const ::VkAccelerationStructureGeometryKHR* const*  ppGeometries             = nullptr;
        VkDeviceOrHostAddressKHR                            scratchData              = {};

        // vector of pointers 
        VkAccelerationStructureBuildGeometryInfoKHR& setGeometries(const std::vector<const ::VkAccelerationStructureGeometryKHR*>& geometries){
            this->ppGeometries = geometries.data();
            this->geometryCount = static_cast<uint32_t>(geometries.size());
        };

        // vector of geometries
        VkAccelerationStructureBuildGeometryInfoKHR& setGeometries(const std::vector<VkAccelerationStructureGeometryKHR> geometries = {}){
            this->pGeometries = reinterpret_cast<::VkAccelerationStructureGeometryKHR*>((void*)geometries.data());
            this->geometryCount = static_cast<uint32_t>(geometries.size());
        };

        // 
        STRUCT_OPERATORS(VkAccelerationStructureBuildGeometryInfoKHR)
        VK_HPP_STRUCT_OPERATORS(VkAccelerationStructureBuildGeometryInfoKHR, vk::AccelerationStructureBuildGeometryInfoKHR)
    } VkAccelerationStructureBuildGeometryInfoKHR;

    // TODO: VkDescriptorBufferInfo getter and acceptor
    typedef struct VkAccelerationStructureCreateInfoKHR {
        VkStructureType                          sType          = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
        const void*                              pNext          = nullptr;
        VkAccelerationStructureCreateFlagsKHR    createFlags    = {};
        VkBuffer                                 buffer         = VK_NULL_HANDLE;
        VkDeviceSize                             offset         = 0ull;
        VkDeviceSize                             size           = VK_WHOLE_SIZE;
        VkAccelerationStructureTypeKHR           type           = VK_ACCELERATION_STRUCTURE_TYPE_GENERIC_KHR;
        VkDeviceAddress                          deviceAddress  = {};

        // For Vector's support
        operator VkDescriptorBufferInfo&() { return *reinterpret_cast<VkDescriptorBufferInfo*>(&buffer); };
        operator const VkDescriptorBufferInfo&() const { return *reinterpret_cast<const VkDescriptorBufferInfo*>(&buffer); };
        VkAccelerationStructureCreateInfoKHR& operator=(const VkDescriptorBufferInfo& info) {memcpy(&buffer, &info, sizeof(VkDescriptorBufferInfo)); };

        // 
        STRUCT_OPERATORS(VkAccelerationStructureCreateInfoKHR)
        VK_HPP_STRUCT_OPERATORS(VkAccelerationStructureCreateInfoKHR, vk::AccelerationStructureCreateInfoKHR)
    } VkAccelerationStructureCreateInfoKHR;

    // 
    typedef struct VkRayTracingPipelineInterfaceCreateInfoKHR {
        VkStructureType    sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_INTERFACE_CREATE_INFO_KHR;
        const void*        pNext = nullptr;
        uint32_t           maxPipelineRayPayloadSize      = 128u;
        uint32_t           maxPipelineRayHitAttributeSize = 128u;

        // 
        STRUCT_OPERATORS(VkRayTracingPipelineInterfaceCreateInfoKHR)
        VK_HPP_STRUCT_OPERATORS(VkRayTracingPipelineInterfaceCreateInfoKHR, vk::RayTracingPipelineInterfaceCreateInfoKHR)
    } VkRayTracingPipelineInterfaceCreateInfoKHR;

    // 
    typedef struct VkRayTracingShaderGroupCreateInfoKHR {
        VkStructureType                   sType                             = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
        const void*                       pNext                             = nullptr;
        VkRayTracingShaderGroupTypeKHR    type                              = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
        uint32_t                          generalShader                     = VK_SHADER_UNUSED_KHR;
        uint32_t                          closestHitShader                  = VK_SHADER_UNUSED_KHR;
        uint32_t                          anyHitShader                      = VK_SHADER_UNUSED_KHR;
        uint32_t                          intersectionShader                = VK_SHADER_UNUSED_KHR;
        const void*                       pShaderGroupCaptureReplayHandle   = nullptr;

        // 
        STRUCT_OPERATORS(VkRayTracingShaderGroupCreateInfoKHR)
        VK_HPP_STRUCT_OPERATORS(VkRayTracingShaderGroupCreateInfoKHR, vk::RayTracingShaderGroupCreateInfoKHR)
    } VkRayTracingShaderGroupCreateInfoKHR;

    // 
    typedef struct VkRayTracingPipelineCreateInfoKHR {
        VkStructureType                                      sType                          = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
        const void*                                          pNext                          = nullptr;
        VkPipelineCreateFlags                                flags                          = {};
        uint32_t                                             stageCount                     = 0u;
        const VkPipelineShaderStageCreateInfo*               pStages                        = nullptr;
        uint32_t                                             groupCount                     = 0u;
        const VkRayTracingShaderGroupCreateInfoKHR*          pGroups                        = nullptr;
        uint32_t                                             maxPipelineRayRecursionDepth   = 4u;
        const VkPipelineLibraryCreateInfoKHR*                pLibraryInfo                   = nullptr;
        const VkRayTracingPipelineInterfaceCreateInfoKHR*    pLibraryInterface              = nullptr;
        const VkPipelineDynamicStateCreateInfo*              pDynamicState                  = nullptr;
        VkPipelineLayout                                     layout                         = VK_NULL_HANDLE;
        VkPipeline                                           basePipelineHandle             = VK_NULL_HANDLE;
        int32_t                                              basePipelineIndex              = 0;

        // 
        STRUCT_OPERATORS(VkRayTracingPipelineCreateInfoKHR)
        VK_HPP_STRUCT_OPERATORS(VkRayTracingPipelineCreateInfoKHR, vk::RayTracingPipelineCreateInfoKHR)
    } VkRayTracingPipelineCreateInfoKHR;

    //
    typedef struct VkShaderModuleCreateInfo {
        VkStructureType              sType      = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        const void*                  pNext      = nullptr;
        VkShaderModuleCreateFlags    flags      = VsDefaultShaderModuleFlags;
        size_t                       codeSize   = 0ull;
        const uint32_t*              pCode      = nullptr;

        // 
        VkShaderModuleCreateInfo& setCode(const std::vector<uint32_t>& V = {}) { pCode = V.data(); codeSize = V.size() * 4ull; return *this; };

        // 
        STRUCT_OPERATORS(VkShaderModuleCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkShaderModuleCreateInfo, vk::ShaderModuleCreateInfo)
    } VkShaderModuleCreateInfo;

    // 
    typedef struct VkCommandBufferAllocateInfo {
        VkStructureType         sType               = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        const void*             pNext               = nullptr;
        VkCommandPool           commandPool         = VK_NULL_HANDLE;
        VkCommandBufferLevel    level               = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        uint32_t                commandBufferCount  = 1u;

        // 
        STRUCT_OPERATORS(VkCommandBufferAllocateInfo)
        VK_HPP_STRUCT_OPERATORS(VkCommandBufferAllocateInfo, vk::CommandBufferAllocateInfo)
    } VkCommandBufferAllocateInfo;

    // 
    typedef struct VkCommandBufferBeginInfo {
        VkStructureType                          sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        const void*                              pNext              = nullptr;
        VkCommandBufferUsageFlags                flags              = 0u; // TODO: FLAGS
        const VkCommandBufferInheritanceInfo*    pInheritanceInfo   = nullptr;

        // 
        STRUCT_OPERATORS(VkCommandBufferBeginInfo)
        VK_HPP_STRUCT_OPERATORS(VkCommandBufferBeginInfo, vk::CommandBufferBeginInfo)
    } VkCommandBufferBeginInfo;

    // 
    typedef struct VkExportMemoryAllocateInfo {
        VkStructureType                    sType        = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO;
        const void*                        pNext        = nullptr;
        FLAGS(VkExternalMemoryHandleType) handleTypes  = {};

        // 
        STRUCT_OPERATORS(VkExportMemoryAllocateInfo)
        VK_HPP_STRUCT_OPERATORS(VkExportMemoryAllocateInfo, vk::ExportMemoryAllocateInfo)
    } VkExportMemoryAllocateInfo;

    // 
    typedef struct VkMemoryAllocateInfo {
        VkStructureType    sType            = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        const void*        pNext            = nullptr;
        VkDeviceSize       allocationSize   = 0u;
        uint32_t           memoryTypeIndex  = 0u;

        // 
        STRUCT_OPERATORS(VkMemoryAllocateInfo)
        VK_HPP_STRUCT_OPERATORS(VkMemoryAllocateInfo, vk::MemoryAllocateInfo)
    } VkMemoryAllocateInfo;

    // 
    typedef struct VkApplicationInfo {
        VkStructureType    sType                = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        const void*        pNext                = nullptr;
        const char*        pApplicationName     = "vkh-based";
        uint32_t           applicationVersion   = 0u;
        const char*        pEngineName          = "vkh-based";
        uint32_t           engineVersion        = 0u;
        uint32_t           apiVersion           = VK_API_VERSION_1_2;

        // 
        STRUCT_OPERATORS(VkApplicationInfo)
        VK_HPP_STRUCT_OPERATORS(VkApplicationInfo, vk::ApplicationInfo)
    } VkApplicationInfo;

    // 
    typedef struct VkInstanceCreateInfo {
        VkStructureType             sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        const void*                 pNext                   = nullptr;
        VkInstanceCreateFlags       flags                   = 0u; // TODO: FLAGS
        const ::VkApplicationInfo*    pApplicationInfo        = nullptr;
        uint32_t                    enabledLayerCount       = 0u;
        const char* const*          ppEnabledLayerNames     = nullptr;
        uint32_t                    enabledExtensionCount   = 0u;
        const char* const*          ppEnabledExtensionNames = nullptr;

        // 
        VkInstanceCreateInfo& setEnabledExtensionNames(const std::vector<char *>& V = {}) { ppEnabledExtensionNames = V.data(); enabledExtensionCount = static_cast<uint32_t>(V.size()); return *this; };
        VkInstanceCreateInfo& setEnabledLayerNames(const std::vector<char *>& V = {}) { ppEnabledLayerNames = V.data(); enabledLayerCount = static_cast<uint32_t>(V.size()); return *this; };

        // 
        STRUCT_OPERATORS(VkInstanceCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkInstanceCreateInfo, vk::InstanceCreateInfo)
    } VkInstanceCreateInfo;

    //
    typedef struct VkImageMemoryBarrier {
        VkStructureType            sType                = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        const void*                pNext                = nullptr;
        FLAGS(VkAccess)            srcAccessMask        = {};
        FLAGS(VkAccess)            dstAccessMask        = {};
        VkImageLayout              oldLayout            = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageLayout              newLayout            = VK_IMAGE_LAYOUT_GENERAL;
        uint32_t                   srcQueueFamilyIndex  = 0u;
        uint32_t                   dstQueueFamilyIndex  = 0u;
        VkImage                    image                = VK_NULL_HANDLE;
        VkImageSubresourceRange    subresourceRange     = {};

        // 
        STRUCT_OPERATORS(VkImageMemoryBarrier)
        VK_HPP_STRUCT_OPERATORS(VkImageMemoryBarrier, vk::ImageMemoryBarrier)
    } VkImageMemoryBarrier;

    // 
    typedef struct VkFenceCreateInfo {
        VkStructureType       sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        const void*           pNext = nullptr;
        VkFenceCreateFlags    flags = 0u; // 

        // 
        STRUCT_OPERATORS(VkFenceCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkFenceCreateInfo, vk::FenceCreateInfo)
    } VkFenceCreateInfo;

    // 
    typedef struct VkSubmitInfo {
        VkStructureType                sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        const void*                    pNext                = nullptr;
        uint32_t                       waitSemaphoreCount   = 0u;
        const VkSemaphore*             pWaitSemaphores      = nullptr;
        const ::VkPipelineStageFlags*  pWaitDstStageMask    = nullptr;
        uint32_t                       commandBufferCount   = 0u;
        const VkCommandBuffer*         pCommandBuffers      = nullptr;
        uint32_t                       signalSemaphoreCount = 0u;
        const VkSemaphore*             pSignalSemaphores    = nullptr;

        // 
        VkSubmitInfo& setCommandBuffers(const std::vector<VkCommandBuffer>& V = {}) { pCommandBuffers = V.data(); commandBufferCount = static_cast<uint32_t>(V.size()); return *this; };
        VkSubmitInfo& setSignalSemaphores(const std::vector<VkSemaphore>& V = {}) { pSignalSemaphores = V.data(); signalSemaphoreCount = static_cast<uint32_t>(V.size()); return *this; };
        VkSubmitInfo& setWaitSemaphores(const std::vector<VkSemaphore>& V = {}) { pWaitSemaphores = V.data(); waitSemaphoreCount = static_cast<uint32_t>(V.size()); return *this; };

        // 
        STRUCT_OPERATORS(VkSubmitInfo)
        VK_HPP_STRUCT_OPERATORS(VkSubmitInfo, vk::SubmitInfo)
    } VkSubmitInfo;

    // 
    typedef struct VkPipelineCacheCreateInfo {
        VkStructureType               sType             = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        const void*                   pNext             = nullptr;
        VkPipelineCacheCreateFlags    flags             = 0u; // TODO: FLAGS
        size_t                        initialDataSize   = 0ull;
        const void*                   pInitialData      = nullptr;

        // 
        template<class T = uint8_t> 
        VkPipelineCacheCreateInfo& setInitialData(const std::vector<T>& V = {}) { pInitialData = V.data(); initialDataSize = V.size(); return *this; };

        // 
        STRUCT_OPERATORS(VkPipelineCacheCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkPipelineCacheCreateInfo, vk::PipelineCacheCreateInfo)
    } VkPipelineCacheCreateInfo;

    // 
    typedef struct VkQueueFamilyProperties {
        FLAGS(VkQueue)  queueFlags                  = {};
        uint32_t        queueCount                  = 0u;
        uint32_t        timestampValidBits          = 0u;
        VkExtent3D      minImageTransferGranularity = {};

        // 
        STRUCT_OPERATORS(VkQueueFamilyProperties)
        VK_HPP_STRUCT_OPERATORS(VkQueueFamilyProperties, vk::QueueFamilyProperties)
    } VkQueueFamilyProperties;

    // 
    typedef struct VkDeviceQueueCreateInfo {
        VkStructureType             sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        const void*                 pNext            = nullptr;
        VkDeviceQueueCreateFlags    flags            = 0u; // TODO: FLAGS
        uint32_t                    queueFamilyIndex = 0u;
        uint32_t                    queueCount       = 0u;
        const float*                pQueuePriorities = nullptr;

        // 
        VkDeviceQueueCreateInfo& setQueuePriorities(const std::vector<float>& V = {}) { pQueuePriorities = V.data(); queueCount = static_cast<uint32_t>(V.size()); return *this; };

        // 
        STRUCT_OPERATORS(VkDeviceQueueCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkDeviceQueueCreateInfo, vk::DeviceQueueCreateInfo)
    } VkDeviceQueueCreateInfo;

    // 
    inline constexpr auto VsDefaultCommandPoolFlags = VkCommandPoolCreateFlags{};

    // 
    typedef struct VkCommandPoolCreateInfo {
        VkStructureType             sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        const void*                 pNext            = nullptr;
        FLAGS(VkCommandPoolCreate)  flags            = VsDefaultCommandPoolFlags;
        uint32_t                    queueFamilyIndex = 0u;

        // 
        STRUCT_OPERATORS(VkCommandPoolCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkCommandPoolCreateInfo, vk::CommandPoolCreateInfo)
    } VkCommandPoolCreateInfo;

    // 
    typedef struct VkDeviceCreateInfo {
        VkStructureType                    sType                    = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        const void*                        pNext                    = nullptr;
        VkDeviceCreateFlags                flags                    = 0u; // TODO: FLAGS
        uint32_t                           queueCreateInfoCount     = 0u;
        const VkDeviceQueueCreateInfo*     pQueueCreateInfos        = nullptr; // TODO: NATIVE
        uint32_t                           enabledLayerCount        = 0u;
        const char* const*                 ppEnabledLayerNames      = nullptr;
        uint32_t                           enabledExtensionCount    = 0u;
        const char* const*                 ppEnabledExtensionNames  = nullptr;
        const VkPhysicalDeviceFeatures*    pEnabledFeatures         = nullptr; // TODO: NATIVE

        // 
        VkDeviceCreateInfo& setQueueCreateInfos(const std::vector<VkDeviceQueueCreateInfo>& V = {}) { pQueueCreateInfos = V.data(); queueCreateInfoCount = static_cast<uint32_t>(V.size()); return *this; };
        VkDeviceCreateInfo& setPEnabledExtensionNames(const std::vector<const char*>& V = {}) { ppEnabledExtensionNames = V.data(); enabledExtensionCount = static_cast<uint32_t>(V.size()); return *this; };
        VkDeviceCreateInfo& setPEnabledLayerNames(const std::vector<const char* >& V = {}) { ppEnabledLayerNames = V.data(); enabledLayerCount = static_cast<uint32_t>(V.size()); return *this; };

        // 
        STRUCT_OPERATORS(VkDeviceCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkDeviceCreateInfo,vk::DeviceCreateInfo)
    } VkDeviceCreateInfo;

    //
    typedef struct VkSemaphoreCreateInfo {
        VkStructureType           sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        const void*               pNext = nullptr;
        VkSemaphoreCreateFlags    flags = 0u; // TODO: FLAGS

        // 
        STRUCT_OPERATORS(VkSemaphoreCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkSemaphoreCreateInfo, vk::SemaphoreCreateInfo)
    } VkSemaphoreCreateInfo;

    // 
    typedef struct VkPhysicalDeviceMemoryProperties2 {
        VkStructureType                     sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
        void*                               pNext            = nullptr;
        VkPhysicalDeviceMemoryProperties    memoryProperties = {};

        // 
        operator const VkPhysicalDeviceMemoryProperties&() const { return memoryProperties; };
        operator VkPhysicalDeviceMemoryProperties&() { return memoryProperties; };
        operator const ::VkPhysicalDeviceMemoryProperties*() const { return &memoryProperties; };
        operator ::VkPhysicalDeviceMemoryProperties*() { return &memoryProperties; };

        // 
        STRUCT_OPERATORS(VkPhysicalDeviceMemoryProperties2)
        VK_HPP_STRUCT_OPERATORS(VkPhysicalDeviceMemoryProperties2, vk::PhysicalDeviceMemoryProperties2)
    } VkPhysicalDeviceMemoryProperties2;

    // 
    typedef struct VkPhysicalDeviceProperties2 {
        VkStructureType               sType      = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        void*                         pNext      = nullptr;
        VkPhysicalDeviceProperties    properties = {};

        // 
        operator const VkPhysicalDeviceProperties&() const { return properties; };
        operator VkPhysicalDeviceProperties&() { return properties; };
        operator const VkPhysicalDeviceProperties*() const { return &properties; };
        operator VkPhysicalDeviceProperties*() { return &properties; };

        // 
        STRUCT_OPERATORS(VkPhysicalDeviceProperties2)
        VK_HPP_STRUCT_OPERATORS(VkPhysicalDeviceProperties2, vk::PhysicalDeviceProperties2)
    } VkPhysicalDeviceProperties2;

    // 
    typedef struct VkPhysicalDeviceFeatures2 {
        VkStructureType             sType    = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        void*                       pNext    = nullptr;
        VkPhysicalDeviceFeatures    features = {};

        //
        operator const VkPhysicalDeviceFeatures&() const { return features; };
        operator VkPhysicalDeviceFeatures&() { return features; };
        operator const VkPhysicalDeviceFeatures*() const { return &features; };
        operator VkPhysicalDeviceFeatures*() { return &features; };

        // 
        STRUCT_OPERATORS(VkPhysicalDeviceFeatures2)
        VK_HPP_STRUCT_OPERATORS(VkPhysicalDeviceFeatures2, vk::PhysicalDeviceFeatures2)
    } VkPhysicalDeviceFeatures2;

    // 
    typedef struct VkMemoryRequirements2 {
        VkStructureType         sType               = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
        void*                   pNext               = nullptr;
        VkMemoryRequirements    memoryRequirements  = {};

        // 
        operator const VkMemoryRequirements&() const { return memoryRequirements; };
        operator VkMemoryRequirements&() { return memoryRequirements; };
        operator const VkMemoryRequirements*() const { return &memoryRequirements; };
        operator VkMemoryRequirements*() { return &memoryRequirements; };

        // 
        STRUCT_OPERATORS(VkMemoryRequirements2)
        VK_HPP_STRUCT_OPERATORS(VkMemoryRequirements2, vk::MemoryRequirements2)
    } VkMemoryRequirements2;

    // 
    union VkClearValue;

    // 
    typedef union VkClearColorValue {
        glm::vec4 float32;
        glm::ivec4 int32;
        glm::uvec4 uint32 = glm::uvec4(0u);

        // 
        operator glm::vec4&() { return float32; };
        operator glm::ivec4& () { return int32; };
        operator glm::uvec4& () { return uint32; };

        // 
        operator const glm::vec4& () const { return float32; };
        operator const glm::ivec4& () const { return int32; };
        operator const glm::uvec4& () const { return uint32; };

        // 
        VkClearColorValue& operator=(const glm::vec4& float32) { this->float32 = float32; };
        VkClearColorValue& operator=(const glm::ivec4& int32) { this->int32 = int32; };
        VkClearColorValue& operator=(const glm::uvec4& uint32) { this->uint32 = uint32; };

        // TODO: fix original type issue
        operator VkClearValue& () { return reinterpret_cast<VkClearValue&>(*this); };
        operator const VkClearValue& () const { return reinterpret_cast<const VkClearValue&>(*this); };

        // 
        STRUCT_OPERATORS(VkClearColorValue)
        VK_HPP_STRUCT_OPERATORS(VkClearColorValue, vk::ClearColorValue)
    } VkClearColorValue;

    // 
    typedef struct VkClearDepthStencilValue {
        float       depth = 1u;
        uint32_t    stencil : 8;

        // TODO: fix original type issue
        operator VkClearValue& () { return reinterpret_cast<VkClearValue&>(*this); };
        operator const VkClearValue& () const { return reinterpret_cast<const VkClearValue&>(*this); };

        // 
        STRUCT_OPERATORS(VkClearDepthStencilValue)
        VK_HPP_STRUCT_OPERATORS(VkClearDepthStencilValue, vk::ClearDepthStencilValue)
    } VkClearDepthStencilValue;

    // 
    typedef union VkClearValue {
        VkClearColorValue           color = {};
        ::VkClearDepthStencilValue  depthStencil;
        
        //
        operator VkClearColorValue& () { return reinterpret_cast<VkClearColorValue&>(color); };
        operator VkClearDepthStencilValue& () { return reinterpret_cast<VkClearDepthStencilValue&>(depthStencil); };

        // 
        operator const VkClearColorValue& () const { return reinterpret_cast<const VkClearColorValue&>(color); };
        operator const VkClearDepthStencilValue& () const { return reinterpret_cast<const VkClearDepthStencilValue&>(depthStencil); };

        // 
        VkClearValue& operator=(const VkClearColorValue& color) { this->color = color; };
        VkClearValue& operator=(const VkClearDepthStencilValue& depthStencil) { this->depthStencil = depthStencil; };

        // 
        STRUCT_OPERATORS(VkClearValue)
        VK_HPP_STRUCT_OPERATORS(VkClearValue, vk::ClearValue)
    } VkClearValue;

    // 
    typedef struct VkRenderPassBeginInfo {
        VkStructureType     sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        const void*         pNext           = nullptr;
        VkRenderPass        renderPass      = {};
        VkFramebuffer       framebuffer     = {};
        VkRect2D            renderArea      = {};
        uint32_t            clearValueCount = 0u;
        const VkClearValue* pClearValues    = nullptr;

        // 
        VkRenderPassBeginInfo& setClearValues(const std::vector<VkClearValue>& V = {}) { pClearValues = V.data(); clearValueCount = static_cast<uint32_t>(V.size()); return *this; };

        // 
        STRUCT_OPERATORS(VkRenderPassBeginInfo)
        VK_HPP_STRUCT_OPERATORS(VkRenderPassBeginInfo, vk::RenderPassBeginInfo)
    } VkRenderPassBeginInfo;

    //
    typedef struct VkPipelineRasterizationStateStreamCreateInfoEXT {
        VkStructureType                                     sType               = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_STREAM_CREATE_INFO_EXT;
        const void*                                         pNext               = nullptr;
        VkPipelineRasterizationStateStreamCreateFlagsEXT    flags               = 0u; // TODO: FLAGS
        uint32_t                                            rasterizationStream = 0u;

        // 
        STRUCT_OPERATORS(VkPipelineRasterizationStateStreamCreateInfoEXT)
        VK_HPP_STRUCT_OPERATORS(VkPipelineRasterizationStateStreamCreateInfoEXT, vk::PipelineRasterizationStateStreamCreateInfoEXT)
    } VkPipelineRasterizationStateStreamCreateInfoEXT;


    //
    typedef struct VkSemaphoreSignalInfo {
        VkStructureType     sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
        const void*         pNext       = nullptr;
        VkSemaphore         semaphore   = VK_NULL_HANDLE;
        uint64_t            value       = 0u;

        // 
        STRUCT_OPERATORS(VkSemaphoreSignalInfo)
        VK_HPP_STRUCT_OPERATORS(VkSemaphoreSignalInfo, vk::SemaphoreSignalInfo)
    } VkSemaphoreSignalInfo;

    // 
    typedef struct VkPresentInfoKHR {
        VkStructureType         sType               = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        const void*             pNext               = nullptr;
        uint32_t                waitSemaphoreCount  = 0u;
        const VkSemaphore*      pWaitSemaphores     = nullptr;
        uint32_t                swapchainCount      = 0u;
        const VkSwapchainKHR*   pSwapchains         = nullptr;
        const uint32_t*         pImageIndices       = nullptr;
        VkResult*               pResults            = nullptr;

        // 
        VkPresentInfoKHR& setSwapchains(const std::vector<VkSwapchainKHR>& V = {}) { pSwapchains = V.data(); swapchainCount = static_cast<uint32_t>(V.size()); return *this; };
        VkPresentInfoKHR& setWaitSemaphores(const std::vector<VkSemaphore>& V = {}) { pWaitSemaphores = V.data(); waitSemaphoreCount = static_cast<uint32_t>(V.size()); return *this; };

        // 
        STRUCT_OPERATORS(VkPresentInfoKHR)
        VK_HPP_STRUCT_OPERATORS(VkPresentInfoKHR, vk::PresentInfoKHR)
    } VkPresentInfoKHR;

    // 
    inline constexpr auto VsDefaultImageUsageFlags = VkImageUsageFlags{};

    // 
    typedef struct VkSwapchainCreateInfoKHR {
        VkStructureType                 sType                   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        const void*                     pNext                   = nullptr;
        VkSwapchainCreateFlagsKHR       flags                   = 0u; // TODO: FLAGS
        VkSurfaceKHR                    surface                 = VK_NULL_HANDLE;
        uint32_t                        minImageCount           = 0u;
        VkFormat                        imageFormat             = VK_FORMAT_R8G8B8A8_SRGB;
        VkColorSpaceKHR                 imageColorSpace         = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        VkExtent2D                      imageExtent             = {1u, 1u};
        uint32_t                        imageArrayLayers        = 1u;
        FLAGS(VkImageUsage)             imageUsage              = VsDefaultImageUsageFlags;
        VkSharingMode                   imageSharingMode        = VK_SHARING_MODE_EXCLUSIVE;
        uint32_t                        queueFamilyIndexCount   = 1u;
        const uint32_t*                 pQueueFamilyIndices     = nullptr;
        VkSurfaceTransformFlagBitsKHR   preTransform            = VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR;
        VkCompositeAlphaFlagBitsKHR     compositeAlpha          = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        VkPresentModeKHR                presentMode             = VK_PRESENT_MODE_IMMEDIATE_KHR;
        VkBool32                        clipped                 = true;
        VkSwapchainKHR                  oldSwapchain            = VK_NULL_HANDLE;

        // 
        VkSwapchainCreateInfoKHR& setQueueFamilyIndices(const std::vector<uint32_t>& V = {}) { pQueueFamilyIndices = V.data(); queueFamilyIndexCount = static_cast<uint32_t>(V.size()); return *this; };

        // 
        STRUCT_OPERATORS(VkSwapchainCreateInfoKHR)
        VK_HPP_STRUCT_OPERATORS(VkSwapchainCreateInfoKHR, vk::SwapchainCreateInfoKHR)
    } VkSwapchainCreateInfoKHR;

    //
    typedef struct VkMemoryAllocateFlagsInfo {
        VkStructureType          sType      = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
        const void*              pNext      = nullptr;
        FLAGS(VkMemoryAllocate)  flags      = VsDefaultMemoryAllocateFlags;
        uint32_t                 deviceMask = 0u;

        // multiple versions
        operator FLAGS(VkMemoryAllocate)& () { return flags; };
        operator const FLAGS(VkMemoryAllocate)& () const { return flags; };
        operator ::VkMemoryAllocateFlags& () { return flags; };
        operator const ::VkMemoryAllocateFlags& () const { return flags; };
        operator vkh::VkMemoryAllocateFlags& () { return flags; };
        operator const vkh::VkMemoryAllocateFlags& () const { return flags; };

        // auto-converted on assign
        VkMemoryAllocateFlagsInfo& operator=(const FLAGS(VkMemoryAllocate)& flags) { this->flags = flags; return *this; };

        // 
        STRUCT_OPERATORS(VkMemoryAllocateFlagsInfo)
        VK_HPP_STRUCT_OPERATORS(VkMemoryAllocateFlagsInfo, vk::MemoryAllocateFlagsInfo)
    } VkMemoryAllocateFlagsInfo;

    //
    typedef struct VkPipelineRasterizationConservativeStateCreateInfoEXT {
        VkStructureType                                           sType                             = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_CONSERVATIVE_STATE_CREATE_INFO_EXT;
        const void*                                               pNext                             = nullptr;
        VkPipelineRasterizationConservativeStateCreateFlagsEXT    flags                             = {};
        VkConservativeRasterizationModeEXT                        conservativeRasterizationMode     = VK_CONSERVATIVE_RASTERIZATION_MODE_DISABLED_EXT;
        float                                                     extraPrimitiveOverestimationSize  = 0.f;

        // mode equally
        VkPipelineRasterizationConservativeStateCreateInfoEXT& operator=(const VkConservativeRasterizationModeEXT& conservativeRasterizationMode) { this->conservativeRasterizationMode = conservativeRasterizationMode; };
        operator VkConservativeRasterizationModeEXT&() { return conservativeRasterizationMode; };
        operator const VkConservativeRasterizationModeEXT&() const { return conservativeRasterizationMode; };

        // 
        STRUCT_OPERATORS(VkPipelineRasterizationConservativeStateCreateInfoEXT)
        VK_HPP_STRUCT_OPERATORS(VkPipelineRasterizationConservativeStateCreateInfoEXT, vk::PipelineRasterizationConservativeStateCreateInfoEXT)
    } VkPipelineRasterizationConservativeStateCreateInfoEXT;

    //
//#ifdef VKT_OPENGL_INTEROP
#ifdef VKT_WIN32_DETECTED
    typedef struct VkSemaphoreGetWin32HandleInfoKHR {
        VkStructureType                          sType      = VK_STRUCTURE_TYPE_SEMAPHORE_GET_WIN32_HANDLE_INFO_KHR;
        const void*                              pNext      = nullptr;
        VkSemaphore                              semaphore  = VK_NULL_HANDLE;
        VkExternalSemaphoreHandleTypeFlagBits    handleType = {};

        // 
        operator VkSemaphore& () { return semaphore; };
        operator const VkSemaphore& () const { return semaphore; };
        VkSemaphoreGetWin32HandleInfoKHR& operator=(const VkSemaphore& semaphore) { this->semaphore = semaphore; return *this; };

        // 
        operator VkExternalSemaphoreHandleTypeFlagBits& () { return handleType; };
        operator const VkExternalSemaphoreHandleTypeFlagBits& () const { return handleType; };
        VkSemaphoreGetWin32HandleInfoKHR& operator=(const VkExternalSemaphoreHandleTypeFlagBits& handleType) { this->handleType = handleType; return *this; };

        // 
        STRUCT_OPERATORS(VkSemaphoreGetWin32HandleInfoKHR)
        VK_HPP_STRUCT_OPERATORS(VkSemaphoreGetWin32HandleInfoKHR, vk::SemaphoreGetWin32HandleInfoKHR)
    } VkSemaphoreGetWin32HandleInfoKHR;
#endif
//#endif

    // 
    inline constexpr auto VsDefaultExternalSemaphoreHandleTypeFlags = VkExternalSemaphoreHandleTypeFlags{};

    // 
    typedef struct VkExportSemaphoreCreateInfo {
        VkStructureType                         sType       = VK_STRUCTURE_TYPE_EXPORT_SEMAPHORE_CREATE_INFO;
        const void*                             pNext       = nullptr;
        FLAGS(VkExternalSemaphoreHandleType)    handleTypes = VsDefaultExternalSemaphoreHandleTypeFlags;

        // multiple versions
        operator FLAGS(VkExternalSemaphoreHandleType)&() { return handleTypes; };
        operator const FLAGS(VkExternalSemaphoreHandleType)&() const { return handleTypes; };
        operator ::VkExternalSemaphoreHandleTypeFlags& () { return handleTypes; };
        operator const ::VkExternalSemaphoreHandleTypeFlags& () const { return handleTypes; };
        operator vkh::VkExternalSemaphoreHandleTypeFlags& () { return handleTypes; };
        operator const vkh::VkExternalSemaphoreHandleTypeFlags& () const { return handleTypes; };

        // auto-conversion
        VkExportSemaphoreCreateInfo& operator=(const FLAGS(VkExternalSemaphoreHandleType)& handleType) { this->handleTypes = handleTypes; return *this; };

        // 
        STRUCT_OPERATORS(VkExportSemaphoreCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkExportSemaphoreCreateInfo, vk::ExportSemaphoreCreateInfo)
    } VkExportSemaphoreCreateInfo;

#pragma pack(pop)

#pragma pack(push, 1)
    struct uint24_t;

    // primary 24-bit value
    struct u24_u8_t { // for compatibility only
        uint32_t u24 : 24; //
        uint32_t u8 : 8;// // 
        operator uint32_t() const { return u24; };
        u24_u8_t(const u24_u8_t& a) { *this = a; };
        u24_u8_t(const uint32_t& a) { *this = a; };
        uint24_t& operator=(const uint32_t& a) { uint32_t* ptr = reinterpret_cast<uint32_t*>(this); *ptr = glm::bitfieldInsert(*ptr, uint32_t(a), 0, 24); };
        uint24_t& operator=(const u24_u8_t& a) { uint32_t* ptr = reinterpret_cast<uint32_t*>(this); *ptr = glm::bitfieldInsert(*ptr, uint32_t(a), 0, 24); };
    };

    // for compatibility only
    struct uint24_t {
        glm::u8vec3 a = glm::u8vec3(0u);

        // constructor
        uint24_t(const uint24_t& a) { this->a = a; };
        uint24_t(const u24_u8_t& a) { u24_u8_t* ptr = reinterpret_cast<u24_u8_t*>(this); ptr->u24 = a.u24; };
        uint24_t(const uint32_t& a) { uint32_t* ptr = reinterpret_cast<uint32_t*>(this); *ptr = glm::bitfieldInsert(*ptr, uint32_t(a), 0, 24); };

        // assign operator
        uint24_t& operator=(const uint24_t& a) { this->a = a; return *this; };
        uint24_t& operator=(const u24_u8_t& a) { u24_u8_t* ptr = reinterpret_cast<u24_u8_t*>(this); ptr->u24 = a.u24; return *this;};
        uint24_t& operator=(const uint32_t& a) { uint32_t* ptr = reinterpret_cast<uint32_t*>(this); *ptr = glm::bitfieldInsert(*ptr, uint32_t(a), 0, 24); return *this; };

        // type conversion
        operator uint32_t() const { return reinterpret_cast<const u24_u8_t&>(a); };
        operator const glm::u8vec3& () const { return a; };
        operator glm::u8vec3& () { return a; };
    };

    // CODING EXTRAS...
    struct VkAccelerationStructureInstanceKHR {
        glm::mat3x4 transform = glm::mat3x4(1.f); // now assignable with GLM
        uint24_t instanceCustomIndex = 0u;
        uint8_t mask = 0xFFu;
        uint24_t instanceShaderBindingTableRecordOffset = 0u;
        uint8_t flags = uint8_t(VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR);
        uint64_t accelerationStructureReference = 0ull;

        // 
        STRUCT_OPERATORS(VkAccelerationStructureInstanceKHR)
        VK_HPP_STRUCT_OPERATORS(VkAccelerationStructureInstanceKHR, vk::AccelerationStructureInstanceKHR)
    }; //__attribute__((packed));
#pragma pack(pop)

};
