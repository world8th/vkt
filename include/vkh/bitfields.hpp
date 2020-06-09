#pragma once // #

// 
#include <memory>
#include <vector>
#include <glm/glm.hpp>

/*
// Enable Vulkan-HPP when defined
#ifdef VULKAN_HPP
#define ENABLE_VULKAN_HPP
#endif

//
#include <vulkan/vulkan.h>

// When enabled, use Vulkan-HPP support...
#ifdef ENABLE_VULKAN_HPP
#include <vulkan/vulkan.hpp>
#endif

// 
#include <xvk/xvk.hpp>
*/

#include <vkh/core.hpp>

// 
namespace vkh {

    #pragma pack(push, 4) // Mostly, should be uint32_t

    // 
    #define OPERATORS(NAME,BITS,COMP) \
        NAME& operator=(const vkt::uni_arg<NAME>& F) { memcpy(this, F, sizeof(NAME)); return *this; };\
        NAME& operator=(const vkt::uni_arg<BITS>& F) { memcpy(this, F, sizeof(NAME)); return *this; };\
        NAME operator|(const NAME& F) { auto f = COMP(F) | COMP(*this); return std::move(reinterpret_cast<NAME&>(f)); };\
        NAME operator&(const NAME& F) { auto f = COMP(F) & COMP(*this); return std::move(reinterpret_cast<NAME&>(f)); };\
        NAME operator^(const NAME& F) { auto f = COMP(F) ^ COMP(*this); return std::move(reinterpret_cast<NAME&>(f)); };\
        NAME operator|(const BITS& F) { auto f = COMP(F) | COMP(*this); return std::move(reinterpret_cast<NAME&>(f)); };\
        NAME operator&(const BITS& F) { auto f = COMP(F) & COMP(*this); return std::move(reinterpret_cast<NAME&>(f)); };\
        NAME operator^(const BITS& F) { auto f = COMP(F) ^ COMP(*this); return std::move(reinterpret_cast<NAME&>(f)); };\
        NAME operator~() { auto f = ~COMP(*this); return reinterpret_cast<NAME&>(f); };\
        BITS& c() { return reinterpret_cast<BITS&>(*this); }; \
        const BITS& c() const { return reinterpret_cast<const BITS&>(*this); }; \
        operator BITS&() {return reinterpret_cast<BITS&>(*this);};\
        operator const BITS&() const {return reinterpret_cast<const BITS&>(*this);};\
        explicit operator COMP& () { return reinterpret_cast<COMP&>(*this); };\
        explicit operator const COMP& () const { return reinterpret_cast<const COMP&>(*this); };\
        NAME& also(const std::function<NAME&(NAME&)>& fn) { return fn(*this); }; // Alternative of Vulkan.HPP

    //NAME& operator=(const vkt::uni_arg<COMP>& F) { memcpy(this, F, sizeof(NAME)); return *this; };\

    //
#ifdef VULKAN_HPP
    #define VK_HPP_OPERATORS(NAME,FTYP,BITS) \
        inline static FTYP cpp() { const auto data = NAME{}; return reinterpret_cast<const FTYP&>(data); };\
        NAME& operator=(const vkt::uni_arg<BITS>& F) { memcpy(this, F, sizeof(NAME)); return *this; };\
        NAME operator|(const BITS& F) { auto f = F | reinterpret_cast<BITS&>(*this); return std::move(reinterpret_cast<NAME&>(f)); };\
        NAME operator&(const BITS& F) { auto f = F & reinterpret_cast<BITS&>(*this); return std::move(reinterpret_cast<NAME&>(f)); };\
        NAME operator^(const BITS& F) { auto f = F ^ reinterpret_cast<BITS&>(*this); return std::move(reinterpret_cast<NAME&>(f)); };\
        NAME& operator=(const vkt::uni_arg<FTYP>& F) { memcpy(this, F, sizeof(NAME)); return *this; };\
        NAME operator|(const FTYP& F) { auto f = F | reinterpret_cast<FTYP&>(*this); return std::move(reinterpret_cast<NAME&>(f)); };\
        NAME operator&(const FTYP& F) { auto f = F & reinterpret_cast<FTYP&>(*this); return std::move(reinterpret_cast<NAME&>(f)); };\
        NAME operator^(const FTYP& F) { auto f = F ^ reinterpret_cast<FTYP&>(*this); return std::move(reinterpret_cast<NAME&>(f)); };\
        operator BITS&() {return reinterpret_cast<BITS&>(*this);};\
        operator const BITS&() const {return reinterpret_cast<const BITS&>(*this);};\
        operator FTYP&() {return reinterpret_cast<FTYP&>(*this);};\
        operator const FTYP&() const {return reinterpret_cast<const FTYP&>(*this);};\
        FTYP& hpp() {return reinterpret_cast<FTYP&>(*this);};\
        const FTYP& hpp() const {return reinterpret_cast<const FTYP&>(*this);};
#else 
    #define VK_HPP_OPERATORS(NAME,FTYP,BITS) // Not Vulkan HPP Support
#endif


