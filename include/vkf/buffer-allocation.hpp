#pragma once // #

//
#ifndef VKT_CORE_USE_VMA
#define VKT_CORE_USE_VMA
#endif

//
#include "./memory-allocation.hpp"

// 
namespace vkf {

    //class VmaBufferAllocation;

    // 
    //class VmaBufferAllocation;
    class BufferAllocation : public std::enable_shared_from_this<BufferAllocation> { public:
        BufferAllocation() {};
        BufferAllocation(vkt::uni_arg<vkh::VkBufferCreateInfo> createInfo, vkt::uni_arg<MemoryAllocationInfo> allocationInfo = MemoryAllocationInfo{}) : info(allocationInfo) { this->construct(allocationInfo, createInfo); };
        BufferAllocation(const vkt::uni_ptr   <BufferAllocation>& allocation) : buffer(allocation->buffer), info(allocation->info) { this->assign(allocation); };
        BufferAllocation(const std::shared_ptr<BufferAllocation>& allocation) : buffer(allocation->buffer), info(allocation->info) { this->assign(vkt::uni_ptr<BufferAllocation>(allocation)); };
        ~BufferAllocation() {
            if (!this->isManaged()) { // Avoid VMA Memory Corruption
                if ((this->buffer || this->info.memory) && this->info.device) {
                    this->info.deviceDispatch->DeviceWaitIdle();
                };
                if (this->buffer && this->info.device) {
                    this->info.deviceDispatch->DestroyBuffer(this->buffer, nullptr), this->buffer = nullptr;
                };
                if (this->info.memory && this->info.device) {
                    this->info.deviceDispatch->FreeMemory(this->info.memory, nullptr), this->info.memory = nullptr;
                };
            };
        };

        virtual bool isManaged() const {
            return false;
        };

        virtual BufferAllocation* construct(
            vkt::uni_arg<MemoryAllocationInfo> allocationInfo,
            vkt::uni_arg<vkh::VkBufferCreateInfo> createInfo = vkh::VkBufferCreateInfo{}
        ) { // 
            this->info = allocationInfo;

            // 
            vkh::VkMemoryAllocateFlagsInfo allocFlags = {};
            vkt::unlock32(allocFlags.flags) = 0u;

            //
#ifdef VKT_CORE_USE_XVK
            if (!this->info.deviceDispatch) { this->info.deviceDispatch = vkt::vkGlobal::device; };
            if (!this->info.instanceDispatch) { this->info.instanceDispatch = vkt::vkGlobal::instance; };

            // reload device and instance
            //this->info = allocationInfo;
            if (!this->info.device) { this->info.device = this->info.deviceDispatch->handle; };
            if (!this->info.instance) { this->info.instance = this->info.instanceDispatch->handle; };
#endif

            // 
            VkExternalMemoryBufferCreateInfo extMemInfo = {};
            extMemInfo.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_BUFFER_CREATE_INFO;
            extMemInfo.pNext = nullptr;

            // 
            {
                auto* prevPtr = createInfo->pNext;
                createInfo->pNext = &extMemInfo;
                extMemInfo.pNext = prevPtr;
#ifdef VKT_WIN32_DETECTED
                extMemInfo.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
#endif
            };

            // 
            if (this->info.memUsage == VMA_MEMORY_USAGE_GPU_ONLY) {
                createInfo->usage->eShaderDeviceAddress = 1; // NEEDS SHARED BIT!
                allocFlags.flags->eAddress = 1;
            };

            //this->buffer = this->info.device.createBuffer(*createInfo);
            vkt::handleVk(this->info.deviceDispatch->CreateBuffer(*createInfo, nullptr, &this->buffer));
            this->info.range = createInfo->size;
            this->usage = createInfo->usage;

            //
            VkMemoryDedicatedRequirementsKHR dedicatedReqs = { VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS_KHR, NULL};
            VkMemoryRequirements2 memReqs2 = { VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2, &dedicatedReqs };

            // 
            const VkBufferMemoryRequirementsInfo2 bufferReqInfo = { VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2, NULL, buffer };
            (this->info.deviceDispatch->GetBufferMemoryRequirements2(&bufferReqInfo, &memReqs2));
            VkMemoryRequirements& memReqs = memReqs2.memoryRequirements;

            //
#ifdef VKT_WIN32_DETECTED
            vkh::VkExportMemoryAllocateInfo exportAllocInfo{ .handleTypes = vkh::VkExternalMemoryHandleTypeFlags{.eOpaqueWin32 = 1} };
#else
            vkh::VkExportMemoryAllocateInfo exportAllocInfo{ .handleTypes = vkh::VkExternalMemoryHandleTypeFlags{.eOpaqueFd = 1} };
#endif

            //
            vkh::VkMemoryAllocateInfo memAllocInfo = {};
            allocFlags.pNext = &exportAllocInfo;

            // prefer dedicated allocation, where and when possible
            VkMemoryDedicatedAllocateInfoKHR dedicatedInfo = { VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO_KHR, NULL, VK_NULL_HANDLE, buffer, };
            if (dedicatedReqs.prefersDedicatedAllocation || dedicatedReqs.requiresDedicatedAllocation) {
                exportAllocInfo.pNext = &dedicatedInfo;
            };

            // 
            memAllocInfo.pNext = &allocFlags;
            memAllocInfo.allocationSize = this->info.reqSize = memReqs.size;
            memAllocInfo.memoryTypeIndex = uint32_t(this->info.getMemoryType(memReqs.memoryTypeBits, { .eDeviceLocal = 1 }));

            // 
            //this->info.device.bindBufferMemory(buffer, info.memory = info.device.allocateMemory(memAllocInfo), 0);
            vkt::handleVk(this->info.deviceDispatch->AllocateMemory(memAllocInfo, nullptr, &this->info.memory));
            vkt::handleVk(this->info.deviceDispatch->BindBufferMemory(buffer, this->info.memory, 0u));

#ifdef VKT_WIN32_DETECTED
            //this->info.handle = info.device.getMemoryWin32HandleKHR({ info.memory, VkExternalMemoryHandleTypeFlagBits::eOpaqueWin32 }, this->info.dispatch);
            const auto handleInfo = VkMemoryGetWin32HandleInfoKHR{ VK_STRUCTURE_TYPE_MEMORY_GET_WIN32_HANDLE_INFO_KHR, nullptr, this->info.memory, VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT };
            this->info.deviceDispatch->GetMemoryWin32HandleKHR(&handleInfo, &this->info.handle);
#endif

            // 
            if (this->info.range == 0 || this->info.range == VK_WHOLE_SIZE) {
                this->info.range = createInfo->size;
            };

            // 
            if (createInfo->queueFamilyIndexCount) {
                this->info.queueFamilyIndices = std::vector<uint32_t>(createInfo->queueFamilyIndexCount);
                memcpy(this->info.queueFamilyIndices.data(), createInfo->pQueueFamilyIndices, sizeof(uint32_t) * createInfo->queueFamilyIndexCount);
            };

            // 
#ifdef VKT_OPENGL_INTEROP
            if (this->info.handle) {
                if (!this->info.glID) {
                    glCreateBuffers(1u, &this->info.glID);
                };
                glCreateMemoryObjectsEXT(1u, &this->info.glMemory);
                glImportMemoryWin32HandleEXT(this->info.glMemory, std::min(this->info.reqSize, this->info.range), GL_HANDLE_TYPE_OPAQUE_WIN32_EXT, this->info.handle);
                glNamedBufferStorageMemEXT(this->info.glID, std::min(this->info.reqSize, this->info.range), this->info.glMemory, 0u);
            };
#endif
            // 
            this->createInfo = createInfo;
            return this;
        };

