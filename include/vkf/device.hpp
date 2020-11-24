#pragma once // #

#include "./incornation.hpp"
#include "./device-extensions.hpp"
#include "./instance.hpp"


// 
namespace vkf {

    struct Properties {
        vkh::VkPhysicalDeviceProperties2 gProperties{};
    };

    struct Features {
        vkh::VkPhysicalDeviceFeatures2 gFeatures{};
        vk::PhysicalDeviceShaderAtomicFloatFeaturesEXT gAtomicFloat{};
        vk::PhysicalDeviceTransformFeedbackFeaturesEXT gTrasformFeedback{};
        vk::PhysicalDeviceRayTracingPipelineFeaturesKHR gRayTracing{};
        vk::PhysicalDeviceTexelBufferAlignmentFeaturesEXT gTexelBufferAligment{};
        vk::PhysicalDevice16BitStorageFeatures gStorage16{};
        vk::PhysicalDevice8BitStorageFeatures gStorage8{};
        vk::PhysicalDeviceDescriptorIndexingFeaturesEXT gDescIndexing{};
        vk::PhysicalDeviceFloat16Int8FeaturesKHR gFloat16U8{}; // Vulkan 1.3
        vk::PhysicalDeviceBufferDeviceAddressFeatures gDeviceAddress{};
        vk::PhysicalDeviceFragmentShaderBarycentricFeaturesNV gBarycentric{};
        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT gExtendedDynamic{};
        vk::PhysicalDeviceAccelerationStructureFeaturesKHR gAccelerationStructure{};
        vk::PhysicalDeviceInlineUniformBlockFeaturesEXT gInlineUniform{};
    };

    // TODO: DEDICATED QUEUE OBJECT
    class Device { public:
        std::shared_ptr<Instance> instance = {};

        vkt::Device dispatch = {};
        
        VkPhysicalDevice physical = VK_NULL_HANDLE;
        vkh::VkDeviceCreateInfo createInfo = {};
        VkDevice device = VK_NULL_HANDLE;
        VmaAllocator allocator = {};

        VkPipelineCache pipelineCache = VK_NULL_HANDLE;
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

        Features features = {};
        Properties properties = {};

        //
        std::vector<std::string> extensions = {};
        std::vector<std::string> layers = {};

        // 
        std::vector<const char*> extensions_c_str = {};
        std::vector<const char*> layers_c_str = {};

        // 
        uint32_t version = 0;

        // 
        std::vector<uint32_t> queueFamilyIndices = {};
        vkt::MemoryAllocationInfo memoryAllocInfo = {};
        vkh::VkPhysicalDeviceMemoryProperties2 memoryProperties = {};
        std::vector<vkh::VkDeviceQueueCreateInfo> usedQueueCreateInfos = {};

        Device(){
            
        };

        Device(std::shared_ptr<Instance> instance) : instance(instance) {
            
        };

        operator VkDevice&(){
            return device;
        };

        operator const VkDevice&() const {
            return device;
        };

        xvk::Device* operator ->() {
            return dispatch.get();
        };

        const xvk::Device* operator ->() const {
            return dispatch.get();
        };