    struct VkBufferCreateFlags { ::VkFlags
        eSparseBinding : 1,
        eResidency : 1,
        eAlised : 1,
        eProtected : 1,
        eDeviceAddressCaptureReplay : 1;

        OPERATORS(VkBufferCreateFlags, ::VkBufferCreateFlagBits,::VkFlags)
        VK_HPP_OPERATORS(VkBufferCreateFlags,vk::BufferCreateFlags,vk::BufferCreateFlagBits)
    };

    // 
    struct VkBufferUsageFlags { ::VkFlags
        eTransferSrc : 1,
        eTransferDst : 1,
        eUniformTexelBuffer : 1,
        eStorageTexelBuffer : 1,
        eUniformBuffer : 1,
        eStorageBuffer : 1,
        eIndexBuffer : 1,
        eVertexBuffer : 1,
        eIndirectBuffer : 1,
        eConditionalRendering : 1,
        eRayTracing : 1,
        eTransformFeedbackBuffer : 1,
        eTransformFeedbackCounterBuffer : 1,
        eUnknown:4,
        eSharedDeviceAddress : 1;

        OPERATORS(VkBufferUsageFlags, ::VkBufferUsageFlagBits,::VkFlags)
        VK_HPP_OPERATORS(VkBufferUsageFlags,vk::BufferUsageFlags,vk::BufferUsageFlagBits)
    };

    // 
    struct VkImageAspectFlags { ::VkFlags
        eColor : 1,
        eDepth : 1,
        eStencil : 1,
        eMetaData : 1,
        ePlane0 : 1,
        ePlane1 : 1,
        ePlane2 : 1,
        eMemoryPlane0 : 1,
        eMemoryPlane1 : 1,
        eMemoryPlane2 : 1;

        OPERATORS(VkImageAspectFlags, ::VkImageAspectFlagBits,::VkFlags)
        VK_HPP_OPERATORS(VkImageAspectFlags,vk::ImageAspectFlags,vk::ImageAspectFlagBits)
    };

    // 
    struct VkSampleCountFlags {
        ::VkFlags
            e1 : 1,
            e2 : 1,
            e4 : 1,
            e8 : 1,
            e16 : 1,
            e32 : 1,
            e64 : 1;

        OPERATORS(VkSampleCountFlags, ::VkSampleCountFlagBits, ::VkFlags)
        VK_HPP_OPERATORS(VkSampleCountFlags,vk::SampleCountFlags,vk::SampleCountFlagBits)
    };

    // 
    struct VkImageCreateFlags { ::VkFlags
        eSparseBinding:1,
        eSparseResidency:1,
        eSparseAliased:1,
        eMutableFormat:1,
        eCubeCompatible:1,
        e2DArrayCompatible:1,
        eBlockTexelViewCompatible:1,
        eExtendedUsage:1,
        eDisjoint:1,
        eAlias:1,
        eProtected:1,
        eSampleLocationsCompatibleDepth:1,
        eCornerSampled:1,
        eSubsampled:1;
        
        OPERATORS(VkImageCreateFlags, ::VkImageCreateFlagBits,::VkFlags)
        VK_HPP_OPERATORS(VkImageCreateFlags,vk::ImageCreateFlags,vk::ImageCreateFlagBits)
    };

    // 
    struct VkImageUsageFlags { ::VkFlags
        eTransferSrc:1,
        eTransferDst:1,
        eSampled:1,
        eStorage:1,
        eColorAttachment:1,
        eDepthStencilAttachment:1,
        eTransientAttachment:1,
        eInputAttachment:1,
        eShadingRateImage:1,
        eFragmentDensityMap:1;

