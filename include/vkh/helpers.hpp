#pragma once // #

//#define VKT_CORE_USE_XVK
#include <vkt3/core.hpp>
#include <vkh/structures.hpp>
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

#ifdef VKT_CORE_USE_XVK
    inline auto vsEnumeratePhysicalDevices(vkt::Instance instance) {
        //uint32_t count = 0u; handleVk(vkEnumeratePhysicalDevices(instance->handle, &count, nullptr));
        uint32_t count = 0u; handleVk(instance->EnumeratePhysicalDevices(&count, nullptr));
        std::vector<VkPhysicalDevice> data(count);
        //handleVk(vkEnumeratePhysicalDevices(instance->handle, &count, data.data()));
        handleVk(instance->EnumeratePhysicalDevices(&count, data.data()));
        return data;
    };

    inline auto vsEnumerateDeviceExtensionProperties(vkt::Instance instance, const VkPhysicalDevice& physicalDevice, const std::string layerName = "") {
        uint32_t count = 0u; handleVk(instance->vkEnumerateDeviceExtensionProperties(physicalDevice, layerName.c_str(), &count, nullptr));
        std::vector<VkExtensionProperties> data(count);
        handleVk(instance->vkEnumerateDeviceExtensionProperties(physicalDevice, layerName.c_str(), &count, data.data()));
        return data;
    };

    inline auto vsEnumerateDeviceLayerProperties(vkt::Instance instance, const VkPhysicalDevice& physicalDevice) {
        uint32_t count = 0u; handleVk(instance->EnumerateDeviceLayerProperties(physicalDevice, &count, nullptr));
        std::vector<VkLayerProperties> data(count);
        handleVk(instance->EnumerateDeviceLayerProperties(physicalDevice, &count, data.data()));
        return data;
    };

    inline auto vsGetPhysicalDeviceQueueFamilyProperties(vkt::Instance instance, const VkPhysicalDevice& physicalDevice) {
        uint32_t count = 0u; (instance->GetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr));
        std::vector<vkh::VkQueueFamilyProperties> data(count);
        (instance->GetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, reinterpret_cast<::VkQueueFamilyProperties*>(data.data())));
        return data;
    };

    inline auto vsEnumerateInstanceExtensionProperties(vkt::uni_ptr<xvk::Loader> loader, const std::string layerName = "") {
        uint32_t count = 0u; handleVk(loader->vkEnumerateInstanceExtensionProperties(layerName.c_str(), &count, nullptr));
        std::vector<VkExtensionProperties> data(count);
        handleVk(loader->vkEnumerateInstanceExtensionProperties(layerName.c_str(), &count, data.data()));
        return data;
    };

    inline auto vsEnumerateInstanceLayerProperties(vkt::uni_ptr<xvk::Loader> loader) {
        uint32_t count = 0u; handleVk(loader->vkEnumerateInstanceLayerProperties(&count, nullptr));
        std::vector<VkLayerProperties> data(count);
        handleVk(loader->vkEnumerateInstanceLayerProperties(&count, data.data()));
        return data;
    };

    inline auto vsEnumerateInstanceVersion(vkt::uni_ptr<xvk::Loader> loader) {
        uint32_t version = 0u; handleVk(loader->vkEnumerateInstanceVersion(&version));
        return version;
    };

    inline auto vsGetPhysicalDeviceSurfaceFormatsKHR(vkt::Instance instance, const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface) { // TODO: V2
        uint32_t count = 0u; handleVk(instance->GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, nullptr));
        std::vector<VkSurfaceFormatKHR> data(count);
        handleVk(instance->GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, reinterpret_cast<VkSurfaceFormatKHR*>(data.data())));
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
        uint32_t count = 0u; handleVk(instance->GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, nullptr));
        std::vector<VkPresentModeKHR> data(count);
        handleVk(instance->GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, reinterpret_cast<::VkPresentModeKHR*>(data.data())));
        return data;
    };
