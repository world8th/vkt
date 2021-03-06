#pragma once // #

//#define VKH_USE_XVK
#include "./core.hpp"
#include "./structures.hpp"

// 
#include <string_view>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>

#ifdef VKT_USE_GLFW
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#endif

namespace vkh {

    // TODO: Some Vookoo-like aggregators and helpers
    // 1. [W] Ray Tracing Pipeline and SBT helper
    // 2. [W] Descriptor Set Layout aggregator
    // 3. [W] Render Pass helper
    // 4. [W] Descriptor Set aggregate and helper
    // 5. [W] Device and driver aggregate
    // 6. [W] Buffer and VMA based vectors
    // W - Work in Progress, V - Verified, D - deprecated...

#define VK_FLAGS_NONE 0 // Custom define for better code readability
#define DEFAULT_FENCE_TIMEOUT (30ull*1000ull*1000ull*1000ull) // Default fence timeout in nanoseconds

    class VsRayTracingPipelineCreateInfoHelper {
    protected:
        VkRayTracingShaderGroupCreateInfoKHR raygenShaderGroup = {};
        std::vector<VkPipelineShaderStageCreateInfo> stages = {};
        std::vector<VkRayTracingShaderGroupCreateInfoKHR> missShaderGroups = {};
        std::vector<VkRayTracingShaderGroupCreateInfoKHR> hitShaderGroups = {};
        std::vector<VkRayTracingShaderGroupCreateInfoKHR> compiledShaderGroups = {};

    public: // get offsets of shader groups
        VkRayTracingPipelineCreateInfoKHR vkInfo = {};
        uintptr_t raygenOffsetIndex() { return 0u; };
        uintptr_t missOffsetIndex() { return 1u + raygenOffsetIndex(); };
        uintptr_t hitOffsetIndex() { return missShaderGroups.size() + missOffsetIndex(); };
        uintptr_t groupCount() { return missShaderGroups.size() + hitShaderGroups.size() + 1u; };

        //
        size_t missShaderCount() { return missShaderGroups.size(); };
        size_t hitShaderCount() { return hitShaderGroups.size(); };

        // 
        VsRayTracingPipelineCreateInfoHelper(const VkRayTracingPipelineCreateInfoKHR& info = {}) : vkInfo(info) {};

        // result groups
        inline std::vector<VkRayTracingShaderGroupCreateInfoKHR>& compileGroups() {
            compiledShaderGroups = { raygenShaderGroup };
            for (auto& group : missShaderGroups) { compiledShaderGroups.push_back(group); };
            for (auto& group : hitShaderGroups) { compiledShaderGroups.push_back(group); };
            return compiledShaderGroups;
        };

