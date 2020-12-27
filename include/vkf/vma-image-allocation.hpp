#pragma once // #

//
#ifndef VKT_CORE_USE_VMA
#define VKT_CORE_USE_VMA
#endif

// 
#include "./image-allocation.hpp"

// out of core definition
#ifdef VKT_CORE_USE_VMA
#include <vma/vk_mem_alloc.h>
#endif

// 
namespace vkf {

#ifdef VKT_CORE_USE_VMA
    // 
    class VmaImageAllocation : public ImageAllocation {
    public: friend VmaImageAllocation; friend ImageAllocation;// 
        VmaImageAllocation() {};
        VmaImageAllocation(vkt::uni_arg<VmaAllocator> allocator, vkt::uni_arg<vkh::VkImageCreateInfo> createInfo = vkh::VkImageCreateInfo{}, vkt::uni_arg<VmaMemoryInfo> vmaInfo = VmaMemoryInfo{ .memUsage = VMA_MEMORY_USAGE_GPU_ONLY }) { this->construct(allocator, createInfo, vmaInfo); };

        // 
        VmaImageAllocation(const vkt::uni_ptr<VmaImageAllocation>& allocation) : allocation(allocation->allocation), allocationInfo(allocation->allocationInfo), allocator(allocation->allocator) { *this = allocation; };
        VmaImageAllocation(const vkt::uni_ptr<ImageAllocation>& allocation) { *this = allocation.dyn_cast<VmaImageAllocation>(); };

        // 
        VmaImageAllocation(const std::shared_ptr<VmaImageAllocation>& allocation) : allocation(allocation->allocation), allocationInfo(allocation->allocationInfo), allocator(allocation->allocator) { *this = vkt::uni_ptr<VmaImageAllocation>(allocation); };
        VmaImageAllocation(const std::shared_ptr<ImageAllocation>& allocation) { *this = dynamic_cast<VmaImageAllocation&>(*vkt::uni_ptr<ImageAllocation>(allocation)); };

        //
        ~VmaImageAllocation() {
            if (this->image && this->isManaged()) {
                this->info.deviceDispatch->DeviceWaitIdle();
                vmaDestroyImage(allocator, this->image, allocation);
                this->image = nullptr, this->info.memory = nullptr;
            };
        };

        // 
        virtual VmaImageAllocation* construct(
            vkt::uni_arg<VmaAllocator> allocator,
            vkt::uni_arg<vkh::VkImageCreateInfo> createInfo = vkh::VkImageCreateInfo{},
            vkt::uni_arg<VmaMemoryInfo> memInfo = VmaMemoryInfo{ .memUsage = VMA_MEMORY_USAGE_GPU_ONLY }
        ) {
            VmaAllocationCreateInfo vmaInfo = {}; vmaInfo.usage = memInfo->memUsage;
            if (memInfo->memUsage == VMA_MEMORY_USAGE_CPU_TO_GPU || memInfo->memUsage == VMA_MEMORY_USAGE_GPU_TO_CPU) { vmaInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; };
            vkt::handleVk(vmaCreateImage(this->allocator = allocator.ref(), *createInfo, &vmaInfo, &reinterpret_cast<VkImage&>(image), &this->allocation, &this->allocationInfo));
            this->info.initialLayout = VkImageLayout(createInfo->initialLayout);
            this->info.range = allocationInfo.size;
            this->info.memory = allocationInfo.deviceMemory;
            this->info.memUsage = memInfo->memUsage;

            // Get Dispatch Loader From VMA Allocator Itself!
            VmaAllocatorInfo info = {};
            vmaGetAllocatorInfo(this->allocator = allocator.ref(), &info);
            //this->info.dispatch = VkDispatchLoaderDynamic(info.instance, vkGetInstanceProcAddr, this->info.device = info.device, vkGetDeviceProcAddr); // 

            // 
#ifdef VKT_CORE_USE_XVK
            this->info.instanceDispatch = memInfo->instanceDispatch;
            this->info.deviceDispatch = memInfo->deviceDispatch;

            //
            if (!this->info.deviceDispatch) { this->info.deviceDispatch = vkt::vkGlobal::device; };
            if (!this->info.instanceDispatch) { this->info.instanceDispatch = vkt::vkGlobal::instance; };

            // when loader initialized
            if (vkt::vkGlobal::initialized) {
                if (!this->info.instanceDispatch) this->info.instanceDispatch = std::make_shared<xvk::Instance>(vkt::vkGlobal::loader.get(), info.instance);
                if (!this->info.deviceDispatch) this->info.deviceDispatch = std::make_shared<xvk::Device>(this->info.instanceDispatch, info.device);
            };

            // reload device and instance
            if (!this->info.device) { this->info.device = info.device; };
            if (!this->info.instance) { this->info.instance = info.instance; };
#endif

            // 
            if (createInfo->queueFamilyIndexCount) {
                this->info.queueFamilyIndices = std::vector<uint32_t>(createInfo->queueFamilyIndexCount);
                memcpy(this->info.queueFamilyIndices.data(), createInfo->pQueueFamilyIndices, sizeof(uint32_t) * createInfo->queueFamilyIndexCount);
            };

            //
            this->createInfo = createInfo;

            // 
            return this;
        };

        // 
        virtual VmaImageAllocation& operator=(const VmaImageAllocation& allocation) {
            if (allocation.allocation) {
                vmaDestroyImage(allocator, *this, allocation); // don't assign into already allocated
            };
            this->image = allocation.image;
            this->allocation = allocation.allocation;
            this->allocationInfo = allocation.allocationInfo;
            this->allocator = allocation.allocator;
            this->info = allocation.info;

            //this->info.device = this->_getDevice();
            return *this;
        };

        virtual bool isManaged() const override {
            return true;
        };

        // Allocation
        virtual operator const VmaAllocation& () const { return allocation; };
        virtual operator const VmaAllocationInfo& () const { return allocationInfo; };

        // 
        virtual operator VmaAllocation& () { return allocation; };
        virtual operator VmaAllocationInfo& () { return allocationInfo; };

        // 
        virtual VmaImageAllocation* address() { return this; };
        virtual const VmaImageAllocation* address() const { return this; };

    // 
    protected: friend VmaImageAllocation; friend ImageAllocation; // 
        VmaAllocationInfo allocationInfo = {};
        VmaAllocation allocation = VK_NULL_HANDLE;
        VmaAllocator allocator = VK_NULL_HANDLE;
    };
#endif

};