        OPERATORS(VkImageUsageFlags, ::VkImageUsageFlagBits,::VkFlags)
        VK_HPP_OPERATORS(VkImageUsageFlags,vk::ImageUsageFlags,vk::ImageUsageFlagBits)
    };

    // 
    struct VkPipelineStageFlags { ::VkFlags
        eTopOfPipe:1,
        eDrawIndirect:1,
        eVertexInput:1,
        eVertexShader:1,
        eTessellationControlShader:1,
        eTessellationEvaluationShader:1,
        eGeometryShader:1,
        eFragmentShader:1,
        eEarlyFragmentTests:1,
        eLateFragmentTests:1,
        eColorAttachmentOutput:1,
        eComputeShader:1,
        eTransfer:1,
        eBottomOfPipe:1,
        eHost:1,
        eAllGraphics:1,
        eAllCommands:1,
        eCommandProcess:1,
        eConditionalRendering:1,
        eTaskShader:1,
        eMeshShader:1,
#define eRayTracing eRayTracingShader // Alias
        eRayTracingShader:1,
#undef  eRayTracing // Do not conflict!
        eShadingRateImage:1,
        eFragmentDensityProcess:1,
        eTransformFeedback:1,
        eAccelerationStructureBuild:1;

        OPERATORS(VkPipelineStageFlags,::VkPipelineStageFlagBits,::VkFlags)
        VK_HPP_OPERATORS(VkPipelineStageFlags,vk::PipelineStageFlags,vk::PipelineStageFlagBits)
    };

    // 
    struct VkShaderStageFlags { ::VkFlags
        eVertex:1,
        eTessellationControl:1,
        eTessellationEvaluation:1,
        eGeometry:1,
        eFragment:1,
        eCompute:1,
        eTask:1,
        eMesh:1,
        eRaygen:1,
        eAnyHit:1,
        eClosestHit:1,
        eMiss:1,
        eIntersection:1,
        eCallable:1;

        OPERATORS(VkShaderStageFlags,::VkShaderStageFlagBits,::VkFlags)
        VK_HPP_OPERATORS(VkShaderStageFlags,vk::ShaderStageFlags,vk::ShaderStageFlagBits)
    };

    // 
    struct VkCullModeFlags { ::VkFlags
        eFront:1,
        eBack:1;

        OPERATORS(VkCullModeFlags, ::VkCullModeFlagBits,::VkFlags)
        VK_HPP_OPERATORS(VkCullModeFlags,vk::CullModeFlags,vk::CullModeFlagBits)
    };

    // 
    struct VkFormatFeatureFlags { ::VkFlags
        eSampledImage:1,
        eStorageImage:1,
        eStorageImageAtomic:1,
        eUniformTexelBuffer:1,
        eStorageTexelBuffer:1,
        eStorageTexelBufferAtomic:1,
        eVertexBuffer:1,
        eColorAttachment:1,
        eColorAttachmentBlend:1,
        eDepthStencilAttachment:1,
        eBlitSrc:1,
        eBlitDst:1,
        eSampledImageFilterLinear:1,
        eSampledImageFilterCubic:1,
        eTransferSrc:1,
        eTransferDst:1,
        eSampledImageFilterMinmax:1,
        eMidpointChromaSamples:1,
        eSampledImageYcbcrConversionLinearFilter:1,
        eSampledImageYcbcrConversionSeparateReconstructionFilter:1,
        eSampledImageYcbcrConversionChromaReconstructionExplicit:1,
        eSampledImageYcbcrConversionChromaReconstructionExplicitForceable:1,
        eDisjoint:1,
        eCositedChromaSamples:1,
        eFragmentDensityMap:1;

        OPERATORS(VkFormatFeatureFlags, ::VkFormatFeatureFlagBits,::VkFlags)
        VK_HPP_OPERATORS(VkFormatFeatureFlags,vk::FormatFeatureFlags,vk::FormatFeatureFlagBits)
    };

    // 
    struct VkColorComponentFlags { ::VkFlags
        eR:1,
        eG:1,
        eB:1,
        eA:1;
        
