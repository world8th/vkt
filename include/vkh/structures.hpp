#pragma once // #

#include "./enums.hpp"
#include "./bitfields.hpp"

namespace vkh { // TODO: Coverage ALL of MOST and Common USING Vulkan Structures
                // TODO: WIP FULL C++20 SUPPORT
                // TODO: WIP FULL Vulkan-HPP COMPATIBILITY (but currently can be accessed by casting hacks, for PRO users only)

    // Structures should be packed accurately as Vulkan.H and Vulkan.HPP
    #pragma pack(push, 8) // BUT Vulkan Should PACK ONLY BY ONE BYTE

    // 
    #define STRUCT_OPERATORS(NAME)\
        operator NAME*() { return this; };\
        operator const NAME*() const { return this; };\
        operator ::NAME&() { return reinterpret_cast<::NAME&>(*this); };\
        operator const ::NAME&() const { return reinterpret_cast<const ::NAME&>(*this); };\
        operator ::NAME*() { return reinterpret_cast<::NAME*>(this); };\
        operator const ::NAME*() const { return reinterpret_cast<const ::NAME*>(this); };\
        NAME& operator =( const ::NAME& info ) { reinterpret_cast<::NAME&>(*this) = info; return *this; };\
        NAME& operator =( const NAME& info ) { reinterpret_cast<::NAME&>(*this) = reinterpret_cast<const ::NAME&>(info); return *this; }; 

    #define VK_HPP_STRUCT_OPERATORS(NAME,VKNAME)\
        operator VKNAME&() { return reinterpret_cast<VKNAME&>(*this); };\
        operator const VKNAME&() const { return reinterpret_cast<const VKNAME&>(*this); };\
        operator VKNAME*() { return reinterpret_cast<VKNAME*>(this); };\
        operator const VKNAME*() const { return reinterpret_cast<const VKNAME*>(this); };\
        VKNAME* operator->() { return reinterpret_cast<VKNAME*>(this); };\
        const VKNAME* operator->() const { return reinterpret_cast<const VKNAME*>(this); };\
        VKNAME& hpp() { return reinterpret_cast<VKNAME&>(*this); };\
        const VKNAME& hpp() const { return reinterpret_cast<const VKNAME&>(*this); };\
        NAME& operator =( const VKNAME& info ) { reinterpret_cast<VKNAME&>(*this) = info; return *this; };

    #define STRUCT_TYPE_COMPATIBLE(NAME,T)\
        NAME& operator=(const T& V) { memcpy(this, &V, sizeof(T)); return *this; };\
        operator T&() { return reinterpret_cast<T&>(*this); };\
        operator const T&() const { return reinterpret_cast<const T&>(*this); };

    // GLM-Compatible
    typedef struct VkOffset3D {
        int32_t    x = 0;
        int32_t    y = 0;
        int32_t    z = 0;

        STRUCT_OPERATORS(VkOffset3D)
        VK_HPP_STRUCT_OPERATORS(VkOffset3D,vk::Offset3D)
        STRUCT_TYPE_COMPATIBLE(VkOffset3D,glm::ivec3)
    } VkOffset3D;

    // GLM-Compatible
    typedef struct VkOffset2D {
        int32_t    x = 0;
        int32_t    y = 0;

        STRUCT_OPERATORS(VkOffset2D)
        VK_HPP_STRUCT_OPERATORS(VkOffset2D,vk::Offset2D)
        STRUCT_TYPE_COMPATIBLE(VkOffset2D,glm::ivec2)
    } VkOffset2D;

    // GLM-Compatible
    typedef struct VkExtent2D {
        uint32_t    width  = 1u;
        uint32_t    height = 1u;

        STRUCT_OPERATORS(VkExtent2D)
        VK_HPP_STRUCT_OPERATORS(VkExtent2D,vk::Extent2D)
        STRUCT_TYPE_COMPATIBLE(VkExtent2D,glm::uvec2)
    } VkExtent2D;

    // GLM-Compatible
    typedef struct VkExtent3D {
        uint32_t    width  = 1u;
        uint32_t    height = 1u;
        uint32_t    depth  = 1u;

        STRUCT_OPERATORS(VkExtent3D)
        VK_HPP_STRUCT_OPERATORS(VkExtent3D,vk::Extent3D)
        STRUCT_TYPE_COMPATIBLE(VkExtent3D,glm::uvec3)
    } VkExtent3D;

    // 
    typedef struct VkRect2D {
        VkOffset2D offset = { 0,0 };
        VkExtent2D extent = { 1,1 };

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

        STRUCT_OPERATORS(VkViewport)
        VK_HPP_STRUCT_OPERATORS(VkViewport,vk::Viewport)
    } VkViewport;

    // 
    typedef struct VkVertexInputBindingDescription {
        uint32_t             binding    = 0u;
        uint32_t             stride     = 16u;
        VkVertexInputRate    inputRate  = VK_VERTEX_INPUT_RATE_VERTEX;

        STRUCT_OPERATORS(VkVertexInputBindingDescription)
        VK_HPP_STRUCT_OPERATORS(VkVertexInputBindingDescription,vk::VertexInputBindingDescription)
    } VkVertexInputBindingDescription;

    // 
    typedef struct VkVertexInputAttributeDescription {
        uint32_t    location = 0u;
        uint32_t    binding  = 0u;
        VkFormat    format   = VK_FORMAT_R32G32B32_SFLOAT;
        uint32_t    offset   = 0u;

        STRUCT_OPERATORS(VkVertexInputAttributeDescription)
        VK_HPP_STRUCT_OPERATORS(VkVertexInputAttributeDescription,vk::VertexInputAttributeDescription)
    } VkVertexInputAttributeDescription;

    //
    typedef struct VkDescriptorPoolSize {
        VkDescriptorType    type            = VK_DESCRIPTOR_TYPE_SAMPLER;
        uint32_t            descriptorCount = 1u;

        STRUCT_OPERATORS(VkDescriptorPoolSize)
        VK_HPP_STRUCT_OPERATORS(VkDescriptorPoolSize,vk::DescriptorPoolSize)
    } VkDescriptorPoolSize;

    // 
    typedef struct VkDescriptorBufferInfo {
        VkBuffer        buffer  = VK_NULL_HANDLE;
        VkDeviceSize    offset  = 0u;
        VkDeviceSize    range   = 16u;

        STRUCT_OPERATORS(VkDescriptorBufferInfo)
        VK_HPP_STRUCT_OPERATORS(VkDescriptorBufferInfo,vk::DescriptorBufferInfo)
    } VkDescriptorBufferInfo;

    // 
    typedef struct VkDescriptorImageInfo {
        VkSampler        sampler     = VK_NULL_HANDLE;
        VkImageView      imageView   = VK_NULL_HANDLE;
        VkImageLayout    imageLayout = VK_IMAGE_LAYOUT_GENERAL;

        STRUCT_OPERATORS(VkDescriptorImageInfo)
        VK_HPP_STRUCT_OPERATORS(VkDescriptorImageInfo,vk::DescriptorImageInfo)
    } VkDescriptorImageInfo;

