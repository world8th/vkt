#pragma once

#include <memory>
#include <vector>
#include "./structures.hpp"

namespace vkh {

    // TODO: Some Vookoo-like aggregators and helpers
    // 1. [W] Ray Tracing Pipeline and SBT helper
    // 2. [W] Descriptor Set Layout aggregator
    // 3. [W] Render Pass helper
    // 4. [W] Descriptor Set aggregate and helper
    // 5. [ ] Device and driver aggregate
    // 6. [ ] Buffer and VMA based vectors
    // W - Work in Progress, V - Verified, D - deprecated...

    // TODO: REMOVE CODE TAFTOLOGY
    class VsRayTracingPipelineCreateInfoHelper { protected: 
        VkRayTracingPipelineCreateInfoNV vk_info = {};
        VkRayTracingShaderGroupCreateInfoNV raygen_shader_group = {};
        std::vector<VkPipelineShaderStageCreateInfo> stages = {};
        std::vector<VkRayTracingShaderGroupCreateInfoNV> miss_shader_groups = {};
        std::vector<VkRayTracingShaderGroupCreateInfoNV> hit_shader_groups = {};
        std::vector<VkRayTracingShaderGroupCreateInfoNV> compiled_shader_groups = {};

    public: // get offsets of shader groups
        uintptr_t raygenOffsetIndex() { return 0u; };
        uintptr_t missOffsetIndex() {return 1u; };
        uintptr_t hitOffsetIndex() { return miss_shader_groups.size()+missOffsetIndex(); };

        // 
        VsRayTracingPipelineCreateInfoHelper(const VkRayTracingPipelineCreateInfoNV& info = {}) : vk_info(info) {};

        // result groups
        inline std::vector<VkRayTracingShaderGroupCreateInfoNV>& compileGroups() {
            compiled_shader_groups = { raygen_shader_group };
            for (auto& group : miss_shader_groups) { compiled_shader_groups.push_back(group); };
            for (auto& group : hit_shader_groups) { compiled_shader_groups.push_back(group); };
            return compiled_shader_groups;
        };

        // WARNING: Only One Hit Group Supported At Once
        inline VsRayTracingPipelineCreateInfoHelper& addShaderStages(const std::vector<VkPipelineShaderStageCreateInfo>& stages_in = {}, const VkRayTracingShaderGroupTypeNV& prior_group_type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_NV) {
            for (auto& stage : stages_in) {
                if (stage.stage == VK_SHADER_STAGE_RAYGEN_BIT_NV) {
                    const uintptr_t last_idx = stages.size(); stages.push_back(stage);
                    raygen_shader_group.generalShader = last_idx;
                };
            };

            uintptr_t group_idx = -1U;
            for (auto& stage : stages_in) {
                if (stage.stage == VK_SHADER_STAGE_MISS_BIT_NV) {
                    const uintptr_t last_idx = stages.size(); stages.push_back(stage);
                    group_idx = miss_shader_groups.size(); miss_shader_groups.push_back({});
                    //if (group_idx == -1U) { group_idx = miss_shader_groups.size(); miss_shader_groups.push_back({}); };
                    miss_shader_groups[group_idx].generalShader = last_idx;//break;
                };
            };

            group_idx = -1U; // Only One Hit Group Supported At Once
            for (auto& stage : stages_in) {
                if (stage.stage == VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV || stage.stage == VK_SHADER_STAGE_ANY_HIT_BIT_NV || stage.stage == VK_SHADER_STAGE_INTERSECTION_BIT_NV) {
                    const uintptr_t last_idx = stages.size(); stages.push_back(stage);
                    if (group_idx == -1U) { group_idx = hit_shader_groups.size(); hit_shader_groups.push_back({}); };
                    if (stage.stage == VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV) {
                        hit_shader_groups[group_idx].type = prior_group_type;
                        hit_shader_groups[group_idx].closestHitShader = last_idx;
                    };
                    if (stage.stage == VK_SHADER_STAGE_ANY_HIT_BIT_NV) {
                        hit_shader_groups[group_idx].type = prior_group_type;
                        hit_shader_groups[group_idx].anyHitShader = last_idx;
                    };
                    if (stage.stage == VK_SHADER_STAGE_INTERSECTION_BIT_NV) {
                        hit_shader_groups[group_idx].type = VK_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT_GROUP_NV, 
                        hit_shader_groups[group_idx].intersectionShader = last_idx;
                    };
                };
            };

            return *this;
        };

        // 
        inline VkRayTracingPipelineCreateInfoNV& format() {
            auto& groups = compileGroups();
            vk_info.pGroups = groups.data();
            vk_info.pStages = stages.data();
            vk_info.stageCount = stages.size();
            vk_info.groupCount = groups.size();
            return vk_info;
        };