        // WARNING: Only One Hit Group Supported At Once
        inline VsRayTracingPipelineCreateInfoHelper& addShaderStages(const std::vector<VkPipelineShaderStageCreateInfo>& stages_in = {}, const VkRayTracingShaderGroupTypeKHR& prior_group_type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR) {
            for (auto& stage : stages_in) {
                if (stage.stage == VK_SHADER_STAGE_RAYGEN_BIT_KHR) {
                    const uintptr_t last_idx = stages.size(); stages.push_back(stage);
                    raygenShaderGroup.generalShader = static_cast<uint32_t>(last_idx);
                };
            };

            uintptr_t groupIdx = ~0U;
            for (auto& stage : stages_in) {
                if (stage.stage == VK_SHADER_STAGE_MISS_BIT_KHR) {
                    const uintptr_t lastIdx = stages.size(); stages.push_back(stage);
                    groupIdx = missShaderGroups.size(); missShaderGroups.push_back({});
                    //if (group_idx == -1U) { group_idx = miss_shader_groups.size(); miss_shader_groups.push_back({}); };
                    missShaderGroups[groupIdx].generalShader = static_cast<uint32_t>(lastIdx);//break;
                };
            };

            groupIdx = ~0U;  // Only One Hit Group Supported At Once
            for (auto& stage : stages_in) {
                if (stage.stage == VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR || stage.stage == VK_SHADER_STAGE_ANY_HIT_BIT_KHR || stage.stage == VK_SHADER_STAGE_INTERSECTION_BIT_KHR) {
                    const uintptr_t lastIdx = stages.size(); stages.push_back(stage);
                    if (groupIdx == ~0U) {
                        groupIdx = static_cast<uint32_t>(hitShaderGroups.size());
                        hitShaderGroups.push_back({});
                    };
                    auto& group = hitShaderGroups[groupIdx];
                    if (stage.stage == VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR) {
                        group.type = prior_group_type;
                        group.closestHitShader = static_cast<uint32_t>(lastIdx);
                    };
                    if (stage.stage == VK_SHADER_STAGE_ANY_HIT_BIT_KHR) {
                        group.type = prior_group_type;
                        group.anyHitShader = static_cast<uint32_t>(lastIdx);
                    };
                    if (stage.stage == VK_SHADER_STAGE_INTERSECTION_BIT_KHR) {
                        group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT_GROUP_KHR,
                            group.intersectionShader = static_cast<uint32_t>(lastIdx);
                    };
                };
            };

            return *this;
        };

        // 
        inline VkRayTracingPipelineCreateInfoKHR& format() {
            auto& groups = compileGroups();
            vkInfo.pGroups = groups.data();
            vkInfo.pStages = stages.data();
            vkInfo.stageCount = static_cast<uint32_t>(stages.size());
            vkInfo.groupCount = static_cast<uint32_t>(groups.size());
            //vkInfo.maxRecursionDepth = 4u;
            return vkInfo;
        };

        // 
        inline operator const ::VkRayTracingPipelineCreateInfoKHR* () const { return &vkInfo; };
        inline operator ::VkRayTracingPipelineCreateInfoKHR* () { return &format(); };

        // 
        inline operator const ::VkRayTracingPipelineCreateInfoKHR& () const { return vkInfo; };
        inline operator const VkRayTracingPipelineCreateInfoKHR& () { return vkInfo; };

        // 
        inline operator ::VkRayTracingPipelineCreateInfoKHR& () { return format(); };
        inline operator VkRayTracingPipelineCreateInfoKHR& () { return format(); };

#ifdef VULKAN_HPP // Vulkan-HPP
        inline operator const vk::RayTracingPipelineCreateInfoKHR& () const { return vkInfo; };
        inline operator vk::RayTracingPipelineCreateInfoKHR& () { return format(); };
#endif
    };

    // 
    template<class T = uint8_t>
    struct VsDescriptorHandle { //using T = T;
        uint32_t count = 0u;
        std::shared_ptr<std::vector<uint8_t>> V_t = {};
        uintptr_t field_t = 0u; // Byte Based
        size_t stride_t = sizeof(T);

        // 
        template<class C = T>
          VsDescriptorHandle(const VsDescriptorHandle<C>& handle) : count(handle.count), field_t(handle.field_t), stride_t(handle.stride_t), V_t(handle.V_t) {};
          VsDescriptorHandle(const uint32_t& count, const uintptr_t& field_t = 0ull, const std::shared_ptr<std::vector<uint8_t>>& V_t = {}, const size_t stride_t = sizeof(T)) : V_t(V_t), count(count), field_t(field_t), stride_t(stride_t) {};
         ~VsDescriptorHandle() {  };