    // 
    typedef struct VkBufferCopy {
        VkDeviceSize  srcOffset = 0u;
        VkDeviceSize  dstOffset = 0u;
        VkDeviceSize  size      = 16u;

        STRUCT_OPERATORS(VkBufferCopy)
        VK_HPP_STRUCT_OPERATORS(VkBufferCopy, vk::BufferCopy)
    } VkBufferCopy;

    // 
    typedef struct VkWriteDescriptorSet {
        VkStructureType                  sType              = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        const void*                      pNext              = nullptr;
        VkDescriptorSet                  dstSet             = 0u;
        uint32_t                         dstBinding         = 0u;
        uint32_t                         dstArrayElement    = 0u;
        uint32_t                         descriptorCount    = 1u;
        VkDescriptorType                 descriptorType     = VK_DESCRIPTOR_TYPE_SAMPLER;
        const vkh::VkDescriptorImageInfo*     pImageInfo         = nullptr;
        const vkh::VkDescriptorBufferInfo*    pBufferInfo        = nullptr;
        const VkBufferView*              pTexelBufferView   = nullptr;

        STRUCT_OPERATORS(VkWriteDescriptorSet)
        VK_HPP_STRUCT_OPERATORS(VkWriteDescriptorSet,vk::WriteDescriptorSet)
    } VkWriteDescriptorSet;

    // 
    typedef struct VkWriteDescriptorSetAccelerationStructureNV {
        VkStructureType                     sType                       = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_NV;
        const void*                         pNext                       = nullptr;
        uint32_t                            accelerationStructureCount  = 1u;
        const VkAccelerationStructureNV*    pAccelerationStructures     = nullptr;
        
        STRUCT_OPERATORS(VkWriteDescriptorSetAccelerationStructureNV)
        VK_HPP_STRUCT_OPERATORS(VkWriteDescriptorSetAccelerationStructureNV,vk::WriteDescriptorSetAccelerationStructureNV)
    } VkWriteDescriptorSetAccelerationStructureNV;

    // 
    typedef struct VkDescriptorPoolCreateInfo {
        VkStructureType                     sType            = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        const void*                         pNext            = nullptr;
        VkDescriptorPoolCreateFlags         flags            = {}; // TODO: FLAGS
        uint32_t                            maxSets          = 256u;
        uint32_t                            poolSizeCount    = 0u;
        const vkh::VkDescriptorPoolSize*    pPoolSizes       = nullptr;

        STRUCT_OPERATORS(VkDescriptorPoolCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkDescriptorPoolCreateInfo,vk::DescriptorPoolCreateInfo)
    } VkDescriptorPoolCreateInfo;

    // 
    typedef struct VkDeviceCreateInfo {
        VkStructureType                    sType                    = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        const void*                        pNext                    = nullptr;
        VkDeviceCreateFlags                flags                    = {}; // TODO: FLAGS
        uint32_t                           queueCreateInfoCount     = 0u;
        const VkDeviceQueueCreateInfo*     pQueueCreateInfos        = nullptr; // TODO: NATIVE
        uint32_t                           enabledLayerCount        = 0u;
        const char* const*                 ppEnabledLayerNames      = nullptr;
        uint32_t                           enabledExtensionCount    = 0u;
        const char* const*                 ppEnabledExtensionNames  = nullptr;
        const VkPhysicalDeviceFeatures*    pEnabledFeatures         = nullptr; // TODO: NATIVE

        VkDeviceCreateInfo& setQueueCreateInfos(const std::vector<VkDeviceQueueCreateInfo>& V = {}) { pQueueCreateInfos = V.data(); queueCreateInfoCount = V.size(); return *this; };
        VkDeviceCreateInfo& setPEnabledExtensionNames(const std::vector<const char*>& V = {}) { ppEnabledExtensionNames = V.data(); enabledExtensionCount = V.size(); return *this; };
        VkDeviceCreateInfo& setPEnabledLayerNames(const std::vector<const char* >& V = {}) { ppEnabledLayerNames = V.data(); enabledLayerCount = V.size(); return *this; };

        STRUCT_OPERATORS(VkDeviceCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkDeviceCreateInfo,vk::DeviceCreateInfo)
    } VkDeviceCreateInfo;

    // 
    typedef struct VkBufferViewCreateInfo {
        VkStructureType            sType    = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
        const void*                pNext    = nullptr;
        VkBufferViewCreateFlags    flags    = {}; // TODO: FLAGS
        VkBuffer                   buffer   = VK_NULL_HANDLE;
        VkFormat                   format   = VK_FORMAT_R32G32B32A32_SFLOAT;
        VkDeviceSize               offset   = 0u;
        VkDeviceSize               range    = 16u;

        STRUCT_OPERATORS(VkBufferViewCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkBufferViewCreateInfo,vk::BufferViewCreateInfo)
    } VkBufferViewCreateInfo;

    // 
    typedef struct VkBufferCreateInfo {
        VkStructureType     sType                   = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        const void*         pNext                   = nullptr;
        VkBufferCreateFlags flags                   = {};
        VkDeviceSize        size                    = 4u;
        VkBufferUsageFlags  usage                   = {};
        VkSharingMode       sharingMode             = VK_SHARING_MODE_EXCLUSIVE;
        uint32_t            queueFamilyIndexCount   = 0u;
        const uint32_t*     pQueueFamilyIndices     = nullptr;

        VkBufferCreateInfo& setQueueFamilyIndices(const std::vector<uint32_t>& V = {}) { pQueueFamilyIndices = V.data(); queueFamilyIndexCount = V.size(); return *this; };

        STRUCT_OPERATORS(VkBufferCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkBufferCreateInfo,vk::BufferCreateInfo)
    } VkBufferCreateInfo;

    // 
    typedef struct VkImageCreateInfo {
        VkStructureType          sType                  = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        const void*              pNext                  = nullptr;
        VkImageCreateFlags       flags                  = {};
        VkImageType              imageType              = VK_IMAGE_TYPE_2D;
        VkFormat                 format                 = VK_FORMAT_R8G8B8A8_UNORM;
        VkExtent3D               extent                 = {1u,1u,1u};
        uint32_t                 mipLevels              = 1u;
        uint32_t                 arrayLayers            = 1u;
        VkSampleCountFlagBits    samples                = VK_SAMPLE_COUNT_1_BIT;
        VkImageTiling            tiling                 = VK_IMAGE_TILING_OPTIMAL;
        VkImageUsageFlags        usage                  = {};
        VkSharingMode            sharingMode            = VK_SHARING_MODE_EXCLUSIVE;
        uint32_t                 queueFamilyIndexCount  = 0u;
        const uint32_t*          pQueueFamilyIndices    = nullptr;
        VkImageLayout            initialLayout          = VK_IMAGE_LAYOUT_UNDEFINED;

        VkImageCreateInfo& setQueueFamilyIndices(const std::vector<uint32_t>& V = {}) { pQueueFamilyIndices = V.data(); queueFamilyIndexCount = V.size(); return *this; };

        STRUCT_OPERATORS(VkImageCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkImageCreateInfo,vk::ImageCreateInfo)
    } VkImageCreateInfo;

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