        OPERATORS(VkColorComponentFlags, ::VkColorComponentFlagBits,::VkFlags)
        VK_HPP_OPERATORS(VkColorComponentFlags,vk::ColorComponentFlags,vk::ColorComponentFlagBits)
    };

    // 
    struct VkDescriptorBindingFlags { ::VkFlags
        eUpdateAfterBind:1,
        eUpdateUnusedWhilePending:1,
        ePartiallyBound:1,
        eVariableDescriptorCount:1;

        OPERATORS(VkDescriptorBindingFlags, ::VkDescriptorBindingFlagBits,::VkFlags)
        VK_HPP_OPERATORS(VkDescriptorBindingFlags,vk::DescriptorBindingFlags,vk::DescriptorBindingFlagBits)
    };

    // 
    struct VkBuildAccelerationStructureFlagsKHR { ::VkFlags
        eAllowUpdate:1,
        eAllowCompaction:1,
        ePreferFastTrace:1,
        ePreferFastBuild:1,
        eLowMemory:1;
        
        OPERATORS(VkBuildAccelerationStructureFlagsKHR, ::VkBuildAccelerationStructureFlagBitsKHR,::VkFlags)
        VK_HPP_OPERATORS(VkBuildAccelerationStructureFlagsKHR,vk::BuildAccelerationStructureFlagsKHR,vk::BuildAccelerationStructureFlagBitsKHR)
    };

    // 
    using VkBuildAccelerationStructureFlagsNV = VkBuildAccelerationStructureFlagsKHR;

    // 
    struct VkGeometryFlagsKHR { ::VkFlags
        eOpaque:1,
        eNoDuplicateAnyHitInvocation:1;
        
        OPERATORS(VkGeometryFlagsKHR, ::VkGeometryFlagBitsKHR, ::VkFlags)
        VK_HPP_OPERATORS(VkGeometryFlagsKHR,vk::GeometryFlagsKHR,vk::GeometryFlagBitsKHR)
    };

    // 
    using VkGeometryFlagsNV = VkGeometryFlagsKHR;

    // 
    struct VkQueueFlags { ::VkFlags
        eGraphics:1,
        eCompute:1,
        eTransfer:1,
        eSparseBinding:1,
        eProtected:1;
        
        OPERATORS(VkQueueFlags, ::VkQueueFlagBits, ::VkFlags)
        VK_HPP_OPERATORS(VkQueueFlags,vk::QueueFlags,vk::QueueFlagBits)
    };

    // 
    struct VkSurfaceTransformFlagsKHR { ::VkFlags
        eIdentity:1,
        eRotate90:1,
        eRotate180:1,
        eRotate270:1,
        eHorizontalMirror:1,
        eHorizontalMirrorRotate90:1,
        eHorizontalMirrorRotate180:1,
        eHorizontalMirrorRotate270:1,
        eInherit:1;

        OPERATORS(VkSurfaceTransformFlagsKHR, ::VkSurfaceTransformFlagBitsKHR, ::VkFlags)
        VK_HPP_OPERATORS(VkSurfaceTransformFlagsKHR,vk::SurfaceTransformFlagsKHR,vk::SurfaceTransformFlagBitsKHR)
    };

    // 
    struct VkCompositeAlphaFlagsKHR { ::VkFlags
        eOpaque: 1,
        ePreMultiplied:1,
        ePostMultiplied:1,
        eInherit:1;

        OPERATORS(VkCompositeAlphaFlagsKHR, ::VkCompositeAlphaFlagBitsKHR, ::VkFlags)
        VK_HPP_OPERATORS(VkCompositeAlphaFlagsKHR,vk::CompositeAlphaFlagsKHR,vk::CompositeAlphaFlagBitsKHR)
    };

    // 
    struct VkDescriptorSetLayoutCreateFlags { ::VkFlags
        ePushDescriptor:1,
        eUpdateAfterBindPool:1;
        
        OPERATORS(VkDescriptorSetLayoutCreateFlags, ::VkDescriptorSetLayoutCreateFlagBits,::VkFlags)
        VK_HPP_OPERATORS(VkDescriptorSetLayoutCreateFlags,vk::DescriptorSetLayoutCreateFlags,vk::DescriptorSetLayoutCreateFlagBits)
    };

    // 
    struct VkDependencyFlags { ::VkFlags
        eByRegion:1,
        eViewLocal:1,
        eDeviceGroup:1;

