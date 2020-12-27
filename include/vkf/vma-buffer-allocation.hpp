#pragma once // #

//
#ifndef VKT_CORE_USE_VMA
#define VKT_CORE_USE_VMA
#endif

// 
#include "./buffer-allocation.hpp"

// out of core definition
#ifdef VKT_CORE_USE_VMA
#include <vma/vk_mem_alloc.h>
#endif

// 
namespace vkf {

#ifdef VKT_CORE_USE_VMA
    // 
    class VmaBufferAllocation : public BufferAllocation { public: 
        VmaBufferAllocation() {};
        VmaBufferAllocation(const vkt::uni_arg<VmaAllocator>& allocator, const vkt::uni_arg<vkh::VkBufferCreateInfo>& createInfo = vkh::VkBufferCreateInfo{}, vkt::uni_arg<VmaMemoryInfo> vmaInfo = VmaMemoryInfo{.memUsage = VMA_MEMORY_USAGE_GPU_ONLY }) { this->construct(*allocator, createInfo, vmaInfo); };

        // 
        VmaBufferAllocation(const vkt::uni_ptr<VmaBufferAllocation>& allocation) : allocation(allocation->allocation), allocationInfo(allocation->allocationInfo), allocator(allocation->allocator) { this->assign(allocation); };
        VmaBufferAllocation(const vkt::uni_ptr<BufferAllocation>& allocation) { this->assign(allocation.dyn_cast<VmaBufferAllocation>()); };

        // 
        VmaBufferAllocation(const std::shared_ptr<VmaBufferAllocation>& allocation) : allocation(allocation->allocation), allocationInfo(allocation->allocationInfo), allocator(allocation->allocator) { this->assign(vkt::uni_ptr<VmaBufferAllocation>(allocation)); };
        VmaBufferAllocation(const std::shared_ptr<BufferAllocation>& allocation) { this->assign(vkt::uni_ptr<VmaBufferAllocation>(std::dynamic_pointer_cast<VmaBufferAllocation>(allocation))); };

        // 
        ~VmaBufferAllocation() {
            if (this->buffer && this->isManaged()) {
                this->info.deviceDispatch->DeviceWaitIdle();
                vmaDestroyBuffer(allocator, this->buffer, allocation);
                this->buffer = nullptr, this->info.memory = nullptr;
            };
        };