#endif


    // LEGACY Functional
    class VsRayTracingPipelineCreateInfoHelperNV {
    protected:
        VkRayTracingShaderGroupCreateInfoNV raygenShaderGroup = {};
        std::vector<VkPipelineShaderStageCreateInfo> stages = {};
        std::vector<VkRayTracingShaderGroupCreateInfoNV> missShaderGroups = {};
        std::vector<VkRayTracingShaderGroupCreateInfoNV> hitShaderGroups = {};
        std::vector<VkRayTracingShaderGroupCreateInfoNV> compiledShaderGroups = {};

    public: // get offsets of shader groups
        VkRayTracingPipelineCreateInfoNV vkInfo = {};
        uintptr_t raygenOffsetIndex() { return 0u; };
        uintptr_t missOffsetIndex() { return 1u; };
        uintptr_t hitOffsetIndex() { return missShaderGroups.size() + missOffsetIndex(); };
        uintptr_t groupCount() { return missShaderGroups.size() + hitShaderGroups.size() + 1u; };

        // 
        VsRayTracingPipelineCreateInfoHelperNV(const VkRayTracingPipelineCreateInfoNV& info = {}) : vkInfo(info) {};

        // result groups
        inline std::vector<VkRayTracingShaderGroupCreateInfoNV>& compileGroups() {
            compiledShaderGroups = { raygenShaderGroup };
            for (auto& group : missShaderGroups) { compiledShaderGroups.push_back(group); };
            for (auto& group : hitShaderGroups) { compiledShaderGroups.push_back(group); };
            return compiledShaderGroups;
        };

        // WARNING: Only One Hit Group Supported At Once
        inline VsRayTracingPipelineCreateInfoHelperNV& addShaderStages(const std::vector<VkPipelineShaderStageCreateInfo>& stages_in = {}, const VkRayTracingShaderGroupTypeNV& prior_group_type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_NV) {
            for (auto& stage : stages_in) {
                if (stage.stage == VK_SHADER_STAGE_RAYGEN_BIT_NV) {
                    const uintptr_t lastIdx = stages.size(); stages.push_back(stage);
                    raygenShaderGroup.generalShader = uint32_t(lastIdx);
                };
            };
            
            uintptr_t groupIdx = ~0U;
            for (auto& stage : stages_in) {
                if (stage.stage == VK_SHADER_STAGE_MISS_BIT_NV) {
                    const uintptr_t lastIdx = stages.size(); stages.push_back(stage);
                    groupIdx = missShaderGroups.size(); missShaderGroups.push_back({});
                    //if (group_idx == -1U) { group_idx = miss_shader_groups.size(); miss_shader_groups.push_back({}); };
                    missShaderGroups[groupIdx].generalShader = uint32_t(lastIdx);//break;
                };
            };
             
            groupIdx = ~0U;  // Only One Hit Group Supported At Once
            for (auto& stage : stages_in) {
                if (stage.stage == VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV || stage.stage == VK_SHADER_STAGE_ANY_HIT_BIT_NV || stage.stage == VK_SHADER_STAGE_INTERSECTION_BIT_NV) {
                    const uintptr_t lastIdx = stages.size(); stages.push_back(stage);
                    if (groupIdx == ~0U) { groupIdx = hitShaderGroups.size(); hitShaderGroups.push_back({}); };
                    auto& group = hitShaderGroups[groupIdx];
                    if (stage.stage == VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV) {
                        group.type = prior_group_type;
                        group.closestHitShader = uint32_t(lastIdx);;
                    };
                    if (stage.stage == VK_SHADER_STAGE_ANY_HIT_BIT_NV) {
                        group.type = prior_group_type;
                        group.anyHitShader = uint32_t(lastIdx);;
                    };
                    if (stage.stage == VK_SHADER_STAGE_INTERSECTION_BIT_NV) {
                        group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT_GROUP_NV,
                            group.intersectionShader = uint32_t(lastIdx);;
                    };
                };
            };

            return *this;
        };

        // 
        inline VkRayTracingPipelineCreateInfoNV& format() {
            auto& groups = compileGroups();
            vkInfo.pGroups = groups.data();
            vkInfo.pStages = stages.data();
            vkInfo.stageCount = uint32_t(stages.size());
            vkInfo.groupCount = uint32_t(groups.size());
            //vkInfo.maxRecursionDepth = 4u;
            return vkInfo;
        };

        // 
        inline operator const ::VkRayTracingPipelineCreateInfoNV* () const { return vkInfo; };
        inline operator const VkRayTracingPipelineCreateInfoNV* () { return &vkInfo; };

        // 
        inline operator ::VkRayTracingPipelineCreateInfoNV* () { return format(); };
        inline operator VkRayTracingPipelineCreateInfoNV* () { return &format(); };

        // 
        inline operator const ::VkRayTracingPipelineCreateInfoNV& () const { return vkInfo; };
        inline operator const VkRayTracingPipelineCreateInfoNV& () { return vkInfo; };

        // 
        inline operator ::VkRayTracingPipelineCreateInfoNV& () { return format(); };
        inline operator VkRayTracingPipelineCreateInfoNV& () { return format(); };

#ifdef VULKAN_HPP // 
        inline operator const vk::RayTracingPipelineCreateInfoNV& () const { return vkInfo; };
        inline operator vk::RayTracingPipelineCreateInfoNV& () { return format(); };
#endif
    };