        // any buffers and images can `write` into types
        inline VsDescriptorHandle<T> offset(const uint32_t& idx = 0u) { return VsDescriptorHandle<T>(count, stride_t*idx+field_t, V_t, stride_t); };
        inline const VsDescriptorHandle<T> offset(const uint32_t& idx = 0u) const { return VsDescriptorHandle<T>(count, stride_t*idx+field_t, V_t, stride_t); };
        template<class M> inline VsDescriptorHandle<M> offset(const uint32_t& idx = 0u) { return VsDescriptorHandle<M>(count, sizeof(M)*idx+field_t, V_t, sizeof(M)); };
        template<class M> inline const VsDescriptorHandle<M> offset(const uint32_t& idx = 0u) const { return VsDescriptorHandle<M>(count, sizeof(M)*idx+field_t, V_t, sizeof(M)); };
        inline const uint32_t& size() const { return count; };

        // new accessor operator
        inline VsDescriptorHandle<T> operator[](const uint32_t& idx) { return this->offset(idx); };
        inline VsDescriptorHandle<T> operator[](const uint32_t& idx) const { return this->offset(idx); };

        // 
        template<class C = T> inline VsDescriptorHandle<T>& operator=(const VsDescriptorHandle<C>& d) { stride_t = d.stride_t, count = d.count, field_t = d.field_t, V_t = d.V_t; return *this; };
        inline VsDescriptorHandle<T>& operator=(const T& d) { memcpy(&(*V_t)[field_t],&d,sizeof(T)); return *this; }; // Use MEMCPY method

        //
        template<class C = T> operator VsDescriptorHandle<C>& () { return reinterpret_cast<VsDescriptorHandle<C>&>(*this); };
        template<class C = T> operator const VsDescriptorHandle<C>& () const { return reinterpret_cast<const VsDescriptorHandle<C>&>(*this); };

        // 
        inline T& operator*()  { return (T&)  (*V_t)[field_t]; };
        inline T* operator&()  { return (T*)(&(*V_t)[field_t]); };
        inline T* operator->() { return (T*)(&(*V_t)[field_t]); };

        // 
        inline const T& operator*()  const { return (const T&)  (*V_t)[field_t]; };
        inline const T* operator&()  const { return (const T*)(&(*V_t)[field_t]); };
        inline const T* operator->() const { return (const T*)(&(*V_t)[field_t]); };

        // 
        inline operator T& () { return *reinterpret_cast<T*>((*V_t)[field_t]); };
        inline operator const T& () const { return *reinterpret_cast<T*>((*V_t)[field_t]); };

        //
        template<class M = T> M* handle() { return (M*)(&(*V_t)[field_t]); }
        template<class M = T> const M* handle() const { return (const M*)(&(*V_t)[field_t]); }
    };

    // 
    class VsDescriptorSetCreateInfoHelper { public: uint32_t flags = 0u; using T = uintptr_t; //
        inline  VsDescriptorSetCreateInfoHelper& reset() {
            heap->clear(); heap->resize(0);
            entries.clear(); entries.resize(0u);
            handles.clear(); //handles.resize(0);
            writes.clear(); writes.resize(0);
            writes_acs.clear(); writes_acs.resize(0);
            format(); return *this;
        };
        inline ~VsDescriptorSetCreateInfoHelper() { this->reset(); };
        inline  VsDescriptorSetCreateInfoHelper(const VkDescriptorSetLayout& layout = {}, const VkDescriptorPool& pool = {}) {
            heap = std::make_shared<std::vector<uint8_t>>(); this->reset();
            template_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_CREATE_INFO;
            template_info.pNext = nullptr;
            template_info.flags = flags;
            template_info.descriptorUpdateEntryCount = 0u;
            template_info.pDescriptorUpdateEntries = nullptr;
            template_info.descriptorSetLayout = layout;
            template_info.pipelineLayout = VK_NULL_HANDLE;
            template_info.pipelineBindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
            template_info.templateType = VK_DESCRIPTOR_UPDATE_TEMPLATE_TYPE_DESCRIPTOR_SET;
            allocate_info.descriptorPool = pool;
            allocate_info.pSetLayouts = &layout;
            allocate_info.descriptorSetCount = 1u;
        };

