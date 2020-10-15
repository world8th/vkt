#pragma once // #

#include "./incornation.hpp"
#include "./device-extensions.hpp"
#include "./instance.hpp"


// 
namespace vkt {

    struct VktProperties {
        vkh::VkPhysicalDeviceProperties2 gProperties{};
    };

    struct VktFeatures {
        vkh::VkPhysicalDeviceFeatures2 gFeatures{};
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
    };

    class VktDevice { public:
        std::shared_ptr<VktInstance> instance = {};

        vkt::Device dispatch = {};
        
        VkPhysicalDevice physical = VK_NULL_HANDLE;
        vkh::VkDeviceCreateInfo createInfo = {};
        VkDevice device = VK_NULL_HANDLE;
        VkQueue queue = VK_NULL_HANDLE;
        VkCommandPool commandPool = VK_NULL_HANDLE;
        VkPipelineCache pipelineCache = VK_NULL_HANDLE;
        VmaAllocator allocator = {};
        VktFeatures features = {};
        VktProperties properties = {};
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

        //
        std::vector<std::string> extensions = {};
        std::vector<std::string> layers = {};

        // 
        std::vector<const char*> extensions_c_str = {};
        std::vector<const char*> layers_c_str = {};

        // 
        uint32_t queueFamilyIndex = 0;
        uint32_t version = 0;

        // 
        std::vector<uint32_t> queueFamilyIndices = {};
        vkt::MemoryAllocationInfo memoryAllocInfo = {};
        vkh::VkPhysicalDeviceMemoryProperties2 memoryProperties = {};
        std::vector<vkh::VkDeviceQueueCreateInfo> usedQueueCreateInfos = {};

        VktDevice(){
            
        };
        
        VktDevice(std::shared_ptr<VktInstance> instance) : instance(instance) {
            
        };

        operator VkDevice&(){
            return device;
        };

        operator const VkDevice&() const {
            return device;
        };

        virtual VkDevice& create(const uint32_t& deviceID = 0u, const VkSurfaceKHR& surface = VK_NULL_HANDLE){
            // 
            this->physical = instance->physicalDevices[deviceID];

            // use extensions
            std::string layerName = "";
            std::vector<std::string> wantedExtensions = std::vector<std::string>(wantedDeviceExtensions_CStr.begin(), std::end(wantedDeviceExtensions_CStr));
            std::vector<VkExtensionProperties> gpuExtensions = {};
            vkh::vsEnumerateDeviceExtensionProperties(instance->dispatch, this->physical, gpuExtensions, layerName); // TODO: vkh helper for getting
            std::vector<std::string> extensions = std::vector<std::string>(std::min(gpuExtensions.size(), wantedExtensions.size())); uint32_t extensionCount = 0u;
            for (auto& w : wantedExtensions) {
                for (auto& i : gpuExtensions) {
                    if (w.compare(i.extensionName) == 0) {
                        //extensions.emplace_back(std::string(w)); extensionCount++; break;
                        extensions[extensionCount++] = std::string(w); break;
                    };
                };
            };
            extensions.resize(extensionCount);
            this->extensions = extensions;

            // use layers
            std::vector<std::string> wantedLayers = std::vector<std::string>(wantedDeviceLayers_CStr.begin(), std::end(wantedDeviceLayers_CStr));
            std::vector<VkLayerProperties> gpuLayers = {};
            vkh::vsEnumerateDeviceLayerProperties(instance->dispatch, this->physical, gpuLayers); // TODO: vkh helper for getting
            std::vector<std::string> layers = std::vector<std::string>(std::min(gpuLayers.size(), wantedLayers.size())); uint32_t layerCount = 0u;
            for (auto& w : wantedLayers) {
                for (auto& i : gpuLayers) {
                    if (w.compare(i.layerName) == 0) {
                        //layers.emplace_back(std::string(w)); layerCount++; break;
                        layers[layerCount++] = std::string(w); break;
                    };
                };
            };
            layers.resize(layerCount);
            this->layers = layers;

            // 
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
            const uint32_t qptr = 0;
            if ((this->usedQueueCreateInfos = queueCreateInfos).size() > 0) {
                this->queueFamilyIndex = queueFamilyIndices[qptr];
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

            //
            auto resetFlag = vkh::VkCommandPoolCreateFlags{ .eResetCommandBuffer = 1 };
            this->dispatch->GetDeviceQueue(queueFamilyIndex, 0u, &this->queue);
            vkh::handleVk(this->dispatch->CreateCommandPool(vkh::VkCommandPoolCreateInfo{ .flags = resetFlag, .queueFamilyIndex = queueFamilyIndex }, nullptr, &this->commandPool));

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
            vkh::handleVk(this->dispatch->CreateDescriptorPool(vkh::VkDescriptorPoolCreateInfo{
                .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT | VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT ,
                .maxSets = 256u, .poolSizeCount = static_cast<uint32_t>(dps.size()), .pPoolSizes = dps.data()
            }, nullptr, &this->descriptorPool));

            // 
            return this->device;
        }

        // 
        VktDevice* submitUtilize(vkt::uni_arg<VkCommandBuffer> cmds, vkt::uni_arg<vkh::VkSubmitInfo> smbi) {
            return this->submitUtilize(std::vector<VkCommandBuffer>{ cmds }, smbi);
        };

        // 
        VktDevice* submitUtilize(const std::vector<VkCommandBuffer>& cmds, vkt::uni_arg<vkh::VkSubmitInfo> smbi) {
            vkt::submitUtilize(dispatch, queue, commandPool, cmds, smbi);
            return this;
        };

        // 
        VktDevice* submitOnce(const std::function<void(VkCommandBuffer&)>& cmdFn, vkt::uni_arg<vkh::VkSubmitInfo> smbi) {
            vkt::submitOnce(dispatch, queue, commandPool, cmdFn, smbi);
            return this;
        };

        // Async Version
        std::future<VktDevice*> submitOnceAsync(const std::function<void(VkCommandBuffer&)>& cmdFn, vkt::uni_arg<vkh::VkSubmitInfo> smbi) {
            return std::async(std::launch::async | std::launch::deferred, [=, this]() {
                vkt::submitOnceAsync(dispatch, queue, commandPool, cmdFn, smbi).get();
                return this;
            });
        };

        // 
        VktDevice* submitCmd(vkt::uni_arg<VkCommandBuffer> cmds, vkt::uni_arg<vkh::VkSubmitInfo> smbi) {
            return this->submitCmd(std::vector<VkCommandBuffer>{ cmds }, smbi);
        };

        // 
        VktDevice* submitCmd(const std::vector<VkCommandBuffer>& cmds, vkt::uni_arg<vkh::VkSubmitInfo> smbi) {
            vkt::submitCmd(dispatch, queue, cmds, smbi);
            return this;
        };
    };

};