        OPERATORS(VkDependencyFlags, ::VkDependencyFlagBits, ::VkFlags)
        VK_HPP_OPERATORS(VkDependencyFlags,vk::DependencyFlags,vk::DependencyFlagBits)
    };

    // 
    struct VkSubgroupFeatureFlags { ::VkFlags
        eBasic: 1,
        eVote: 1,
        eArithmetic: 1,
        eBallot: 1,
        eShuffle: 1,
        eShuffleRelative: 1,
        eClustered: 1,
        eQuad: 1,
        ePartitioned: 1;

        OPERATORS(VkSubgroupFeatureFlags, ::VkSubgroupFeatureFlagBits, ::VkFlags)
        VK_HPP_OPERATORS(VkSubgroupFeatureFlags,vk::SubgroupFeatureFlags,vk::SubgroupFeatureFlagBits)
    };

    // 
    struct VkPipelineShaderStageCreateFlags { ::VkFlags
        eAllowVaryingSubgroupSize: 1,
        eRequireFullSubgroups: 1;

        OPERATORS(VkPipelineShaderStageCreateFlags, ::VkPipelineShaderStageCreateFlagBits, ::VkFlags)
        VK_HPP_OPERATORS(VkPipelineShaderStageCreateFlags,vk::PipelineShaderStageCreateFlags,vk::PipelineShaderStageCreateFlagBits)
    };

    // 
    struct VkAccessFlags { ::VkFlags
        eIndirectCommandRead:1,
        eIndexRead:1,
        eVertexAttributeRead:1,
        eUniformRead:1,
        eInputAttachmentRead:1,
        eShaderRead:1,
        eShaderWrite:1,
        eColorAttachmentRead:1,
        eColorAttachmentWrite:1,
        eDepthStencilAttachmentRead : 1,
        eDepthStencilAttachmentWrite : 1,
        eTransferRead:1,
        eTransferWrite:1,
        eHostRead:1,
        eHostWrite:1,
        eMemoryRead:1,
        eMemoryWrite:1,
        eCommandProcessRead:1,
        eCommandProcessWrite:1,
        eColorAttachmentReadNonCoherent:1,
        eConditionalRenderingRead:1,
        eAccelerationStructureRead:1,
        eAccelerationStructureWrite:1,
        eShadingRateImageRead:1,
        eFragmentDensityMapRead:1,
        eTransformFeedbackWrite:1,
        eTransformFeedbackCounterRead:1,
        eTransformFeedbackCounterWrite:1;

        OPERATORS(VkAccessFlags, ::VkAccessFlagBits, ::VkFlags)
        VK_HPP_OPERATORS(VkAccessFlags, vk::AccessFlags, vk::AccessFlagBits)
    };

    // 
    struct VkMemoryPropertyFlags { ::VkFlags
        eDeviceLocal:1,
        eHostVisible:1,
        eHostCoherent:1,
        eHostCached:1,
        eLazilyAllocated:1,
        eProtected:1,
        eDeviceCoherent:1,
        eDeviceUncached:1;

        OPERATORS(VkMemoryPropertyFlags, ::VkMemoryPropertyFlagBits, ::VkFlags)
        VK_HPP_OPERATORS(VkMemoryPropertyFlags, vk::MemoryPropertyFlags, vk::MemoryPropertyFlagBits)
    };

    // 
    struct VkMemoryHeapFlags { ::VkFlags
        eDeviceLocal:1,
        eMultiInstance:1;

        OPERATORS(VkMemoryHeapFlags, ::VkMemoryHeapFlagBits, ::VkFlags)
        VK_HPP_OPERATORS(VkMemoryHeapFlags, vk::MemoryHeapFlags, vk::MemoryHeapFlagBits)
    };

    // Vulkan 1.2
    // Beta: VkPipelineCreateFlagBits and VkPipelineCreateFlags
    struct VkPipelineCreateFlags { ::VkFlags
        eDisableOptimization:1,
        eAllowDirevatives:1,
        eDirevative:1,
        eViewIndexFromDevice:1,
        eDispatchBase:1,
        eDeferCompile:1,
        eCaptureStatistics:1,
        eCaptureInternalRepresentations:1,
        eFailOnPipelineCompileRequired:1,
        eEarlyReturnOnFailure:1,
        eUnknown:1,
        eLibrary:1,
        eRayTracingSkipTriangles:1,
        eRayTracingSkipAABBs:1,
        eRayTracingNoNullAnyHitShaders:1,
        eRayTracingNoNullClosestHitShaders:1,
        eRayTracingNoNullMissHitShaders:1,
        eRayTracingNoNullIntersectionShaders:1,
        eIndirectBindable:1;