        // official function (not template)
        template<class T = uint8_t>
        inline VsDescriptorHandle<T>& pushDescription( const VkDescriptorUpdateTemplateEntry& entry = {} ) {
            if (entry.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER || entry.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                return _push_description<VkDescriptorBufferInfo>(entry);
            } else
            if (entry.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER || entry.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER) {
                return _push_description<VkBufferView>(entry);
            } else 
            if (entry.descriptorType == VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR) {
                return _push_description<VkAccelerationStructureKHR>(entry);
            } else 
            if (entry.descriptorType == VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT) {
                return _push_description<T>(entry);
            } else {
                return _push_description<VkDescriptorImageInfo>(entry);
            };
        };

        // 
        inline VsDescriptorSetCreateInfoHelper& setDescriptorSet(const VkDescriptorSet& set = {}) {
            this->set = set; return *this;
        };

        // 
        inline VkDescriptorUpdateTemplateCreateInfo& format() {
            template_info.pDescriptorUpdateEntries = entries.data();
            template_info.descriptorUpdateEntryCount = static_cast<uint32_t>(entries.size());
            return template_info;
        };

        // 
        inline operator std::vector<VkDescriptorUpdateTemplateEntry>& () { return entries; };
        inline operator const std::vector<VkDescriptorUpdateTemplateEntry>& () const { return entries; };

        //
        inline operator VkDescriptorPool& () { return allocate_info.descriptorPool; };
        inline operator const VkDescriptorPool&() const { return allocate_info.descriptorPool; };

        // 
        inline operator ::VkDescriptorSetAllocateInfo& () { return allocate_info; };
        inline operator ::VkDescriptorUpdateTemplateCreateInfo& () { return format(); };

        // 
        inline operator VkDescriptorSetAllocateInfo&() { return allocate_info; };
        inline operator VkDescriptorUpdateTemplateCreateInfo& () { return format(); };

        // 
        inline operator ::VkDescriptorSetAllocateInfo* () { return &allocate_info; };
        inline operator ::VkDescriptorUpdateTemplateCreateInfo* () { return &format(); };

        // 
        inline operator const ::VkDescriptorSetAllocateInfo* () const { return &allocate_info; };
        inline operator const ::VkDescriptorUpdateTemplateCreateInfo* () const { return &template_info; };

        // 
        inline operator const ::VkDescriptorSetAllocateInfo& () const { return allocate_info; };
        inline operator const ::VkDescriptorUpdateTemplateCreateInfo& () const { return template_info; };

        // 
        inline operator const VkDescriptorSetAllocateInfo& () const { return allocate_info; };
        inline operator const VkDescriptorUpdateTemplateCreateInfo& () const { return template_info; };

#ifdef VULKAN_HPP // Vulkan-HPP
        inline operator vk::DescriptorSetAllocateInfo& () { return allocate_info; };
        inline operator vk::DescriptorUpdateTemplateCreateInfo& () { return format(); };

        // Vulkan-HPP
        inline operator const vk::DescriptorSetAllocateInfo& () const { return allocate_info; };
        inline operator const vk::DescriptorUpdateTemplateCreateInfo& () const { return template_info; };
#endif

        // 
        inline std::vector<VkWriteDescriptorSet>& mapWriteDescriptorSet() {
            entries.resize(handles.size());
            uint32_t I = 0; for (auto& hndl : handles) {
                const uint32_t i = I++;
                const auto& entry = entries[i];//hndl.entry_t;
                const auto& pt0 = entry.offset;

                if (entry.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER || entry.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) { // Map Buffers
                    writes[i].pBufferInfo = reinterpret_cast<::VkDescriptorBufferInfo*>(heap->data()+pt0);
                } else 
                if (entry.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER || entry.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER) { // Map Buffer Views
                    writes[i].pTexelBufferView = reinterpret_cast<VkBufferView*>(heap->data()+pt0);
                } else 
                if (entry.descriptorType == VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR) { // Map Accelerator Structures (KHR CUSTOM MAP)
                    writes_acs[i].pAccelerationStructures = reinterpret_cast<VkAccelerationStructureKHR*>(heap->data()+pt0);
                    writes[i].pNext = &writes_acs[i];
                } else 
                if (entry.descriptorType == VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT) { // Map Inline Uniform Blocks
                    writes[i].pNext = &writes_inline[i];
                    writes_inline[i].pData = heap->data()+pt0;
                } else 
                { // Map Images, Samplers, Combinations...
                    writes[i].pImageInfo = reinterpret_cast<::VkDescriptorImageInfo*>(heap->data()+pt0);
                };

                writes[i].dstSet = this->set;
                entries[i] = entry;
            };
            return writes;
        };