        STRUCT_OPERATORS(VkSamplerCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkSamplerCreateInfo,vk::SamplerCreateInfo)
    } VkSamplerCreateInfo;

    // 
    typedef struct VkImageSubresourceRange {
        VkImageAspectFlags    aspectMask     = { .eColor = 1 };
        uint32_t              baseMipLevel   = 0u;
        uint32_t              levelCount     = 1u;
        uint32_t              baseArrayLayer = 0u;
        uint32_t              layerCount     = 1u;
        
        STRUCT_OPERATORS(VkImageSubresourceRange)
        VK_HPP_STRUCT_OPERATORS(VkImageSubresourceRange,vk::ImageSubresourceRange)
    } VkImageSubresourceRange;

    // 
    typedef struct VkImageSubresourceLayers {
        VkImageAspectFlags    aspectMask     = { .eColor = 1 };
        uint32_t              mipLevel       = 0u;
        uint32_t              baseArrayLayer = 0u;
        uint32_t              layerCount     = 1u;
        
        STRUCT_OPERATORS(VkImageSubresourceLayers)
        VK_HPP_STRUCT_OPERATORS(VkImageSubresourceLayers,vk::ImageSubresourceLayers)
    } VkImageSubresourceLayers;

    // 
    typedef struct VkComponentMapping {
        VkComponentSwizzle r = VK_COMPONENT_SWIZZLE_R;
        VkComponentSwizzle g = VK_COMPONENT_SWIZZLE_G;
        VkComponentSwizzle b = VK_COMPONENT_SWIZZLE_B;
        VkComponentSwizzle a = VK_COMPONENT_SWIZZLE_A;
        
        STRUCT_OPERATORS(VkComponentMapping)
        VK_HPP_STRUCT_OPERATORS(VkComponentMapping,vk::ComponentMapping)
    } VkComponentMapping;

    // 
    typedef struct VkImageViewCreateInfo {
        VkStructureType            sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        const void*                pNext            = nullptr;
        VkImageViewCreateFlags     flags            = {};
        VkImage                    image            = VK_NULL_HANDLE;
        VkImageViewType            viewType         = VK_IMAGE_VIEW_TYPE_2D;
        VkFormat                   format           = VK_FORMAT_R8G8B8A8_UNORM;
        VkComponentMapping         components       = {};
        VkImageSubresourceRange    subresourceRange = {};

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
        
        STRUCT_OPERATORS(VkImageCopy)
        VK_HPP_STRUCT_OPERATORS(VkImageCopy,vk::ImageCopy)
    } VkImageCopy;

    // 
    typedef struct VkPipelineInputAssemblyStateCreateInfo {
        VkStructureType                                 sType                   = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        const void*                                     pNext                   = nullptr;
        VkPipelineInputAssemblyStateCreateFlags         flags                   = {}; // TODO: FLAGS
        VkPrimitiveTopology                             topology                = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        VkBool32                                        primitiveRestartEnable  = false;

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

        VkDescriptorSetAllocateInfo& setSetLayouts(const std::vector<VkDescriptorSetLayout>& V = {}) { pSetLayouts = V.data(); descriptorSetCount = V.size(); return *this; };

        STRUCT_OPERATORS(VkDescriptorSetAllocateInfo)
        VK_HPP_STRUCT_OPERATORS(VkDescriptorSetAllocateInfo,vk::DescriptorSetAllocateInfo)
    } VkDescriptorSetAllocateInfo;

    // 
    typedef struct VkPipelineVertexInputStateCreateInfo {
        VkStructureType                                 sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        const void*                                     pNext                           = nullptr;
        VkPipelineVertexInputStateCreateFlags           flags                           = {};
        uint32_t                                        vertexBindingDescriptionCount   = 0u;
        const VkVertexInputBindingDescription*          pVertexBindingDescriptions      = nullptr;
        uint32_t                                        vertexAttributeDescriptionCount = 0u;
        const VkVertexInputAttributeDescription*        pVertexAttributeDescriptions    = nullptr;

        VkPipelineVertexInputStateCreateInfo& setVertexBindingDescriptions(const std::vector<VkVertexInputBindingDescription>& V = {}) { pVertexBindingDescriptions = V.data(); vertexBindingDescriptionCount = V.size(); return *this; };
        VkPipelineVertexInputStateCreateInfo& setVertexAttributeDescriptions(const std::vector<VkVertexInputAttributeDescription>& V = {}) { pVertexAttributeDescriptions = V.data(); vertexAttributeDescriptionCount = V.size(); return *this; };

        STRUCT_OPERATORS(VkPipelineVertexInputStateCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkPipelineVertexInputStateCreateInfo,vk::PipelineVertexInputStateCreateInfo)
    } VkPipelineVertexInputStateCreateInfo;

    // 
    typedef struct VkPipelineTessellationStateCreateInfo {
        VkStructureType                           sType                 = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        const void*                               pNext                 = nullptr;
        VkPipelineTessellationStateCreateFlags    flags                 = {}; // TODO: FLAGS
        uint32_t                                  patchControlPoints    = 0u;

        STRUCT_OPERATORS(VkPipelineTessellationStateCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkPipelineTessellationStateCreateInfo,vk::PipelineTessellationStateCreateInfo)
    } VkPipelineTessellationStateCreateInfo;

    // 
    typedef struct VkPipelineViewportStateCreateInfo {
        VkStructureType                       sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        const void*                           pNext         = nullptr;
        VkPipelineViewportStateCreateFlags    flags         = {}; // TODO: FLAGS
        uint32_t                              viewportCount = 1u;
        const VkViewport*                     pViewports    = nullptr;
        uint32_t                              scissorCount  = 1u;
        const VkRect2D*                       pScissors     = nullptr;

        VkPipelineViewportStateCreateInfo& setViewports(const std::vector<VkViewport>& V = {}) { pViewports = V.data(); viewportCount = V.size(); return *this; };
        VkPipelineViewportStateCreateInfo& setScissors(const std::vector<VkRect2D>& V = {}) { pScissors = V.data(); scissorCount = V.size(); return *this; };

        STRUCT_OPERATORS(VkPipelineViewportStateCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkPipelineViewportStateCreateInfo,vk::PipelineViewportStateCreateInfo)
    } VkPipelineViewportStateCreateInfo;

    // 
    typedef struct VkPipelineRasterizationStateCreateInfo {
        VkStructureType                            sType                    = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        const void*                                pNext                    = nullptr;
        VkPipelineRasterizationStateCreateFlags    flags                    = {}; // TODO: FLAGS
        VkBool32                                   depthClampEnable         = false;
        VkBool32                                   rasterizerDiscardEnable  = false;
        VkPolygonMode                              polygonMode              = VK_POLYGON_MODE_FILL;
        VkCullModeFlags                            cullMode                 = {};
        VkFrontFace                                frontFace                = VK_FRONT_FACE_COUNTER_CLOCKWISE ;
        VkBool32                                   depthBiasEnable          = false;
        float                                      depthBiasConstantFactor  = 0.f;
        float                                      depthBiasClamp           = 0.f;
        float                                      depthBiasSlopeFactor     = 0.f;
        float                                      lineWidth                = 1.f;

        STRUCT_OPERATORS(VkPipelineRasterizationStateCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkPipelineRasterizationStateCreateInfo,vk::PipelineRasterizationStateCreateInfo)
    } VkPipelineRasterizationStateCreateInfo;

    //
    typedef struct VkPipelineMultisampleStateCreateInfo {
        VkStructureType                          sType                  = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        const void*                              pNext                  = nullptr;
        VkPipelineMultisampleStateCreateFlags    flags                  = {}; // TODO: FLAGS
        VkSampleCountFlagBits                    rasterizationSamples   = VK_SAMPLE_COUNT_1_BIT;
        VkBool32                                 sampleShadingEnable    = false;
        float                                    minSampleShading       = 0.f;
        const VkSampleMask*                      pSampleMask            = nullptr;
        VkBool32                                 alphaToCoverageEnable  = false;
        VkBool32                                 alphaToOneEnable       = false;

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

        STRUCT_OPERATORS(VkStencilOpState)
        VK_HPP_STRUCT_OPERATORS(VkStencilOpState,vk::StencilOpState)
    } VkStencilOpState;

    // 
    typedef struct VkPipelineDepthStencilStateCreateInfo {
        VkStructureType                           sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        const void*                               pNext                 = nullptr;
        VkPipelineDepthStencilStateCreateFlags    flags                 = {}; // TODO: FLAGS
        VkBool32                                  depthTestEnable       = false;
        VkBool32                                  depthWriteEnable      = false;
        VkCompareOp                               depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
        VkBool32                                  depthBoundsTestEnable = false;
        VkBool32                                  stencilTestEnable     = false;
        VkStencilOpState                          front                 = {};
        VkStencilOpState                          back                  = {};
        float                                     minDepthBounds        = 0.f;
        float                                     maxDepthBounds        = 1.f;

        STRUCT_OPERATORS(VkPipelineDepthStencilStateCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkPipelineDepthStencilStateCreateInfo,vk::PipelineDepthStencilStateCreateInfo)
    } VkPipelineDepthStencilStateCreateInfo;

    // 
    typedef struct VkPipelineColorBlendAttachmentState {
        VkBool32                 blendEnable         = false;
        VkBlendFactor            srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        VkBlendFactor            dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        VkBlendOp                colorBlendOp        = VK_BLEND_OP_ADD;
        VkBlendFactor            srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        VkBlendFactor            dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        VkBlendOp                alphaBlendOp        = VK_BLEND_OP_ADD;
        VkColorComponentFlags    colorWriteMask      = {.eR = 1, .eG = 1, .eB = 1, .eA = 1};

        STRUCT_OPERATORS(VkPipelineColorBlendAttachmentState)
        VK_HPP_STRUCT_OPERATORS(VkPipelineColorBlendAttachmentState,vk::PipelineColorBlendAttachmentState)
    } VkPipelineColorBlendAttachmentState;

    // 
    typedef struct VkPipelineColorBlendStateCreateInfo {
        VkStructureType                               sType            = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        const void*                                   pNext            = nullptr;
        VkPipelineColorBlendStateCreateFlags          flags            = {}; // TODO: FLAGS
        VkBool32                                      logicOpEnable    = false;
        VkLogicOp                                     logicOp          = VK_LOGIC_OP_SET;
        uint32_t                                      attachmentCount  = 0u;
        const VkPipelineColorBlendAttachmentState*    pAttachments     = nullptr;
        glm::vec4                                     blendConstants   = {1.f,1.f,1.f,1.f};
        //float                                         blendConstants[4];

        VkPipelineColorBlendStateCreateInfo& setAttachments(const std::vector<VkPipelineColorBlendAttachmentState>& V = {}) { pAttachments = V.data(); attachmentCount = V.size(); return *this; };

        STRUCT_OPERATORS(VkPipelineColorBlendStateCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkPipelineColorBlendStateCreateInfo,vk::PipelineColorBlendStateCreateInfo)
    } VkPipelineColorBlendStateCreateInfo;

    // 
    typedef struct VkPipelineDynamicStateCreateInfo {
        VkStructureType                      sType              = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        const void*                          pNext              = nullptr;
        VkPipelineDynamicStateCreateFlags    flags              = {}; // TODO: FLAGS
        uint32_t                             dynamicStateCount  = 0u;
        const VkDynamicState*                pDynamicStates     = nullptr;

        VkPipelineDynamicStateCreateInfo& setDynamicStates(const std::vector<VkDynamicState>& V = {}) { pDynamicStates = V.data(); dynamicStateCount = V.size(); return *this; };

        STRUCT_OPERATORS(VkPipelineDynamicStateCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkPipelineDynamicStateCreateInfo,vk::PipelineDynamicStateCreateInfo)
    } VkPipelineDynamicStateCreateInfo;

    //
    typedef struct VkPipelineShaderStageCreateInfo {
        VkStructureType                     sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        const void*                         pNext               = nullptr;
        VkPipelineShaderStageCreateFlags    flags               = {}; // TODO: FLAGS
        VkShaderStageFlagBits               stage               = VK_SHADER_STAGE_COMPUTE_BIT;
        VkShaderModule                      module              = VK_NULL_HANDLE;
        const char*                         pName               = "main";
        const VkSpecializationInfo*         pSpecializationInfo = nullptr; // TODO: NATIVE

        STRUCT_OPERATORS(VkPipelineShaderStageCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkPipelineShaderStageCreateInfo,vk::PipelineShaderStageCreateInfo)
    } VkPipelineShaderStageCreateInfo;

    // 
    typedef struct VkGraphicsPipelineCreateInfo {
        VkStructureType                                  sType                  = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        const void*                                      pNext                  = nullptr;
        VkPipelineCreateFlags                            flags                  = {}; // TODO: FLAGS
        uint32_t                                         stageCount             = 0u;
        const VkPipelineShaderStageCreateInfo*           pStages                = nullptr;
        const VkPipelineVertexInputStateCreateInfo*      pVertexInputState      = nullptr;
        const VkPipelineInputAssemblyStateCreateInfo*    pInputAssemblyState    = nullptr;
        const VkPipelineTessellationStateCreateInfo*     pTessellationState     = nullptr;
        const VkPipelineViewportStateCreateInfo*         pViewportState         = nullptr;
        const VkPipelineRasterizationStateCreateInfo*    pRasterizationState    = nullptr;
        const VkPipelineMultisampleStateCreateInfo*      pMultisampleState      = nullptr;
        const VkPipelineDepthStencilStateCreateInfo*     pDepthStencilState     = nullptr;
        const VkPipelineColorBlendStateCreateInfo*       pColorBlendState       = nullptr;
        const VkPipelineDynamicStateCreateInfo*          pDynamicState          = nullptr;
        VkPipelineLayout                                 layout                 = VK_NULL_HANDLE;
        VkRenderPass                                     renderPass             = VK_NULL_HANDLE;
        uint32_t                                         subpass                = 0u;
        VkPipeline                                       basePipelineHandle     = VK_NULL_HANDLE;
        int32_t                                          basePipelineIndex      = 0;

        VkGraphicsPipelineCreateInfo& setStages(const std::vector<VkPipelineShaderStageCreateInfo>& V = {}) { pStages = V.data(); stageCount = V.size(); return *this; };

        STRUCT_OPERATORS(VkGraphicsPipelineCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkGraphicsPipelineCreateInfo,vk::GraphicsPipelineCreateInfo)
    } VkGraphicsPipelineCreateInfo;

    // Vulkan 1.3
    typedef struct VkPhysicalDeviceSubgroupSizeControlPropertiesEXT {
        VkStructureType            sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES_EXT;
        void*                      pNext = nullptr;
        uint32_t                   minSubgroupSize = 0u;//16u;
        uint32_t                   maxSubgroupSize = 0u;//64u;
        uint32_t                   maxComputeWorkgroupSubgroups = 0u;
        VkShaderStageFlags         requiredSubgroupSizeStages = {};

        STRUCT_OPERATORS(VkPhysicalDeviceSubgroupSizeControlPropertiesEXT)
        VK_HPP_STRUCT_OPERATORS(VkPhysicalDeviceSubgroupSizeControlPropertiesEXT,vk::PhysicalDeviceSubgroupSizeControlPropertiesEXT)
    } VkPhysicalDeviceSubgroupSizeControlPropertiesEXT;

    // Vulkan 1.3
    typedef struct VkPipelineShaderStageRequiredSubgroupSizeCreateInfoEXT {
        VkStructureType         sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO_EXT;
        void*                   pNext = nullptr;
        uint32_t                requiredSubgroupSize = 32u; // RDNA 2

        STRUCT_OPERATORS(VkPipelineShaderStageRequiredSubgroupSizeCreateInfoEXT)
        VK_HPP_STRUCT_OPERATORS(VkPipelineShaderStageRequiredSubgroupSizeCreateInfoEXT,vk::PipelineShaderStageRequiredSubgroupSizeCreateInfoEXT)
    } VkPipelineShaderStageRequiredSubgroupSizeCreateInfoEXT;

    // 
    typedef struct VkComputePipelineCreateInfo {
        VkStructureType                    sType                = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        const void*                        pNext                = nullptr;
        VkPipelineCreateFlags              flags                = {}; // TODO: FLAGS
        VkPipelineShaderStageCreateInfo    stage                = {};
        VkPipelineLayout                   layout               = VK_NULL_HANDLE;
        VkPipeline                         basePipelineHandle   = VK_NULL_HANDLE;
        int32_t                            basePipelineIndex    = 0;

        STRUCT_OPERATORS(VkComputePipelineCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkComputePipelineCreateInfo,vk::ComputePipelineCreateInfo)
    } VkComputePipelineCreateInfo;

    // 
    typedef struct VkRayTracingShaderGroupCreateInfoNV {
        VkStructureType                  sType              = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_NV;
        const void*                      pNext              = nullptr;
        VkRayTracingShaderGroupTypeNV    type               = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_NV;
        uint32_t                         generalShader      = VK_SHADER_UNUSED_NV;
        uint32_t                         closestHitShader   = VK_SHADER_UNUSED_NV;
        uint32_t                         anyHitShader       = VK_SHADER_UNUSED_NV;
        uint32_t                         intersectionShader = VK_SHADER_UNUSED_NV;

        STRUCT_OPERATORS(VkRayTracingShaderGroupCreateInfoNV)
        VK_HPP_STRUCT_OPERATORS(VkRayTracingShaderGroupCreateInfoNV,vk::RayTracingShaderGroupCreateInfoNV)
    } VkRayTracingShaderGroupCreateInfoNV;

    //
    typedef struct VkRayTracingPipelineCreateInfoNV {
        VkStructureType                               sType                 = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_NV;
        const void*                                   pNext                 = nullptr;
        VkPipelineCreateFlags                         flags                 = {}; // TODO: FLAGS
        uint32_t                                      stageCount            = 1u;
        const VkPipelineShaderStageCreateInfo*        pStages               = nullptr;
        uint32_t                                      groupCount            = 1u;
        const VkRayTracingShaderGroupCreateInfoNV*    pGroups               = nullptr;
        uint32_t                                      maxRecursionDepth     = 1u;
        VkPipelineLayout                              layout                = VK_NULL_HANDLE;
        VkPipeline                                    basePipelineHandle    = VK_NULL_HANDLE;
        int32_t                                       basePipelineIndex     = 0;

        VkRayTracingPipelineCreateInfoNV& setStages(const std::vector<VkPipelineShaderStageCreateInfo>& V = {}) { pStages = V.data(); stageCount = V.size(); return *this; };
        VkRayTracingPipelineCreateInfoNV& setGroups(const std::vector<VkRayTracingShaderGroupCreateInfoNV>& V = {}) { pGroups = V.data(); groupCount = V.size(); return *this; };

        STRUCT_OPERATORS(VkRayTracingPipelineCreateInfoNV)
        VK_HPP_STRUCT_OPERATORS(VkRayTracingPipelineCreateInfoNV,vk::RayTracingPipelineCreateInfoNV)
    } VkRayTracingPipelineCreateInfoNV;

    //
    typedef struct VkDescriptorSetLayoutBinding {
        uint32_t              binding               = 0u;
        VkDescriptorType      descriptorType        = VK_DESCRIPTOR_TYPE_SAMPLER;
        uint32_t              descriptorCount       = 1u;
        VkShaderStageFlags    stageFlags            = { .eCompute = 1 };
        const VkSampler*      pImmutableSamplers    = nullptr;

        STRUCT_OPERATORS(VkDescriptorSetLayoutBinding)
        VK_HPP_STRUCT_OPERATORS(VkDescriptorSetLayoutBinding,vk::DescriptorSetLayoutBinding)
    } VkDescriptorSetLayoutBinding;

    // 
    typedef struct VkPushConstantRange {
        VkShaderStageFlags    stageFlags = {};
        uint32_t              offset     = 0u;
        uint32_t              size       = 16u;

        STRUCT_OPERATORS(VkPushConstantRange)
        VK_HPP_STRUCT_OPERATORS(VkPushConstantRange, vk::PushConstantRange)
    } VkPushConstantRange;

    // 
    typedef struct VkPipelineLayoutCreateInfo {
        VkStructureType                 sType                   = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        const void*                     pNext                   = nullptr;
        VkPipelineLayoutCreateFlags     flags                   = {};
        uint32_t                        setLayoutCount          = 0u;
        const VkDescriptorSetLayout*    pSetLayouts             = nullptr;
        uint32_t                        pushConstantRangeCount  = 0u;
        const VkPushConstantRange*      pPushConstantRanges     = nullptr; // TODO: NATIVE
        VkPipelineLayoutCreateInfo& setSetLayouts(const std::vector<VkDescriptorSetLayout>& V = {}) { pSetLayouts = V.data(); setLayoutCount = V.size(); return *this; };
        VkPipelineLayoutCreateInfo& setPushConstantRanges(const std::vector<VkPushConstantRange>& V = {}) { pPushConstantRanges = V.data(); pushConstantRangeCount = V.size(); return *this; };

        STRUCT_OPERATORS(VkPipelineLayoutCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkPipelineLayoutCreateInfo,vk::PipelineLayoutCreateInfo)
    } VkPipelineLayoutCreateInfo;

    // 
    typedef struct VkDescriptorSetLayoutBindingFlagsCreateInfo {
        VkStructureType                       sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
        const void*                           pNext         = nullptr;
        uint32_t                              bindingCount  = 0u;
        const VkDescriptorBindingFlags*       pBindingFlags = {};

        VkDescriptorSetLayoutBindingFlagsCreateInfo& setBindingFlags(const std::vector<VkDescriptorBindingFlags>& V = {}) { pBindingFlags = V.data(); bindingCount = V.size(); return *this; };

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

        STRUCT_OPERATORS(VkDescriptorUpdateTemplateEntry)
        VK_HPP_STRUCT_OPERATORS(VkDescriptorUpdateTemplateEntry,vk::DescriptorUpdateTemplateEntry)
    } VkDescriptorUpdateTemplateEntry;

    // 
    typedef struct VkDescriptorUpdateTemplateCreateInfo {
        VkStructureType                           sType                         = VK_STRUCTURE_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_CREATE_INFO;
        const void*                               pNext                         = nullptr;
        VkDescriptorUpdateTemplateCreateFlags     flags                         = {}; // TODO: FLAGS
        uint32_t                                  descriptorUpdateEntryCount    = 0u;
        const VkDescriptorUpdateTemplateEntry*    pDescriptorUpdateEntries      = nullptr;
        VkDescriptorUpdateTemplateType            templateType                  = VK_DESCRIPTOR_UPDATE_TEMPLATE_TYPE_DESCRIPTOR_SET;
        VkDescriptorSetLayout                     descriptorSetLayout           = VK_NULL_HANDLE;
        VkPipelineBindPoint                       pipelineBindPoint             = VK_PIPELINE_BIND_POINT_COMPUTE;
        VkPipelineLayout                          pipelineLayout                = VK_NULL_HANDLE;
        uint32_t                                  set                           = 0u;

        VkDescriptorUpdateTemplateCreateInfo& setDescriptorUpdateEntries(const std::vector<VkDescriptorUpdateTemplateEntry>& V = {}) { pDescriptorUpdateEntries = V.data(); descriptorUpdateEntryCount = V.size(); return *this; };

        STRUCT_OPERATORS(VkDescriptorUpdateTemplateCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkDescriptorUpdateTemplateCreateInfo,vk::DescriptorUpdateTemplateCreateInfo)
    } VkDescriptorUpdateTemplateCreateInfo;

    // 
    typedef struct VkDescriptorSetLayoutCreateInfo {
        VkStructureType                        sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        const void*                            pNext        = nullptr;
        VkDescriptorSetLayoutCreateFlags       flags        = {};
        uint32_t                               bindingCount = 0u;
        const VkDescriptorSetLayoutBinding*    pBindings    = nullptr;

        VkDescriptorSetLayoutCreateInfo& setBindings(const std::vector<VkDescriptorSetLayoutBinding>& V = {}) { pBindings = V.data(); bindingCount = V.size(); return *this; };

        STRUCT_OPERATORS(VkDescriptorSetLayoutCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkDescriptorSetLayoutCreateInfo,vk::DescriptorSetLayoutCreateInfo)
    } VkDescriptorSetLayoutCreateInfo;

    // 
    typedef struct VkAttachmentDescription {
        VkAttachmentDescriptionFlags    flags           = {}; // TODO: FLAGS
        VkFormat                        format          = VK_FORMAT_R8G8B8A8_UNORM;
        VkSampleCountFlagBits           samples         = VK_SAMPLE_COUNT_1_BIT;
        VkAttachmentLoadOp              loadOp          = VK_ATTACHMENT_LOAD_OP_CLEAR;
        VkAttachmentStoreOp             storeOp         = VK_ATTACHMENT_STORE_OP_STORE;
        VkAttachmentLoadOp              stencilLoadOp   = VK_ATTACHMENT_LOAD_OP_CLEAR;
        VkAttachmentStoreOp             stencilStoreOp  = VK_ATTACHMENT_STORE_OP_STORE;
        VkImageLayout                   initialLayout   = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageLayout                   finalLayout     = VK_IMAGE_LAYOUT_UNDEFINED;

        STRUCT_OPERATORS(VkAttachmentDescription)
        VK_HPP_STRUCT_OPERATORS(VkAttachmentDescription,vk::AttachmentDescription)
    } VkAttachmentDescription;

    // 
    typedef struct VkSubpassDescription {
        VkSubpassDescriptionFlags       flags                   = {}; // TODO: FLAGS
        VkPipelineBindPoint             pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        uint32_t                        inputAttachmentCount    = 0u;
        const VkAttachmentReference*    pInputAttachments       = nullptr; // TODO: NATIVE
        uint32_t                        colorAttachmentCount    = 0u;
        const VkAttachmentReference*    pColorAttachments       = nullptr; // TODO: NATIVE
        const VkAttachmentReference*    pResolveAttachments     = nullptr; // TODO: NATIVE
        const VkAttachmentReference*    pDepthStencilAttachment = nullptr; // TODO: NATIVE
        uint32_t                        preserveAttachmentCount = 0u;
        const uint32_t*                 pPreserveAttachments    = nullptr;

        VkSubpassDescription& setColorAttachments(const std::vector<VkAttachmentReference>& V = {}) { pColorAttachments = V.data(); colorAttachmentCount = V.size(); return *this; };
        VkSubpassDescription& setInputAttachments(const std::vector<VkAttachmentReference>& V = {}) { pInputAttachments = V.data(); inputAttachmentCount = V.size(); return *this; };
        VkSubpassDescription& setPreserveAttachments(const std::vector<uint32_t>& V = {}) { pPreserveAttachments = V.data(); preserveAttachmentCount = V.size(); return *this; };

        STRUCT_OPERATORS(VkSubpassDescription)
        VK_HPP_STRUCT_OPERATORS(VkSubpassDescription,vk::SubpassDescription)
    } VkSubpassDescription;

    // 
    typedef struct VkSubpassDependency {
        uint32_t                srcSubpass      = 0u;
        uint32_t                dstSubpass      = 0u;
        VkPipelineStageFlags    srcStageMask    = {};
        VkPipelineStageFlags    dstStageMask    = {};
        VkAccessFlags           srcAccessMask   = {};
        VkAccessFlags           dstAccessMask   = {};
        VkDependencyFlags       dependencyFlags = {};

        STRUCT_OPERATORS(VkSubpassDependency)
        VK_HPP_STRUCT_OPERATORS(VkSubpassDependency,vk::SubpassDependency)
    } VkSubpassDependency;

    // 
    typedef struct VkRenderPassCreateInfo {
        VkStructureType                   sType             = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        const void*                       pNext             = nullptr;
        VkRenderPassCreateFlags           flags             = {}; // TODO: FLAGS
        uint32_t                          attachmentCount   = 0u;
        const VkAttachmentDescription*    pAttachments      = nullptr;
        uint32_t                          subpassCount      = 0u;
        const VkSubpassDescription*       pSubpasses        = nullptr;
        uint32_t                          dependencyCount   = 0u;
        const VkSubpassDependency*        pDependencies     = nullptr;

        VkRenderPassCreateInfo& setAttachments(const std::vector<VkAttachmentDescription>& V = {}) { pAttachments = V.data(); attachmentCount = V.size(); return *this; };
        VkRenderPassCreateInfo& setSubpasses(const std::vector<VkSubpassDescription>& V = {}) { pSubpasses = V.data(); subpassCount = V.size(); return *this; };
        VkRenderPassCreateInfo& setDependencies(const std::vector<VkSubpassDependency>& V = {}) { pDependencies = V.data(); dependencyCount = V.size(); return *this; };

        STRUCT_OPERATORS(VkRenderPassCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkRenderPassCreateInfo,vk::RenderPassCreateInfo)
    } VkRenderPassCreateInfo;

    // 
    typedef struct VkFramebufferCreateInfo {
        VkStructureType             sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        const void*                 pNext           = nullptr;
        VkFramebufferCreateFlags    flags           = {}; // TODO: FLAGS
        VkRenderPass                renderPass      = VK_NULL_HANDLE;
        uint32_t                    attachmentCount = 0u;
        const VkImageView*          pAttachments    = nullptr;
        uint32_t                    width           = 1u;
        uint32_t                    height          = 1u;
        uint32_t                    layers          = 1u;

        VkFramebufferCreateInfo& setAttachments(const std::vector<VkImageView>& V = {}) { pAttachments = V.data(); attachmentCount = V.size(); return *this; };

        STRUCT_OPERATORS(VkFramebufferCreateInfo)
        VK_HPP_STRUCT_OPERATORS(VkFramebufferCreateInfo,vk::FramebufferCreateInfo)
    } VkFramebufferCreateInfo;

    // 
    typedef struct VkAccelerationStructureMemoryRequirementsInfoNV {
        VkStructureType                                    sType                    = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_NV;
        const void*                                        pNext                    = nullptr;
        VkAccelerationStructureMemoryRequirementsTypeNV    type                     = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_OBJECT_NV;
        VkAccelerationStructureNV                          accelerationStructure    = VK_NULL_HANDLE;

        STRUCT_OPERATORS(VkAccelerationStructureMemoryRequirementsInfoNV)
        VK_HPP_STRUCT_OPERATORS(VkAccelerationStructureMemoryRequirementsInfoNV,vk::AccelerationStructureMemoryRequirementsInfoNV)
    } VkAccelerationStructureMemoryRequirementsInfoNV;

    // 
    typedef struct VkGeometryAABBNV {
        VkStructureType    sType    = VK_STRUCTURE_TYPE_GEOMETRY_AABB_NV;
        const void*        pNext    = nullptr;
        VkBuffer           aabbData = VK_NULL_HANDLE;
        uint32_t           numAABBs = 0u;
        uint32_t           stride   = 24u;
        VkDeviceSize       offset   = 0u;

        STRUCT_OPERATORS(VkGeometryAABBNV)
        VK_HPP_STRUCT_OPERATORS(VkGeometryAABBNV,vk::GeometryAABBNV)
    } VkGeometryAABBNV;

    // 
    typedef struct VkGeometryTrianglesNV {
        VkStructureType    sType            = VK_STRUCTURE_TYPE_GEOMETRY_TRIANGLES_NV;
        const void*        pNext            = nullptr;
        VkBuffer           vertexData       = VK_NULL_HANDLE;
        VkDeviceSize       vertexOffset     = 0u;
        uint32_t           vertexCount      = 1u;
        VkDeviceSize       vertexStride     = 16u;
        VkFormat           vertexFormat     = VK_FORMAT_R32G32B32_SFLOAT;
        VkBuffer           indexData        = VK_NULL_HANDLE;
        VkDeviceSize       indexOffset      = 0u;
        uint32_t           indexCount       = 0u;
        VkIndexType        indexType        = VK_INDEX_TYPE_NONE_NV;
        VkBuffer           transformData    = VK_NULL_HANDLE;
        VkDeviceSize       transformOffset  = 0u;

        STRUCT_OPERATORS(VkGeometryTrianglesNV)
        VK_HPP_STRUCT_OPERATORS(VkGeometryTrianglesNV,vk::GeometryTrianglesNV)
    } VkGeometryTrianglesNV;

    // 
    typedef struct VkGeometryDataNV {
        VkGeometryTrianglesNV    triangles = {};
        VkGeometryAABBNV         aabbs     = {};

        STRUCT_OPERATORS(VkGeometryDataNV)
        VK_HPP_STRUCT_OPERATORS(VkGeometryDataNV,vk::GeometryDataNV)
    } VkGeometryDataNV;

    // 
    typedef struct VkGeometryNV {
        VkStructureType      sType        = VK_STRUCTURE_TYPE_GEOMETRY_NV;
        const void*          pNext        = nullptr;
        VkGeometryTypeNV     geometryType = VK_GEOMETRY_TYPE_TRIANGLES_NV;
        VkGeometryDataNV     geometry     = {};
        VkGeometryFlagsNV    flags        = {};

        STRUCT_OPERATORS(VkGeometryNV)
        VK_HPP_STRUCT_OPERATORS(VkGeometryNV,vk::GeometryNV)
    } VkGeometryNV;

    // 
    typedef struct VkAccelerationStructureInfoNV {
        VkStructureType                        sType         = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
        const void*                            pNext         = nullptr;
        VkAccelerationStructureTypeNV          type          = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_NV;
        VkBuildAccelerationStructureFlagsNV    flags         = {};
        uint32_t                               instanceCount = 0u;
        uint32_t                               geometryCount = 0u;
        const VkGeometryNV*                    pGeometries   = nullptr;

        VkAccelerationStructureInfoNV& setGeometries(const std::vector<VkGeometryNV>& V = {}) { pGeometries = V.data(); geometryCount = V.size(); return *this; };

        STRUCT_OPERATORS(VkAccelerationStructureInfoNV)
        VK_HPP_STRUCT_OPERATORS(VkAccelerationStructureInfoNV,vk::AccelerationStructureInfoNV)
    } VkAccelerationStructureInfoNV;

    // 
    typedef struct VkAccelerationStructureCreateInfoNV {
        VkStructureType                  sType           = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_NV;
        const void*                      pNext           = nullptr;
        VkDeviceSize                     compactedSize   = 0u;
        VkAccelerationStructureInfoNV    info            = {};

        STRUCT_OPERATORS(VkAccelerationStructureCreateInfoNV)
        VK_HPP_STRUCT_OPERATORS(VkAccelerationStructureCreateInfoNV,vk::AccelerationStructureCreateInfoNV)
    } VkAccelerationStructureCreateInfoNV;

    // 
    typedef struct VkBindAccelerationStructureMemoryInfoNV {
        VkStructureType              sType                  = VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_NV;
        const void*                  pNext                  = nullptr;
        VkAccelerationStructureNV    accelerationStructure  = VK_NULL_HANDLE;
        VkDeviceMemory               memory                 = VK_NULL_HANDLE;
        VkDeviceSize                 memoryOffset           = 0ull;
        uint32_t                     deviceIndexCount       = 0u;
        const uint32_t*              pDeviceIndices         = nullptr;

        VkBindAccelerationStructureMemoryInfoNV& setDeviceIndices(const std::vector<uint32_t>& V = {}) { pDeviceIndices = V.data(); deviceIndexCount = V.size(); return *this; };

        STRUCT_OPERATORS(VkBindAccelerationStructureMemoryInfoNV)
        VK_HPP_STRUCT_OPERATORS(VkBindAccelerationStructureMemoryInfoNV,vk::BindAccelerationStructureMemoryInfoNV)
    } VkBindAccelerationStructureMemoryInfoNV;

    // 
    typedef struct VkMemoryBarrier {
        VkStructureType     sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        const void*         pNext         = nullptr;
        VkAccessFlags       srcAccessMask = {};
        VkAccessFlags       dstAccessMask = {};

        STRUCT_OPERATORS(VkMemoryBarrier)
        VK_HPP_STRUCT_OPERATORS(VkMemoryBarrier, vk::MemoryBarrier)
    } VkMemoryBarrier;

    //
    typedef struct VkMemoryType {
        VkMemoryPropertyFlags    propertyFlags = {};
        uint32_t                 heapIndex = 0u;

        STRUCT_OPERATORS(VkMemoryType)
        VK_HPP_STRUCT_OPERATORS(VkMemoryType, vk::MemoryType)
    } VkMemoryType;

    // 
    typedef struct VkMemoryHeap {
        VkDeviceSize      size  = 0u;
        VkMemoryHeapFlags flags = {};

        STRUCT_OPERATORS(VkMemoryHeap)
        VK_HPP_STRUCT_OPERATORS(VkMemoryHeap, vk::MemoryHeap)
    } VkMemoryHeap;

    // 
    typedef struct VkPhysicalDeviceMemoryProperties {
        uint32_t     memoryTypeCount                  = 0u;
        VkMemoryType memoryTypes[VK_MAX_MEMORY_TYPES] = {};
        uint32_t     memoryHeapCount                  = 0u;
        VkMemoryHeap memoryHeaps[VK_MAX_MEMORY_HEAPS] = {};

        STRUCT_OPERATORS(VkPhysicalDeviceMemoryProperties)
        VK_HPP_STRUCT_OPERATORS(VkPhysicalDeviceMemoryProperties, vk::PhysicalDeviceMemoryProperties)
    } VkPhysicalDeviceMemoryProperties;