        // 
        inline operator const ::VkRayTracingPipelineCreateInfoNV* () const { return vk_info; };
        inline operator const VkRayTracingPipelineCreateInfoNV* () { return &vk_info; };

        // 
        inline operator ::VkRayTracingPipelineCreateInfoNV* () { return format(); };
        inline operator VkRayTracingPipelineCreateInfoNV* () { return &format(); };

        // 
        inline operator const ::VkRayTracingPipelineCreateInfoNV&() const { return vk_info; };
        inline operator const VkRayTracingPipelineCreateInfoNV&() { return vk_info; };

        // 
        inline operator ::VkRayTracingPipelineCreateInfoNV&() { return format(); };
        inline operator VkRayTracingPipelineCreateInfoNV&() { return format(); };

        // Vulkan-HPP
        inline operator const vk::RayTracingPipelineCreateInfoNV& () const { return vk_info; };
        inline operator vk::RayTracingPipelineCreateInfoNV& () { return format(); };
    };

    // 
    struct VsDescriptorHandle { using T = uint8_t;
        VkDescriptorUpdateTemplateEntry* entry_t = nullptr;
        void* field_t = nullptr;

        // any buffers and images can `write` into types
        template<class T = uint8_t> inline operator T&() { return *reinterpret_cast<T*>(field_t); };
        template<class T = uint8_t> inline operator const T&() const { return *reinterpret_cast<T*>(field_t); };
        template<class T = uint8_t> inline T& offset(const uint32_t& idx = 0u) { return VsDescriptorHandle{entry_t,(T*)field_t+idx}; };
        template<class T = uint8_t> inline const T& offset(const uint32_t& idx = 0u) const { return VsDescriptorHandle{entry_t,(T*)field_t+idx}; };
        inline const uint32_t& size() const { return entry_t->descriptorCount; }; 

        template<class T = uint8_t> // 
        inline VsDescriptorHandle& operator=(const T& d) { *reinterpret_cast<T*>(field_t) = d; return *this; };
    };

    // TODO: REMOVE CODE TAFTOLOGY
    class VsDescriptorSetCreateInfoHelper { public: uint32_t flags = 0u; using T = uintptr_t; // 

         VsDescriptorSetCreateInfoHelper& reset() { heap.clear(); entries.clear(); handles.clear(); writes.clear(); writes_acs.clear(); format(); return *this; };
        ~VsDescriptorSetCreateInfoHelper() { reset(); };
         VsDescriptorSetCreateInfoHelper(const VkDescriptorSetLayout& layout = {}, const VkDescriptorPool& pool = {}) {
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
            allocate_info.pSetLayouts = &template_info.descriptorSetLayout;
            allocate_info.descriptorSetCount = 1u;
        };

        // official function (not template)
        inline VsDescriptorHandle& pushDescription( const VkDescriptorUpdateTemplateEntry& entry = {} ) {
            if (entry.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER || entry.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                return _push_description<VkDescriptorBufferInfo>(entry);
            } else 
            if (entry.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER || entry.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER) {
                return _push_description<VkBufferView>(entry);
            } else 
            if (entry.descriptorType == VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV) {
                return _push_description<VkAccelerationStructureNV>(entry);
            } else {
                return _push_description<VkDescriptorImageInfo>(entry);
            };
        };

        // 
        inline VkDescriptorUpdateTemplateCreateInfo& format() {
            template_info.pDescriptorUpdateEntries = entries.data();
            template_info.descriptorUpdateEntryCount = entries.size();
            return template_info;
        };

        // 
        inline operator std::vector<VkDescriptorUpdateTemplateEntry>& () { return entries; };
        inline operator const std::vector<VkDescriptorUpdateTemplateEntry>& () const { return entries; };

        // 
        inline operator ::VkDescriptorSetAllocateInfo& () { return allocate_info; };
        inline operator ::VkDescriptorUpdateTemplateCreateInfo& () { return format(); };

        // 
        inline operator VkDescriptorSetAllocateInfo&() { return allocate_info; };
        inline operator VkDescriptorUpdateTemplateCreateInfo& () { return format(); };

        // 
        inline operator ::VkDescriptorSetAllocateInfo* () { return allocate_info; };
        inline operator ::VkDescriptorUpdateTemplateCreateInfo* () { return format(); };

        // 
        inline operator VkDescriptorSetAllocateInfo* () { return &allocate_info; };
        inline operator VkDescriptorUpdateTemplateCreateInfo* () { return &format(); };

        // 
        inline operator const ::VkDescriptorSetAllocateInfo* () const { return allocate_info; };
        inline operator const ::VkDescriptorUpdateTemplateCreateInfo* () const { return template_info; };