        // Export By Operator
        inline operator std::vector<VkWriteDescriptorSet>&() { return mapWriteDescriptorSet(); };
        inline operator const std::vector<VkWriteDescriptorSet>&() const { return writes; };

    protected: template<class T = T> // 
        inline VsDescriptorHandle<T>& _push_description( VkDescriptorUpdateTemplateEntry entry ) { // Un-Safe API again
            const uintptr_t pt0 = heap->size(); entry.offset = pt0, entry.stride = sizeof(T);

            // 
            heap->resize(pt0+sizeof(T)*entry.descriptorCount, 0u);

            // Default Values
            if (entry.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER || entry.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                *reinterpret_cast<VkDescriptorBufferInfo*>(heap->data()+pt0) = VkDescriptorBufferInfo{};
            } else 
            if (entry.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER || entry.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER) {
                *reinterpret_cast<VkBufferView*>(heap->data()+pt0) = VK_NULL_HANDLE;
            } else 
            if (entry.descriptorType == VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR) {
                *reinterpret_cast<VkAccelerationStructureKHR*>(heap->data()+pt0) = VK_NULL_HANDLE;
            } else 
            if (entry.descriptorType == VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT) {
                //*reinterpret_cast<VkWriteDescriptorSetInlineUniformBlockEXT*>(heap->data()+pt0) = VkWriteDescriptorSetInlineUniformBlockEXT{};
                
            } else {
                *reinterpret_cast<VkDescriptorImageInfo*>(heap->data()+pt0) = VkDescriptorImageInfo{};
            };

            handles.push_back(VsDescriptorHandle<T>(entry.descriptorCount, uintptr_t(pt0), heap));
            writes.push_back({
                .dstSet = this->set,
                .dstBinding = entry.dstBinding,
                .dstArrayElement = entry.dstArrayElement,
                .descriptorCount = entry.descriptorType == VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT ? (sizeof(T)*entry.descriptorCount) : entry.descriptorCount,
                .descriptorType = entry.descriptorType,
            });
            writes_acs.push_back(VkWriteDescriptorSetAccelerationStructureKHR{
                .accelerationStructureCount = entry.descriptorType == VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR ? entry.descriptorCount : 0u
            });
            entries.push_back(vkh::VkDescriptorUpdateTemplateEntry{ // TODO: Direct Contact With Descriptor and Heap
                .dstBinding = entry.dstBinding,
                .dstArrayElement = entry.dstArrayElement,
                .descriptorCount = entry.descriptorCount,
                .descriptorType = entry.descriptorType,
                .offset = pt0,
                .stride = sizeof(T)
            });
            writes_inline.push_back(vkh::VkWriteDescriptorSetInlineUniformBlockEXT{
                .dataSize = sizeof(T)*entry.descriptorCount
            });
            return handles.back();
        };

        // 
        VkDescriptorSetAllocateInfo allocate_info = {};
        VkDescriptorUpdateTemplateCreateInfo template_info = {};
        VkDescriptorSet set = {};

        // 
        std::shared_ptr<std::vector<uint8_t>> heap = {};
        std::vector<VkDescriptorUpdateTemplateEntry> entries = {};
        std::vector<VsDescriptorHandle<>> handles = {};