        OPERATORS(VkPipelineCreateFlags, ::VkPipelineCreateFlagBits, ::VkFlags)
        VK_HPP_OPERATORS(VkPipelineCreateFlags, vk::PipelineCreateFlags, vk::PipelineCreateFlagBits)
    };

    // EMPTY GET ERROR! USED STUB!
    /*struct VkShaderModuleCreateFlags {
        ::VkFlags flags = 0u;

        OPERATORS(VkShaderModuleCreateFlags, ::VkShaderModuleCreateFlagBits, ::VkFlags)
        VK_HPP_OPERATORS(VkShaderModuleCreateFlags, vk::ShaderModuleCreateFlags, vk::ShaderModuleCreateFlagBits)
    };*/

    // 
    struct VkExternalMemoryHandleTypeFlags { ::VkFlags
        eOpaqueFd:1,
        eOpaqueWin32:1,
        eOpaqueWin32Kmt:1,
        eD3D11Texture:1,
        eD3D11TextureKmt:1,
        eD3D12Heap:1,
        eD3D12Resource:1,
        eHostAllocation:1,
        eHostMappedForeignMemory:1,
        eDmaBuf:1,
        eAndroidHardwareBuffer:1;

        OPERATORS(VkExternalMemoryHandleTypeFlags, ::VkExternalMemoryHandleTypeFlagBits, ::VkFlags)
        VK_HPP_OPERATORS(VkExternalMemoryHandleTypeFlags, vk::ExternalMemoryHandleTypeFlags, vk::ExternalMemoryHandleTypeFlagBits)
    };

    // 
    struct VkExternalSemaphoreHandleTypeFlags { ::VkFlags
        eOpaqueFd : 1,
        eOpaqueWin32 : 1,
        eOpaqueWin32Kmt : 1,
        eD3D12Fence : 1,
        eSyncFd : 1;

        OPERATORS(VkExternalSemaphoreHandleTypeFlags, ::VkExternalSemaphoreHandleTypeFlagBits, ::VkFlags)
        VK_HPP_OPERATORS(VkExternalSemaphoreHandleTypeFlags, vk::ExternalSemaphoreHandleTypeFlags, vk::ExternalSemaphoreHandleTypeFlagBits)
    };

    //
    struct VkCommandPoolCreateFlags { ::VkFlags
        eTransient : 1,
        eResetCommandBuffer : 1,
        eProtected : 1;

        OPERATORS(VkCommandPoolCreateFlags, ::VkCommandPoolCreateFlagBits, ::VkFlags)
        VK_HPP_OPERATORS(VkCommandPoolCreateFlags, vk::CommandPoolCreateFlags, vk::CommandPoolCreateFlagBits)
    };

    //
    struct VkMemoryAllocateFlags { ::VkFlags
        eMask : 1,
        eAddress : 1,
        eAddressCaptureReplay : 1;

        OPERATORS(VkMemoryAllocateFlags, ::VkMemoryAllocateFlagBits, ::VkFlags)
        VK_HPP_OPERATORS(VkMemoryAllocateFlags, vk::MemoryAllocateFlags, vk::MemoryAllocateFlagBits)
    };

    


#pragma pack(pop)
    
    // Extras
#pragma pack(push, 1)
    struct VkGeometryInstanceFlagsNV { uint8_t
        eTriangleCullDisable:1,
        eTriangleFrontCounterclockwise:1,
        eForceOpaque:1,
        eForceNoOpaque:1;

        OPERATORS(VkGeometryInstanceFlagsNV, ::VkGeometryInstanceFlagBitsNV, ::uint8_t)
        VK_HPP_OPERATORS(VkGeometryInstanceFlagsNV,vk::GeometryInstanceFlagsNV,vk::GeometryInstanceFlagBitsNV)
    };
#pragma pack(pop)


};