        // 
        inline operator const VkDescriptorSetAllocateInfo* () const { return &allocate_info; };
        inline operator const VkDescriptorUpdateTemplateCreateInfo* () const { return &template_info; };

        // 
        inline operator const ::VkDescriptorSetAllocateInfo& () const { return allocate_info; };
        inline operator const ::VkDescriptorUpdateTemplateCreateInfo& () const { return template_info; };

        // 
        inline operator const VkDescriptorSetAllocateInfo& () const { return allocate_info; };
        inline operator const VkDescriptorUpdateTemplateCreateInfo& () const { return template_info; };

        // Vulkan-HPP
        inline operator vk::DescriptorSetAllocateInfo& () { return allocate_info; };
        inline operator vk::DescriptorUpdateTemplateCreateInfo& () { return format(); };

        // Vulkan-HPP
        inline operator const vk::DescriptorSetAllocateInfo& () const { return allocate_info; };
        inline operator const vk::DescriptorUpdateTemplateCreateInfo& () const { return template_info; };

        // 
        inline std::vector<VkWriteDescriptorSet>& mapWriteDescriptorSet() {
            uint32_t I=0; for (auto& entry : entries) { const uint32_t i = I++;
                const uintptr_t& pt0 = entry.offset;
                if (entry.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER || entry.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) { // Map Buffers
                    writes[i].pBufferInfo = (VkDescriptorBufferInfo*)(heap.data()+pt0);
                } else 
                if (entry.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER || entry.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER) { // Map Buffer Views
                    writes[i].pTexelBufferView = (VkBufferView*)(heap.data()+pt0);
                } else 
                if (entry.descriptorType == VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV) { // Map Accelerator Structures (NV CUSTOM MAP)
                    writes_acs[i].pAccelerationStructures = (VkAccelerationStructureNV*)(heap.data()+pt0);
                    writes[i].pNext = &writes_acs[i];
                } else { // Map Images, Samplers, Combinations...
                    writes[i].pImageInfo = (VkDescriptorImageInfo*)(heap.data()+pt0);
                };
            };
            return writes;
        };

        // Export By Operator
        operator std::vector<VkWriteDescriptorSet>&() { return mapWriteDescriptorSet(); };
        operator const std::vector<VkWriteDescriptorSet>&() const { return writes; };


    protected: template<class T = T> // 
        inline VsDescriptorHandle& _push_description( const VkDescriptorUpdateTemplateEntry& entry ) { // Un-Safe API again
            const uintptr_t pt0 = heap.size();
            heap.resize(pt0+sizeof(T)*entry.descriptorCount, 0u);
            entries.push_back(entry);
            entries.back().offset = pt0;
            entries.back().stride = sizeof(T);
            handles.push_back({ &entries.back(), &heap.back() });
            writes.push_back({
                .dstBinding = entry.dstBinding,
                .dstArrayElement = entry.dstArrayElement,
                .descriptorCount = entry.descriptorCount,
                .descriptorType = entry.descriptorType,
            });
            writes_acs.push_back(VkWriteDescriptorSetAccelerationStructureNV{
                .accelerationStructureCount = entry.descriptorType == VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV ? entry.descriptorCount : 0u
            });
            return handles.back();
        };

        // 
        VkDescriptorSetAllocateInfo allocate_info = {};
        VkDescriptorUpdateTemplateCreateInfo template_info = {};

        // 
        std::vector<uint8_t> heap = {};
        std::vector<VkDescriptorUpdateTemplateEntry> entries = {};
        std::vector<VsDescriptorHandle> handles = {};

        // regular version
        std::vector<VkWriteDescriptorSetAccelerationStructureNV> writes_acs = {};
        std::vector<VkWriteDescriptorSet> writes = {};
    };
 

    // TODO: REMOVE CODE TAFTOLOGY
    class VsDescriptorSetLayoutCreateInfoHelper { public: 
        VsDescriptorSetLayoutCreateInfoHelper(const VkDescriptorSetLayoutCreateInfo& info = {}) : vk_info(info) {
            vk_info.pNext = &flags_info;
        };

        // 
        inline VsDescriptorSetLayoutCreateInfoHelper& pushBinding(const VkDescriptorSetLayoutBinding& binding = {}, const VkDescriptorBindingFlagsEXT& flags = {}){
            binding_flags.push_back(flags);
            bindings.push_back(binding);
            return *this;
        };

        // 
        inline VkDescriptorSetLayoutCreateInfo& format() {
            vk_info.pBindings = bindings.data();
            vk_info.bindingCount = bindings.size();
            flags_info.pBindingFlags = binding_flags.data();
            flags_info.bindingCount = binding_flags.size();
            return vk_info;
        };