        // regular version
        std::vector<VkWriteDescriptorSetInlineUniformBlockEXT> writes_inline = {};
        std::vector<VkWriteDescriptorSetAccelerationStructureKHR> writes_acs = {};
        std::vector<VkWriteDescriptorSet> writes = {};
    };
 

    // TODO: REMOVE CODE TAFTOLOGY
    class VsDescriptorSetLayoutCreateInfoHelper { public: 
        VsDescriptorSetLayoutCreateInfoHelper(const VkDescriptorSetLayoutCreateInfo& info = {}) : vk_info(info) {
            vk_info.pNext = &flags_info;
        };

        // 
        inline VsDescriptorSetLayoutCreateInfoHelper& pushBinding(const VkDescriptorSetLayoutBinding& binding = {}, const FLAGS(VkDescriptorBinding)& flags = {}){
            binding_flags.push_back(flags);
            bindings.push_back(binding);
            return *this;
        };

        // 
        inline VkDescriptorSetLayoutCreateInfo& format() {
            vk_info.pBindings = bindings.data();
            vk_info.bindingCount = static_cast<uint32_t>(bindings.size());
            flags_info.pBindingFlags = binding_flags.data();
            flags_info.bindingCount = static_cast<uint32_t>(binding_flags.size());
            return vk_info;
        };

        // 
        inline operator const ::VkDescriptorSetLayoutCreateInfo& () const { return vk_info; };
        inline operator ::VkDescriptorSetLayoutCreateInfo& () { return format(); };

        // Vulkan-HPP
#ifdef VULKAN_HPP
        inline operator const vk::DescriptorSetLayoutCreateInfo& () const { return vk_info; };
        inline operator vk::DescriptorSetLayoutCreateInfo& () { return format(); };
#endif

        // 
        inline operator const VkDescriptorSetLayoutCreateInfo&() const { return vk_info; };
        inline operator VkDescriptorSetLayoutCreateInfo&() { return format(); };

        // 
        inline operator const ::VkDescriptorSetLayoutCreateInfo* () const { return &vk_info; };
        inline operator ::VkDescriptorSetLayoutCreateInfo* () { return &format(); };

        // 
        inline const VkDescriptorSetLayoutCreateInfo& get() const { return vk_info; };
        inline VkDescriptorSetLayoutCreateInfo& get() { return format(); };

    protected: // 
        VkDescriptorSetLayoutCreateInfo vk_info = {};
        VkDescriptorSetLayoutBindingFlagsCreateInfo flags_info = {};
        std::vector<VkDescriptorSetLayoutBinding> bindings = {};
        std::vector<::VkDescriptorBindingFlags> binding_flags = {};
    };

    // TODO: Update for RenderPass v2
    class VsRenderPassCreateInfoHelper { public: 
        VsRenderPassCreateInfoHelper(const VkRenderPassCreateInfo& info = {}) : vk_info(info) {
            
        };

        // 
        inline std::vector<VkSubpassDescription>& subpassFormat() {
            for (uint32_t i = 0; i < color_attachments.size(); i++) {
                subpasses[i].pColorAttachments = color_attachments[i].data();
                subpasses[i].colorAttachmentCount = static_cast<uint32_t>(color_attachments[i].size());
                subpasses[i].pDepthStencilAttachment = &depth_stencil_attachment[i];
                subpasses[i].pInputAttachments = input_attachments[i].data();
                subpasses[i].inputAttachmentCount = static_cast<uint32_t>(input_attachments[i].size());
            };
            return subpasses;
        };

        // 
        inline VkRenderPassCreateInfo& format() {
            vk_info.pAttachments = attachments.data();
            vk_info.attachmentCount = static_cast<uint32_t>(attachments.size());
            vk_info.pDependencies = dependencies.data();
            vk_info.dependencyCount = static_cast<uint32_t>(dependencies.size());
            vk_info.setSubpasses(subpassFormat());
            return vk_info;
        };