        //
        virtual VmaBufferAllocation* construct(
            vkt::uni_arg<VmaAllocator> allocator,
            vkt::uni_arg<vkh::VkBufferCreateInfo> createInfo = vkh::VkBufferCreateInfo{},
            vkt::uni_arg<VmaMemoryInfo> memInfo = VmaMemoryInfo{ .memUsage = VMA_MEMORY_USAGE_GPU_ONLY }
        ) {
            VmaAllocationCreateInfo vmaInfo = {}; vmaInfo.usage = memInfo->memUsage;
            if (memInfo->memUsage == VMA_MEMORY_USAGE_CPU_TO_GPU || memInfo->memUsage == VMA_MEMORY_USAGE_GPU_TO_CPU) { vmaInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; };
            if (memInfo->memUsage == VMA_MEMORY_USAGE_GPU_ONLY) { createInfo->usage->eShaderDeviceAddress = 1; }; // NEEDS SHARED BIT!

            // 
            vkt::handleVk(vmaCreateBuffer(this->allocator = allocator.ref(), *createInfo, &vmaInfo, &reinterpret_cast<VkBuffer&>(this->buffer), &this->allocation, &this->allocationInfo));
            this->info.range = createInfo->size;
            this->info.memUsage = memInfo->memUsage;
            this->info.memory = allocationInfo.deviceMemory;
            this->info.offset = allocationInfo.offset;
            this->usage = createInfo->usage;

            // 
            //std::cout << "Allocation Code: " << result << std::endl;
            //std::cout << "Vma Allocation PTR:  " << this->allocation << std::endl;

            // Get Dispatch Loader From VMA Allocator Itself!
            VmaAllocatorInfo info = {};
            vmaGetAllocatorInfo(this->allocator = allocator.ref(), &info);

            // 
#ifdef VKT_CORE_USE_XVK
            this->info.deviceDispatch = memInfo->deviceDispatch;
            this->info.instanceDispatch = memInfo->instanceDispatch;

            //
            if (!this->info.deviceDispatch) { this->info.deviceDispatch = vkt::vkGlobal::device; };
            if (!this->info.instanceDispatch) { this->info.instanceDispatch = vkt::vkGlobal::instance; };

            // when loader initialized
            if (vkt::vkGlobal::initialized) {
                if (!this->info.deviceDispatch) this->info.deviceDispatch = std::make_shared<xvk::Device>(this->info.instanceDispatch, info.device);
                if (!this->info.instanceDispatch) this->info.instanceDispatch = std::make_shared<xvk::Instance>(vkt::vkGlobal::loader.get(), info.instance);
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
            return this;
        };
        
        // Dedicated 
        virtual VmaBufferAllocation& assign(const vkt::uni_ptr<VmaBufferAllocation>& allocation) {
            if (allocation->allocation) {
                vmaDestroyBuffer(allocator, *this, *allocation);  // don't assign into already allocated
            };
            this->info = allocation->info;
            this->usage = allocation->usage;
            this->buffer = allocation->buffer;
            this->allocation = allocation->allocation;
            this->allocationInfo = allocation->allocationInfo;
            this->allocator = allocation->allocator;
            return *this;
        }

        // 
        virtual VmaBufferAllocation& operator=(const vkt::uni_ptr<VmaBufferAllocation>& allocation) {
            return this->assign(allocation);
        };

        // 
        virtual bool isManaged() const override {
            return true;
        };

        // Get mapped memory
        virtual void* map() { vkt::handleVk(vmaMapMemory(this->allocator, this->allocation, &this->allocationInfo.pMappedData)); return (this->info.pMapped = this->allocationInfo.pMappedData); };
        virtual void* mapped() { if (!this->allocationInfo.pMappedData) { map(); }; return (this->info.pMapped = this->allocationInfo.pMappedData); };
        virtual void  unmap() { vmaUnmapMemory(this->allocator, this->allocation);  this->info.pMapped = this->allocationInfo.pMappedData = nullptr; };

        // Allocation
        virtual operator const VmaAllocation& () const { return allocation; };
        virtual operator const VmaAllocationInfo& () const { return allocationInfo; };

        // 
        virtual VmaBufferAllocation* address() { return this; };
        virtual const VmaBufferAllocation* address() const { return this; };

        //
        virtual vkh::VkDeviceOrHostAddressKHR deviceAddress() {
            if (!this->usage.eShaderDeviceAddress) {
                std::cerr << "Bad Device Address" << std::endl;
                assert(true);
            };
            return vkh::VkDeviceOrHostAddressKHR{ .deviceAddress = this->usage.eShaderDeviceAddress ? this->info.deviceDispatch->GetBufferDeviceAddress(vkh::VkBufferDeviceAddressInfo{.buffer = this->buffer}) : 0ull };
        };

        // 
        virtual vkh::VkDeviceOrHostAddressConstKHR deviceAddress() const {
            if (!this->usage.eShaderDeviceAddress) {
                std::cerr << "Bad Device Address" << std::endl;
                assert(true);
            };
            return vkh::VkDeviceOrHostAddressConstKHR{ .deviceAddress = this->usage.eShaderDeviceAddress ? const_cast<MemoryAllocationInfo&>(this->info).deviceDispatch->GetBufferDeviceAddress(vkh::VkBufferDeviceAddressInfo{.buffer = this->buffer}) : 0ull};
        };

    // 
    protected: friend VmaBufferAllocation; friend BufferAllocation; // 
        VmaAllocationInfo allocationInfo = {};
        VmaAllocation allocation = VK_NULL_HANDLE;
        VmaAllocator allocator = VK_NULL_HANDLE;
    };
#endif

};