#ifdef VK_ENABLE_BETA_EXTENSIONS
    // TODO: REMOVE CODE TAFTOLOGY
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
        uintptr_t missOffsetIndex() { return 1u; };
        uintptr_t hitOffsetIndex() { return missShaderGroups.size()+missOffsetIndex(); };
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
        inline operator const ::VkRayTracingPipelineCreateInfoKHR* () const { return vkInfo; };
        inline operator const VkRayTracingPipelineCreateInfoKHR* () { return &vkInfo; };

        // 
        inline operator ::VkRayTracingPipelineCreateInfoKHR* () { return format(); };
        inline operator VkRayTracingPipelineCreateInfoKHR* () { return &format(); };

        // 
        inline operator const ::VkRayTracingPipelineCreateInfoKHR&() const { return vkInfo; };
        inline operator const VkRayTracingPipelineCreateInfoKHR&() { return vkInfo; };

        // 
        inline operator ::VkRayTracingPipelineCreateInfoKHR&() { return format(); };
        inline operator VkRayTracingPipelineCreateInfoKHR&() { return format(); };

#ifdef VULKAN_HPP // Vulkan-HPP
        inline operator const vk::RayTracingPipelineCreateInfoKHR& () const { return vkInfo; };
        inline operator vk::RayTracingPipelineCreateInfoKHR& () { return format(); };
#endif
    };