        // 
        inline operator const ::VkRenderPassCreateInfo* () const { return &vk_info; };
        inline operator ::VkRenderPassCreateInfo* () { return &format(); };

        // 
        inline operator const VkRenderPassCreateInfo& () const { return vk_info; };
        inline operator VkRenderPassCreateInfo& () { return format(); };

        // 
        inline operator const ::VkRenderPassCreateInfo& () const { return vk_info; };
        inline operator ::VkRenderPassCreateInfo& () { return format(); };

        // Vulkan-HPP
#ifdef VULKAN_HPP
        inline operator const vk::RenderPassCreateInfo& () const { return vk_info; };
        inline operator vk::RenderPassCreateInfo& () { return format(); };
#endif

        // 
        inline operator const std::vector<VkAttachmentDescription>&() const { return attachments; };
        inline operator const std::vector<VkSubpassDependency>&() const { return dependencies; };
        inline operator const std::vector<VkSubpassDescription>&() const { return subpasses; };
        inline operator std::vector<VkAttachmentDescription>&() { return attachments; };
        inline operator std::vector<VkSubpassDependency>&() { return dependencies; };
        inline operator std::vector<VkSubpassDescription>& () { return subpassFormat(); };

        //
        inline VsRenderPassCreateInfoHelper& beginSubpass() { subpasses.push_back({}); color_attachments.push_back({}); input_attachments.push_back({}); depth_stencil_attachment.push_back({}); return *this; };
        inline VsRenderPassCreateInfoHelper& addSubpassDependency(const VkSubpassDependency& dependency = {}) { dependencies.push_back(dependency); return *this; };

        // 
        inline VsRenderPassCreateInfoHelper& addInputAttachment(const VkAttachmentDescription& attachment = {}) {
            uintptr_t ptr = attachments.size(); attachments.push_back(attachment); auto& layout = attachments.back().finalLayout;
            if (layout == VK_IMAGE_LAYOUT_UNDEFINED) { layout = VK_IMAGE_LAYOUT_GENERAL; };
            if (subpasses.size() < 1u) { beginSubpass(); };
            input_attachments.back().push_back({ .attachment = static_cast<uint32_t>(ptr), .layout = VK_IMAGE_LAYOUT_GENERAL });
            return *this;
        };