        virtual VkDevice& create(const uint32_t& deviceID = 0u, const VkSurfaceKHR& surface = VK_NULL_HANDLE){
            // 
            this->physical = instance->physicalDevices[deviceID];

            // use extensions
            std::string layerName = "";
            std::vector<std::string> wantedExtensions = std::vector<std::string>(wantedDeviceExtensions_CStr.begin(), std::end(wantedDeviceExtensions_CStr));
            std::vector<VkExtensionProperties> gpuExtensions = {};
            vkh::vsEnumerateDeviceExtensionProperties(instance->dispatch, this->physical, gpuExtensions, layerName); // TODO: vkh helper for getting
            uint32_t extensionCount = 0u;
            for (auto& w : wantedExtensions) {
                for (auto& i : gpuExtensions) {
                    if (w.compare(i.extensionName) == 0) { extensionCount++; break; };
                };
            };
            std::vector<std::string> extensions = std::vector<std::string>(extensionCount); extensionCount = 0u;
            for (auto& w : wantedExtensions) {
                for (auto& i : gpuExtensions) {
                    if (w.compare(i.extensionName) == 0) { extensions[extensionCount++] = std::string(w); break; };
                };
            };
            this->extensions = extensions;

            // use layers
            std::vector<std::string> wantedLayers = std::vector<std::string>(wantedDeviceLayers_CStr.begin(), std::end(wantedDeviceLayers_CStr));
            std::vector<VkLayerProperties> gpuLayers = {};
            vkh::vsEnumerateDeviceLayerProperties(instance->dispatch, this->physical, gpuLayers); // TODO: vkh helper for getting
            uint32_t layerCount = 0u;
            for (auto& w : wantedLayers) {
                for (auto& i : gpuLayers) {
                    if (w.compare(i.layerName) == 0) { layerCount++; break; };
                };
            };
            std::vector<std::string> layers = std::vector<std::string>(layerCount); layerCount = 0u;
            for (auto& w : wantedLayers) {
                for (auto& i : gpuLayers) {
                    if (w.compare(i.layerName) == 0) { layers[layerCount++] = std::string(w); break; };
                };
            };
            this->layers = layers;

            // 
            features.gAccelerationStructure.pNext = &features.gInlineUniform;
            features.gAtomicFloat.pNext = &features.gAccelerationStructure;
            features.gExtendedDynamic.pNext = &features.gAtomicFloat;
            features.gBarycentric.pNext = &features.gExtendedDynamic;
            features.gRayTracing.pNext = &features.gBarycentric;
            features.gTrasformFeedback.pNext = &features.gRayTracing;
            features.gDeviceAddress.pNext = &features.gTrasformFeedback;
            features.gTexelBufferAligment.pNext = &features.gDeviceAddress;
            features.gFloat16U8.pNext = &features.gTexelBufferAligment;
            features.gStorage8.pNext = &features.gFloat16U8;
            features.gStorage16.pNext = &features.gStorage8;
            features.gDescIndexing.pNext = &features.gStorage16;
            features.gFeatures.pNext = &features.gDescIndexing;

            // 
            vkh::vsGetPhysicalDeviceFeatures2(instance->dispatch, this->physical, features.gFeatures);
            vkh::vsGetPhysicalDeviceProperties2(instance->dispatch, this->physical, properties.gProperties);
            vkh::vsGetPhysicalDeviceMemoryProperties2(instance->dispatch, this->physical, this->memoryProperties);

            // get features and queue family properties
            std::vector<vkh::VkQueueFamilyProperties> gpuQueueProps = {};
            vkh::vsGetPhysicalDeviceQueueFamilyProperties(instance->dispatch, this->physical, gpuQueueProps); // TODO: vkh helper for getting

            // queue family initial
            float priority = 1.0f;
            int32_t computeFamilyIndex = -1, graphicsFamilyIndex = -1;
            auto queueCreateInfos = std::vector<vkh::VkDeviceQueueCreateInfo>();

    #ifdef VKT_USE_GLFW
            for (auto queuefamily : gpuQueueProps) {
                graphicsFamilyIndex++;

                VkBool32 support = false;
                if (surface) {
                    vkh::handleVk(instance->dispatch->GetPhysicalDeviceSurfaceSupportKHR(this->physical, graphicsFamilyIndex, surface, &support));
                };
                if (queuefamily.queueFlags->eCompute && queuefamily.queueFlags->eGraphics && (support || !surface)) {
                    queueCreateInfos.push_back(vkh::VkDeviceQueueCreateInfo{ .queueFamilyIndex = uint32_t(computeFamilyIndex), .queueCount = 1, .pQueuePriorities = &priority });
                    queueFamilyIndices.push_back(uint32_t(graphicsFamilyIndex));
                    queueCreateInfos.back().queueFamilyIndex = uint32_t(graphicsFamilyIndex);
                    break;
                };
            };
    #else
            for (auto queuefamily : gpuQueueProps) {
                computeFamilyIndex++;
                if (queuefamily.queueFlags->eCompute) { // TODO: vkh helper for getting, bitfeilds support
                    queueCreateInfos.push_back(vkh::VkDeviceQueueCreateInfo{ .queueFamilyIndex = uint32_t(computeFamilyIndex), .queueCount = 1, .pQueuePriorities = &priority });
                    queueFamilyIndices.push_back(uint32_t(computeFamilyIndex));
                    queueCreateInfos.back().queueFamilyIndex = uint32_t(computeFamilyIndex);
                    break;
                };
            };
    #endif

            //
            extensions_c_str = std::vector<const char*>(extensionCount);
            layers_c_str = std::vector<const char*>(layerCount);

            // 
            for (uint32_t i = 0; i < extensionCount; i++) { extensions_c_str[i] = extensions[i].c_str(); };
            for (uint32_t i = 0; i < layerCount; i++) { layers_c_str[i] = layers[i].c_str(); };

            // return device with queue pointer
            if ((this->usedQueueCreateInfos = queueCreateInfos).size() > 0) {
                this->dispatch = std::make_shared<xvk::Device>(instance->dispatch, this->physical, (createInfo = vkh::VkDeviceCreateInfo{
                    .pNext = reinterpret_cast<VkPhysicalDeviceFeatures2*>(&features.gFeatures),
                    .queueCreateInfoCount = uint32_t(this->usedQueueCreateInfos.size()),
                    .pQueueCreateInfos = reinterpret_cast<vkh::VkDeviceQueueCreateInfo*>(this->usedQueueCreateInfos.data()),
                    .enabledLayerCount = uint32_t(layers_c_str.size()),
                    .ppEnabledLayerNames = layers_c_str.data(),
                    .enabledExtensionCount = uint32_t(extensions_c_str.size()),
                    .ppEnabledExtensionNames = extensions_c_str.data(),
                }));
                vkh::handleVk(this->dispatch->CreatePipelineCache(vkh::VkPipelineCacheCreateInfo(), nullptr, &this->pipelineCache));
                this->device = this->dispatch->handle;
                vkt::vkGlobal::device = this->dispatch.get_shared();
                //vkt::vkGlobal::volkDevice();
            };

            // REMAP WITH XVK AGAIN!
            VmaVulkanFunctions func = {};
            func.vkAllocateMemory = this->dispatch->vkAllocateMemory;
            func.vkBindBufferMemory = this->dispatch->vkBindBufferMemory;
            func.vkBindBufferMemory2KHR = this->dispatch->vkBindBufferMemory2;
            func.vkBindImageMemory = this->dispatch->vkBindImageMemory;
            func.vkBindImageMemory2KHR = this->dispatch->vkBindImageMemory2;
            func.vkCmdCopyBuffer = this->dispatch->vkCmdCopyBuffer;
            func.vkCreateBuffer = this->dispatch->vkCreateBuffer;
            func.vkCreateImage = this->dispatch->vkCreateImage;
            func.vkDestroyBuffer = this->dispatch->vkDestroyBuffer;
            func.vkDestroyImage = this->dispatch->vkDestroyImage;
            func.vkFlushMappedMemoryRanges = this->dispatch->vkFlushMappedMemoryRanges;
            func.vkFreeMemory = this->dispatch->vkFreeMemory;
            func.vkGetBufferMemoryRequirements = this->dispatch->vkGetBufferMemoryRequirements;
            func.vkGetBufferMemoryRequirements2KHR = this->dispatch->vkGetBufferMemoryRequirements2;
            func.vkGetImageMemoryRequirements = this->dispatch->vkGetImageMemoryRequirements;
            func.vkGetImageMemoryRequirements2KHR = this->dispatch->vkGetImageMemoryRequirements2;
            func.vkGetPhysicalDeviceMemoryProperties = instance->dispatch->vkGetPhysicalDeviceMemoryProperties;
            func.vkGetPhysicalDeviceMemoryProperties2KHR = instance->dispatch->vkGetPhysicalDeviceMemoryProperties2;
            func.vkGetPhysicalDeviceProperties = instance->dispatch->vkGetPhysicalDeviceProperties;
            func.vkInvalidateMappedMemoryRanges = this->dispatch->vkInvalidateMappedMemoryRanges;
            func.vkMapMemory = this->dispatch->vkMapMemory;
            func.vkUnmapMemory = this->dispatch->vkUnmapMemory;

            // 
            VmaAllocatorCreateInfo vma_info = {};
            vma_info.pVulkanFunctions = &func;
            vma_info.device = this->device;
            vma_info.instance = *this->instance;
            vma_info.physicalDevice = this->physical;
            vma_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
            vkh::handleVk(vmaCreateAllocator(&vma_info, &this->allocator));

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

            //
            vkh::VkDescriptorPoolInlineUniformBlockCreateInfoEXT inlineUniformPool = {
                .maxInlineUniformBlockBindings = 256u
            };

            //
            vkh::handleVk(this->dispatch->CreateDescriptorPool(vkh::VkDescriptorPoolCreateInfo{
                .pNext = &inlineUniformPool,
                .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT | VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT, .maxSets = 256u, 
            }.also([=](vkh::VkDescriptorPoolCreateInfo it) {
                it.setPoolSizes(dps);
                return it;
            }), nullptr, & this->descriptorPool));

            // 
            return this->device;
        }
        
    };

};