        // 
        inline operator const ::VkDescriptorSetLayoutCreateInfo& () const { return vk_info; };
        inline operator ::VkDescriptorSetLayoutCreateInfo& () { return format(); };

        // Vulkan-HPP
        inline operator const vk::DescriptorSetLayoutCreateInfo& () const { return vk_info; };
        inline operator vk::DescriptorSetLayoutCreateInfo& () { return format(); };

        // 
        inline operator const VkDescriptorSetLayoutCreateInfo&() const { return vk_info; };
        inline operator VkDescriptorSetLayoutCreateInfo&() { return format(); };

        // 
        inline operator const ::VkDescriptorSetLayoutCreateInfo* () const { return vk_info; };
        inline operator ::VkDescriptorSetLayoutCreateInfo* () { return format(); };

        // 
        inline operator const VkDescriptorSetLayoutCreateInfo* () const { return &vk_info; };
        inline operator VkDescriptorSetLayoutCreateInfo* () { return &format(); };

    protected: // 
        VkDescriptorSetLayoutCreateInfo vk_info = {};
        VkDescriptorSetLayoutBindingFlagsCreateInfoEXT flags_info = {};
        std::vector<VkDescriptorSetLayoutBinding> bindings = {};
        std::vector<VkDescriptorBindingFlagsEXT> binding_flags = {};
    };

    // TODO: REMOVE CODE TAFTOLOGY
    class VsRenderPassCreateInfoHelper { public: 
        VsRenderPassCreateInfoHelper(const VkRenderPassCreateInfo& info = {}) : vk_info(info) {

        };

        // 
        inline std::vector<VkSubpassDescription>& subpassFormat() {
            for (uint32_t i = 0; i < color_attachments.size(); i++) {
                subpasses[i].pColorAttachments = color_attachments[i].data();
                subpasses[i].colorAttachmentCount = color_attachments[i].size();
                subpasses[i].pDepthStencilAttachment = &depth_stencil_attachment[i];
                subpasses[i].pInputAttachments = input_attachments[i].data();
                subpasses[i].inputAttachmentCount = input_attachments[i].size();
            };
            return subpasses;
        };

        // 
        inline VkRenderPassCreateInfo& format() {
            vk_info.pAttachments = attachments.data();
            vk_info.attachmentCount = attachments.size();
            vk_info.pDependencies = dependencies.data();
            vk_info.dependencyCount = dependencies.size();
            vk_info.setSubpasses(subpassFormat());
            return vk_info;
        };

        // 
        inline operator const VkRenderPassCreateInfo* () const { return &vk_info; };
        inline operator VkRenderPassCreateInfo* () { return &format(); };

        // 
        inline operator const ::VkRenderPassCreateInfo* () const { return vk_info; };
        inline operator ::VkRenderPassCreateInfo* () { return format(); };

        // 
        inline operator const VkRenderPassCreateInfo& () const { return vk_info; };
        inline operator VkRenderPassCreateInfo& () { return format(); };

        // 
        inline operator const ::VkRenderPassCreateInfo& () const { return vk_info; };
        inline operator ::VkRenderPassCreateInfo& () { return format(); };

        // Vulkan-HPP
        inline operator const vk::RenderPassCreateInfo& () const { return vk_info; };
        inline operator vk::RenderPassCreateInfo& () { return format(); };

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

        // Vulkan-HPP
        inline operator vk::GraphicsPipelineCreateInfo& () { construct(); return graphicsPipelineCreateInfo; };
        inline operator const vk::GraphicsPipelineCreateInfo& () const { return graphicsPipelineCreateInfo; };

        //
        inline operator ::VkGraphicsPipelineCreateInfo* () { construct(); return graphicsPipelineCreateInfo; };
        inline operator const ::VkGraphicsPipelineCreateInfo* () const { return graphicsPipelineCreateInfo; };

        // 
        inline operator VkGraphicsPipelineCreateInfo* () { construct(); return &graphicsPipelineCreateInfo; };
        inline operator const VkGraphicsPipelineCreateInfo* () const { return &graphicsPipelineCreateInfo; };

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
            dynamicState.setDynamicStates(dynamicStates);
            colorBlendState.setAttachments(colorBlendAttachmentStates);
            vertexInputState.setVertexAttributeDescriptions(vertexInputAttributeDescriptions);
            vertexInputState.setVertexBindingDescriptions(vertexInputBindingDescriptions);
            return *this;
        };

        // direct access operator
        inline VkGraphicsPipelineCreateInfo* operator->() { construct(); return &graphicsPipelineCreateInfo; };
        inline const VkGraphicsPipelineCreateInfo* operator->() const { return &graphicsPipelineCreateInfo; };
    };


    // TODO: IMAGE VIEW REGION IN MIP LEVEL
    // WILL USED FOR SIMPLER COPYING

};