#pragma pack(pop)

#pragma pack(push, 1)
    struct uint24__ { // for compatibility only
        uint32_t a : 24;
        operator const uint32_t&() const {return a; };
        uint24__(const uint32_t& a) : a(a) {};
        uint24__(const uint24__& a) : a(a.a) {};
        uint24__& operator=(const uint32_t& a) { this->a = a; return *this; };
        uint24__& operator=(const uint24__& a) { this->a = a.a; return *this; };
    };

    struct uint24_t { // for compatibility only
        uint8_t a[3u] = {0u,0u,0u};
        uint24_t(const uint24__& a) { memcpy(this, &a, 3u); };
        uint24_t(const uint24_t& a) { memcpy(this, &a, 3u); };
        uint24_t(const uint32_t& a) { memcpy(this, &a, 3u); };
        operator uint32_t() const { return reinterpret_cast<const uint24__&>(a); };
        uint24_t& operator=(const uint32_t& a) { memcpy(this, &a, 3u); return *this; };
        uint24_t& operator=(const uint24__& a) { memcpy(this, &a, 3u); return *this; };
        uint24_t& operator=(const uint24_t& a) { memcpy(this, &a, 3u); return *this; };
    };

    // CODING EXTRAS...
    #pragma pack(push, 1)
    struct VsGeometryInstance {
        glm::mat3x4 transform = {
            glm::vec4(1.f,0.f,0.f,0.f),
            glm::vec4(0.f,1.f,0.f,0.f),
            glm::vec4(0.f,0.f,1.f,0.f)
        };
        uint32_t instanceId : 24;
        uint32_t mask : 8;
        uint32_t instanceOffset : 24;
        uint32_t flags : 8;
        //uint24_t instanceId = 0u;
        //uint8_t mask = 0xFF;
        //uint24_t instanceOffset = 0u;
        //VkGeometryInstanceFlagsNV flags = {};
        uint64_t accelerationStructureHandle = 0ull;

        // Few Operators
        operator VsGeometryInstance* () { return this; };
        operator VsGeometryInstance& () { return *this; };
        operator const VsGeometryInstance* () const { return this; };
        operator const VsGeometryInstance& () const { return *this; };
        VsGeometryInstance& operator =(const VsGeometryInstance& info) { memcpy(this,&info,sizeof(VsGeometryInstance)); return *this; };
    };
    #pragma pack(pop)

};