#endif

    // 
    template<class T = uint8_t>
    struct VsDescriptorHandle { //using T = T;
        VkDescriptorUpdateTemplateEntry entry_t = {};
        std::shared_ptr<std::vector<uint8_t>> V_t = {};
        uintptr_t field_t = 0u; // Byte Based
        size_t stride_t = sizeof(T);

        // 
        template<class C = T>
          VsDescriptorHandle(const VsDescriptorHandle<C>& handle) : entry_t(handle.entry_t), field_t(handle.field_t), stride_t(handle.stride_t), V_t(handle.V_t) {};
          VsDescriptorHandle(const VkDescriptorUpdateTemplateEntry& entry_t, const uintptr_t& field_t = 0ull, const std::shared_ptr<std::vector<uint8_t>>& V_t = {}, const size_t stride_t = sizeof(T)) : V_t(V_t), entry_t(entry_t), field_t(field_t), stride_t(stride_t) {};
         ~VsDescriptorHandle() {  };

        // any buffers and images can `write` into types
        template<class M = T> inline VsDescriptorHandle<M> offset(const uint32_t& idx = 0u) { return VsDescriptorHandle<M>(entry_t, stride_t*std::min(idx,entry_t.descriptorCount)+field_t, V_t); };
        template<class M = T> inline const VsDescriptorHandle<M> offset(const uint32_t& idx = 0u) const { return VsDescriptorHandle<M>(entry_t, stride_t*std::min(idx,entry_t.descriptorCount)+field_t, V_t); };
        inline const uint32_t& size() const { return entry_t.descriptorCount; };

        // 
        template<class C = T> inline VsDescriptorHandle<T>& operator=(const VsDescriptorHandle<C>& d) { stride_t = d.stride_t, entry_t = d.entry_t, field_t = d.field_t, V_t = d.V_t; return *this; };
        //template<class C = T> inline VsDescriptorHandle<T>& operator=(const C& d) { *reinterpret_cast<C*>(&(*V_t)[this->field_t]) = d; return *this; };
        //inline VsDescriptorHandle<T>& operator=(const T& d) { *reinterpret_cast<T*>(&(*V_t)[field_t]) = d; return *this; };
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
    };

    // TODO: REMOVE CODE TAFTOLOGY
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
        inline VsDescriptorHandle<>& pushDescription( const VkDescriptorUpdateTemplateEntry& entry = {} ) {
            if (entry.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER || entry.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                return _push_description<VkDescriptorBufferInfo>(entry);
            } else
            if (entry.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER || entry.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER) {
                return _push_description<VkBufferView>(entry);
            } else 
            if (entry.descriptorType == VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR) {
                return _push_description<VkAccelerationStructureKHR>(entry);
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
                const auto& entry = hndl.entry_t;
                const auto& pt0 = entry.offset;

                if (entry.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER || entry.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) { // Map Buffers
                    writes[i].pBufferInfo = reinterpret_cast<VkDescriptorBufferInfo*>(heap->data()+pt0);
                } else
                if (entry.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER || entry.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER) { // Map Buffer Views
                    writes[i].pTexelBufferView = reinterpret_cast<VkBufferView*>(heap->data()+pt0);
                } else
                if (entry.descriptorType == VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR) { // Map Accelerator Structures (KHR CUSTOM MAP)
                    writes_acs[i].pAccelerationStructures = reinterpret_cast<VkAccelerationStructureKHR*>(heap->data()+pt0);
                    writes[i].pNext = &writes_acs[i];
                } else { // Map Images, Samplers, Combinations...
                    writes[i].pImageInfo = reinterpret_cast<VkDescriptorImageInfo*>(heap->data()+pt0);
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
            heap->resize(pt0+sizeof(T)*entry.descriptorCount, 0u);
            handles.push_back(VsDescriptorHandle<T>(entry, uintptr_t(pt0), heap));
            writes.push_back({
                .dstSet = this->set,
                .dstBinding = entry.dstBinding,
                .dstArrayElement = entry.dstArrayElement,
                .descriptorCount = entry.descriptorCount,
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
        std::vector<VkWriteDescriptorSetAccelerationStructureKHR> writes_acs = {};
        std::vector<VkWriteDescriptorSet> writes = {};
    };
 

    // TODO: REMOVE CODE TAFTOLOGY
    class VsDescriptorSetLayoutCreateInfoHelper { public: 
        VsDescriptorSetLayoutCreateInfoHelper(const VkDescriptorSetLayoutCreateInfo& info = {}) : vk_info(info) {
            vk_info.pNext = &flags_info;
        };

        // 
        inline VsDescriptorSetLayoutCreateInfoHelper& pushBinding(const VkDescriptorSetLayoutBinding& binding = {}, const VkDescriptorBindingFlags& flags = {}){
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
        inline operator const ::VkDescriptorSetLayoutCreateInfo* () const { return vk_info; };
        inline operator ::VkDescriptorSetLayoutCreateInfo* () { return format(); };

        // 
        inline operator const VkDescriptorSetLayoutCreateInfo* () const { return &vk_info; };
        inline operator VkDescriptorSetLayoutCreateInfo* () { return &format(); };

        // 
        inline const VkDescriptorSetLayoutCreateInfo& get() const { return vk_info; };
        inline VkDescriptorSetLayoutCreateInfo& get() { return format(); };

    protected: // 
        VkDescriptorSetLayoutCreateInfo vk_info = {};
        VkDescriptorSetLayoutBindingFlagsCreateInfo flags_info = {};
        std::vector<VkDescriptorSetLayoutBinding> bindings = {};
        std::vector<VkDescriptorBindingFlags> binding_flags = {};
    };

    // TODO: REMOVE CODE TAFTOLOGY
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

    // 
    inline VkResult AllocateDescriptorSetWithUpdate(const vkt::Device& device, vkh::VsDescriptorSetCreateInfoHelper& helper, VkDescriptorSet& descriptorSet, bool& protection) {
        if (!protection) {
            // Corrupt... 
            //if (descriptorSet) { vkh::handleVk(device->vkFreeDescriptorSets(device->handle, helper, 1u, &descriptorSet)); descriptorSet = {}; };

            // 
            bool created = false;
            if (!descriptorSet) { vkh::handleVk(device->vkAllocateDescriptorSets(device->handle, helper, &descriptorSet)); created = true; };

            //
            if (descriptorSet && created) {
                const auto& writes = helper.setDescriptorSet(descriptorSet).mapWriteDescriptorSet();
                device->vkUpdateDescriptorSets(device->handle, uint32_t(writes.size()), reinterpret_cast<const ::VkWriteDescriptorSet*>(writes.data()), 0u, nullptr);
            };

            // 
            protection = true;
        };

        // 
        return VK_SUCCESS;
    };


};