        // 
        inline VsRenderPassCreateInfoHelper& addColorAttachment(const VkAttachmentDescription& attachment = {}) {
            uintptr_t ptr = attachments.size(); attachments.push_back(attachment); auto& layout = attachments.back().finalLayout;
            if (layout == VK_IMAGE_LAYOUT_UNDEFINED) { layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; };
            if (subpasses.size() < 1u) { beginSubpass(); };
            color_attachments.back().push_back({ .attachment = static_cast<uint32_t>(ptr), .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
            return *this;
        };

        //
        inline VsRenderPassCreateInfoHelper& setDepthStencilAttachment(const VkAttachmentDescription& attachment = {}) {
            uintptr_t ptr = attachments.size(); attachments.push_back(attachment); auto& layout = attachments.back().finalLayout;
            if (layout == VK_IMAGE_LAYOUT_UNDEFINED) { layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; };
            if (subpasses.size() < 1u) { beginSubpass(); };
            depth_stencil_attachment.back() = { .attachment = static_cast<uint32_t>(ptr), .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
            return *this;
        };

    protected: // 
        VkRenderPassCreateInfo vk_info = {};
        std::vector<VkAttachmentDescription> attachments = {};
        std::vector<VkSubpassDescription> subpasses = {};
        std::vector<VkSubpassDependency> dependencies = {};
        std::vector<std::vector<VkAttachmentReference>> color_attachments = {};
        std::vector<std::vector<VkAttachmentReference>> input_attachments = {};
        std::vector<VkAttachmentReference> depth_stencil_attachment {};
    };

    // constructive helper for graphics pipeline
    // also, sub-member can be accessed too
    class VsGraphicsPipelineCreateInfoConstruction { public: 
        VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
        VkPipelineVertexInputStateCreateInfo vertexInputState = {};
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
        VkPipelineTessellationStateCreateInfo tessellationState = {};
        VkPipelineViewportStateCreateInfo viewportState = {};
        VkPipelineRasterizationStateCreateInfo rasterizationState = {};
        VkPipelineMultisampleStateCreateInfo multisampleState = {};
        VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
        VkPipelineColorBlendStateCreateInfo colorBlendState = {};
        VkPipelineDynamicStateCreateInfo dynamicState = {};
        VkPipelineRasterizationConservativeStateCreateInfoEXT conservativeRasterization = {};

        // 
        std::vector<VkDynamicState> dynamicStates = {};
        std::vector<VkPipelineShaderStageCreateInfo> stages = {};
        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates = {};
        std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions = {};
        std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions = {};

        // constructive structure
        inline operator ::VkGraphicsPipelineCreateInfo& () { construct(); return graphicsPipelineCreateInfo; };
        inline operator const ::VkGraphicsPipelineCreateInfo& () const { return graphicsPipelineCreateInfo; };

        // 
        inline operator VkGraphicsPipelineCreateInfo& () { construct(); return graphicsPipelineCreateInfo; };
        inline operator const VkGraphicsPipelineCreateInfo& () const { return graphicsPipelineCreateInfo; };

#ifdef VULKAN_HPP // Vulkan-HPP
        inline operator vk::GraphicsPipelineCreateInfo& () { construct(); return graphicsPipelineCreateInfo; };
        inline operator const vk::GraphicsPipelineCreateInfo& () const { return graphicsPipelineCreateInfo; };
#endif

        //
        inline operator ::VkGraphicsPipelineCreateInfo* () { construct(); return &graphicsPipelineCreateInfo; };
        inline operator const ::VkGraphicsPipelineCreateInfo* () const { return &graphicsPipelineCreateInfo; };

        // 
        inline VsGraphicsPipelineCreateInfoConstruction& operator=(const VkGraphicsPipelineCreateInfo& info) { graphicsPipelineCreateInfo = info; construct(); return *this; };
        inline VsGraphicsPipelineCreateInfoConstruction& construct() {
            graphicsPipelineCreateInfo.setStages(stages);
            graphicsPipelineCreateInfo.pVertexInputState = &vertexInputState;
            graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
            graphicsPipelineCreateInfo.pTessellationState = &tessellationState;
            graphicsPipelineCreateInfo.pViewportState = &viewportState;
            graphicsPipelineCreateInfo.pRasterizationState = &rasterizationState;
            graphicsPipelineCreateInfo.pMultisampleState = &multisampleState;
            graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilState;
            graphicsPipelineCreateInfo.pColorBlendState = &colorBlendState;
            graphicsPipelineCreateInfo.pDynamicState = &dynamicState;
            rasterizationState.pNext = &conservativeRasterization;
            dynamicState.setDynamicStates(dynamicStates);
            colorBlendState.setAttachments(colorBlendAttachmentStates);
            vertexInputState.setVertexAttributeDescriptions(vertexInputAttributeDescriptions);
            vertexInputState.setVertexBindingDescriptions(vertexInputBindingDescriptions);
            return *this;
        };

        // direct access operator
        inline ::VkGraphicsPipelineCreateInfo* operator->() { construct(); return &graphicsPipelineCreateInfo; };
        inline const ::VkGraphicsPipelineCreateInfo* operator->() const { return &graphicsPipelineCreateInfo; };
    };


    // TODO: IMAGE VIEW REGION IN MIP LEVEL
    // WILL USED FOR SIMPLER COPYING

    


};