        // Dedicated version
        virtual BufferAllocation& assign(const vkt::uni_ptr<BufferAllocation>& allocation) {
            this->buffer = allocation->buffer;
            this->info = allocation->info;
            return *this;
        };
          
        // 
        virtual BufferAllocation& operator=(const vkt::uni_ptr<BufferAllocation>& allocation) {
            return this->assign(allocation);
        };

        //
        virtual unsigned& getGLBuffer() { return this->info.glID; };
        virtual unsigned& getGLMemory() { return this->info.glMemory; };

        // 
        virtual const unsigned& getGLBuffer() const { return this->info.glID; };
        virtual const unsigned& getGLMemory() const { return this->info.glMemory; };

        // 
        virtual void* map() { return info.pMapped; };
        virtual void* mapped() { return info.pMapped; };
        virtual const void* map() const { return info.pMapped; };
        virtual const void* mapped() const { return info.pMapped; };
        virtual void  unmap() {};

        // 
        virtual operator const VkBuffer& () const { return buffer; };
        virtual operator const VkDevice& () const { return info.device; };

        //
        virtual operator VkBuffer &() { return buffer; };
        virtual operator VkDevice& () { return info.device; };

        //
        virtual VkDevice& getDevice() { return info.device; };
        virtual const VkDevice& getDevice() const { return info.device; };

        // Avoid recursion or stack overflow
        virtual VkBuffer& getBuffer() { return buffer; };
        virtual const VkBuffer& getBuffer() const { return buffer; };
         
        // 
        virtual VkDeviceSize& range() { return info.range; };
        virtual const VkDeviceSize& range() const { return info.range; };

        // 
        virtual vkf::MemoryAllocationInfo& getAllocationInfo() { return info; };
        virtual const vkf::MemoryAllocationInfo& getAllocationInfo() const { return info; };

        // 
        virtual vkh::VkBufferCreateInfo& getCreateInfo() { return createInfo; };
        virtual const vkh::VkBufferCreateInfo& getCreateInfo() const { return createInfo; };

        // 
        virtual vkf::MemoryAllocationInfo& getInfo() { return info; };
        virtual const vkf::MemoryAllocationInfo& getInfo() const { return info; };

        // 
        virtual BufferAllocation* address() { return this; };
        virtual const BufferAllocation* address() const { return this; };

        // Queue Family Indices
        virtual std::vector<uint32_t>& getQueueFamilyIndices() { return this->info.queueFamilyIndices; };
        virtual const std::vector<uint32_t>& getQueueFamilyIndices() const { return this->info.queueFamilyIndices; };

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
            return vkh::VkDeviceOrHostAddressConstKHR{ .deviceAddress = this->usage.eShaderDeviceAddress ? const_cast<MemoryAllocationInfo&>(this->info).deviceDispatch->GetBufferDeviceAddress(vkh::VkBufferDeviceAddressInfo{.buffer = this->buffer}) : 0ull };
        };
            
        // getter by operator (for direct pass)
        virtual operator vkh::VkDeviceOrHostAddressKHR() { return this->deviceAddress(); };
        virtual operator vkh::VkDeviceOrHostAddressConstKHR() const { return this->deviceAddress(); };

    public: // in-variant 
        VkBuffer buffer = VK_NULL_HANDLE; vkh::VkBufferUsageFlags usage = {};
        VkDeviceAddress cached = VK_NULL_HANDLE;
        vkf::MemoryAllocationInfo info = {};
        vkh::VkBufferCreateInfo createInfo = {};

    protected: friend BufferAllocation;
    };

};
