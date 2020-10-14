#pragma once // #

#include "./incornation.hpp"
#include "./device-extensions.hpp"
#include "./instance.hpp"

// 
namespace vkt {

    class VktDevice { public:
        std::shared_ptr<VktInstance> instance = {};

        vkt::Device dispatch = {};
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkcreateInfoInfo createInfo = {};
        VkDevice device = VK_NULL_HANDLE;
        VkQueue queue = VK_NULL_HANDLE;
        VkCommandPool commandPool = VK_NULL_HANDLE;
        VkPipelineCache pipelineCache = VK_NULL_HANDLE;
        VmaAllocator allocator = {};

        // 
        uint32_t queueFamilyIndex = 0;
        uint32_t version = 0;

        // 
        std::vector<VkPhysicalDevice> physicalDevices = {};
        std::vector<uint32_t> queueFamilyIndices = {};
        vkt::MemoryAllocationInfo memoryAllocInfo = {};

        VktDevice(){
            
        };
        
        VktDevice(std::shared_ptr<VktInstance> instance) : instance(instance) {
            
        };

        virtual VkDevice& create(const uint32_t& deviceID = 0u){
            // 
            physicalDevice = instance->physicalDevices[deviceID];

            // use extensions
            auto deviceExtensions = std::vector<const char*>();
            auto gpuExtensions = vkh::vsEnumerateDeviceExtensionProperties(instance->dispatch, physicalDevice); // TODO: vkh helper for getting
            for (auto w : wantedDeviceExtensions) {
                for (auto i : gpuExtensions) {
                    if (std::string(i.extensionName).compare(w) == 0) {
                        deviceExtensions.emplace_back(w); break;
                    };
                };
            };

            // use layers
            auto deviceLayers = std::vector<const char*>();
            auto gpuLayers = vkh::vsEnumerateDeviceLayerProperties(instance->dispatch, physicalDevice); // TODO: vkh helper for getting
            for (auto w : wantedDeviceValidationLayers) {
                for (auto i : gpuLayers) {
                    if (std::string(i.layerName).compare(w) == 0) {
                        deviceLayers.emplace_back(w); break;
                    };
                };
            };

            // 
            this->usedDeviceExtensions = deviceExtensions;
            this->usedDeviceLayers = deviceLayers;

            // 
            this->gExtendedDynamic.pNext = &this->gAtomicFloat;
            this->gBarycentric.pNext = &this->gExtendedDynamic;
            this->gRayTracing.pNext = &this->gBarycentric;
            this->gTrasformFeedback.pNext = &this->gRayTracing;
            this->gDeviceAddress.pNext = &this->gTrasformFeedback;
            this->gTexelBufferAligment.pNext = &this->gDeviceAddress;
            this->gFloat16U8.pNext = &this->gTexelBufferAligment;
            this->gStorage8.pNext = &this->gFloat16U8;
            this->gStorage16.pNext = &this->gStorage8;
            this->gDescIndexing.pNext = &this->gStorage16;
            this->gFeatures.pNext = &this->gDescIndexing;

            // 
            vkh::vsGetPhysicalDeviceFeatures2(instance->dispatch, physicalDevice, this->gFeatures);
            vkh::vsGetPhysicalDeviceProperties2(instance->dispatch, physicalDevice, this->gProperties);
            vkh::vsGetPhysicalDeviceMemoryProperties2(instance->dispatch, physicalDevice, this->memoryProperties);

            // get features and queue family properties
            auto gpuQueueProps = vkh::vsGetPhysicalDeviceQueueFamilyProperties(instance->dispatch, physicalDevice); // TODO: vkh helper for getting

            // queue family initial
            float priority = 1.0f;
            int32_t computeFamilyIndex = -1, graphicsFamilyIndex = -1;
            auto queueCreateInfos = std::vector<vkh::VkDeviceQueueCreateInfo>();

    #ifdef VKT_USE_GLFW
            for (auto queuefamily : gpuQueueProps) {
                graphicsFamilyIndex++;

                VkBool32 support = false;
                //vkh::handleVk(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, graphicsFamilyIndex, surface(), &support));
                const auto& surface = this->surface();
                if (surface) {
                    vkh::handleVk(instance->dispatch->GetPhysicalDeviceSurfaceSupportKHR(physicalDevice, graphicsFamilyIndex, surface, &support));
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

            // return device with queue pointer
            const uint32_t qptr = 0;
            if ((this->usedQueueCreateInfos = queueCreateInfos).size() > 0) {
                this->queueFamilyIndex = queueFamilyIndices[qptr];
                this->dispatch = std::make_shared<xvk::Device>(instance->dispatch, this->physicalDevice, (createInfo = vkh::VkcreateInfoInfo{
                    .pNext = reinterpret_cast<VkPhysicalDeviceFeatures2*>(&gFeatures),
                    .queueCreateInfoCount = uint32_t(this->usedQueueCreateInfos.size()),
                    .pQueueCreateInfos = reinterpret_cast<vkh::VkDeviceQueueCreateInfo*>(this->usedQueueCreateInfos.data()),
                    .enabledLayerCount = uint32_t(this->usedDeviceLayers.size()),
                    .ppEnabledLayerNames = this->usedDeviceLayers.data(),
                    .enabledExtensionCount = uint32_t(this->usedDeviceExtensions.size()),
                    .ppEnabledExtensionNames = this->usedDeviceExtensions.data(),
                    //.pEnabledFeatures = &(VkPhysicalDeviceFeatures&)(gFeatures.features)
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
            vma_info.instance = this->instance;
            vma_info.physicalDevice = this->physicalDevice;
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

            return this->device;
        }


    };

};
