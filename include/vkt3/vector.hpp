#pragma once // #

#ifdef VKT_FORCE_VMA_IMPLEMENTATION
#ifndef VMA_IMPLEMENTATION
#define VMA_IMPLEMENTATION
#endif
#endif

#include "utils.hpp"
#include "structs.hpp"
#include "core.hpp"
#include <memory>

namespace vkt {

#ifdef ENABLE_OPENGL_INTEROP
    using namespace gl;
#endif

    // 
    class VmaBufferAllocation;
    class BufferAllocation : public std::enable_shared_from_this<BufferAllocation> { public:
        BufferAllocation() {};
        BufferAllocation(vkt::uni_arg<MemoryAllocationInfo> allocationInfo, vkt::uni_arg<vkh::VkBufferCreateInfo> createInfo = vkh::VkBufferCreateInfo{}) : info( allocationInfo) { this->construct( allocationInfo,  createInfo); };
        BufferAllocation(const vkt::uni_ptr   <BufferAllocation>& allocation) : buffer(allocation->buffer), info(allocation->info) { this->assign(allocation); };
        BufferAllocation(const std::shared_ptr<BufferAllocation>& allocation) : buffer(allocation->buffer), info(allocation->info) { this->assign(vkt::uni_ptr<BufferAllocation>(allocation)); };
        ~BufferAllocation() { // Broken Support, but supports Win32 memory export
            if (this->buffer && this->info.device && this->info.memory) {
                this->info.deviceDispatch->DeviceWaitIdle();
            }
        };

        virtual BufferAllocation* construct(
            vkt::uni_arg<MemoryAllocationInfo> allocationInfo,
            vkt::uni_arg<vkh::VkBufferCreateInfo> createInfo = vkh::VkBufferCreateInfo{}
        ) { // 
            vkh::VkMemoryAllocateFlagsInfo allocFlags = {};
            //allocFlags.flags.eMask = 1u;

            //
            if (!this->info.deviceDispatch) { this->info.deviceDispatch = vkGlobal::device; };
            if (!this->info.instanceDispatch) { this->info.instanceDispatch = vkGlobal::instance; };

            // reload device and instance
            //this->info = allocationInfo;
            if (!this->info.device) { this->info.device = this->info.deviceDispatch->handle; };
            if (!this->info.instance) { this->info.instance = this->info.instanceDispatch->handle; };

            // 
            if (allocationInfo->memUsage == VMA_MEMORY_USAGE_GPU_ONLY) {
                createInfo->usage.eSharedDeviceAddress = 1; // NEEDS SHARED BIT!
                allocFlags.flags.eAddress = 1;
            };

            //this->buffer = this->info.device.createBuffer(*createInfo);
            vkh::handleVk(this->info.deviceDispatch->CreateBuffer(*createInfo, nullptr, &this->buffer));
            this->usage = createInfo->usage;

            // 
            VkMemoryRequirements memReqs = {};
            (this->info.deviceDispatch->GetBufferMemoryRequirements(buffer, &memReqs));
            //allocationInfo->device.getBufferMemoryRequirements(buffer);
            vkh::VkExportMemoryAllocateInfo exportAllocInfo {
                .handleTypes = { .eOpaqueWin32 = 1 }
            };

            // 
            vkh::VkMemoryAllocateInfo memAllocInfo = {};
            exportAllocInfo.pNext = &allocFlags;
            memAllocInfo.pNext = &exportAllocInfo;
            memAllocInfo.allocationSize = memReqs.size;
            memAllocInfo.memoryTypeIndex = uint32_t(allocationInfo->getMemoryType(memReqs.memoryTypeBits, { .eDeviceLocal = 1 }));


            // 
            //this->info.device.bindBufferMemory(buffer, info.memory = info.device.allocateMemory(memAllocInfo), 0);
            VkDeviceMemory memory = {};
            vkh::handleVk(this->info.deviceDispatch->AllocateMemory(memAllocInfo, nullptr, &memory));
            vkh::handleVk(this->info.deviceDispatch->BindBufferMemory(buffer, memory, 0u));

            // TODO: FIX BROKEN!
#if defined(ENABLE_OPENGL_INTEROP) && defined(VK_USE_PLATFORM_WIN32_KHR)
            //this->info.handle = info.device.getMemoryWin32HandleKHR({ info.memory, VkExternalMemoryHandleTypeFlagBits::eOpaqueWin32 }, this->info.dispatch);
            const auto handleInfo = VkMemoryGetWin32HandleInfoKHR{ VK_STRUCTURE_TYPE_MEMORY_GET_WIN32_HANDLE_INFO_KHR, nullptr, info.memory, VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT };
            this->info.deviceDispatch->GetMemoryWin32HandleKHR(&handleInfo, &this->info.handle);
#endif

            // 
            this->info.reqSize = memReqs.size;
            if (this->info.range == 0 || this->info.range == VK_WHOLE_SIZE) {
                this->info.range = createInfo->size;
            };

            // 
            if (createInfo->queueFamilyIndexCount) {
                this->info.queueFamilyIndices = std::vector<uint32_t>(createInfo->queueFamilyIndexCount);
                memcpy(this->info.queueFamilyIndices.data(), createInfo->pQueueFamilyIndices, sizeof(uint32_t) * createInfo->queueFamilyIndexCount);
            };

            // 
#ifdef ENABLE_OPENGL_INTEROP
            glCreateBuffers(1u, &this->info.glID);
            glCreateMemoryObjectsEXT(1u, &this->info.glMemory);
            glImportMemoryWin32HandleEXT(this->info.glMemory, this->info.reqSize, GL_HANDLE_TYPE_OPAQUE_WIN32_EXT, this->info.handle);
            glNamedBufferStorageMemEXT(this->info.glID, this->info.range, this->info.glMemory, 0u);
#endif

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
        virtual MemoryAllocationInfo& getAllocationInfo() { return info; };
        virtual const MemoryAllocationInfo& getAllocationInfo() const { return info; };

        // 
        virtual BufferAllocation* address() { return this; };
        virtual const BufferAllocation* address() const { return this; };

        // Queue Family Indices
        virtual std::vector<uint32_t>& getQueueFamilyIndices() { return this->info.queueFamilyIndices; };
        virtual const std::vector<uint32_t>& getQueueFamilyIndices() const { return this->info.queueFamilyIndices; };

        // 
        virtual vkh::VkDeviceOrHostAddressKHR deviceAddress() {
            if (!this->usage.eSharedDeviceAddress) {
                std::cerr << "Bad Device Address" << std::endl;
                assert(true);
            };
            return vkh::VkDeviceOrHostAddressKHR{ .deviceAddress = this->usage.eSharedDeviceAddress ? this->info.deviceDispatch->GetBufferDeviceAddress(vkh::VkBufferDeviceAddressInfo{.buffer = this->buffer}) : 0ull };
        };

        // 
        virtual vkh::VkDeviceOrHostAddressConstKHR deviceAddress() const {
            if (!this->usage.eSharedDeviceAddress) {
                std::cerr << "Bad Device Address" << std::endl;
                assert(true);
            };
            return vkh::VkDeviceOrHostAddressConstKHR{ .deviceAddress = this->usage.eSharedDeviceAddress ? const_cast<MemoryAllocationInfo&>(this->info).deviceDispatch->GetBufferDeviceAddress(vkh::VkBufferDeviceAddressInfo{.buffer = this->buffer}) : 0ull };
        };
            
        // getter by operator (for direct pass)
        virtual operator vkh::VkDeviceOrHostAddressKHR() { return this->deviceAddress(); };
        virtual operator vkh::VkDeviceOrHostAddressConstKHR() const { return this->deviceAddress(); };

    public: // in-variant 
        VkBuffer buffer = {}; vkh::VkBufferUsageFlags usage = {};
        VkDeviceAddress cached = {};
        MemoryAllocationInfo info = {};

    protected: friend BufferAllocation; friend VmaBufferAllocation;
    };

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
            this->info.deviceDispatch->DeviceWaitIdle();
            vmaDestroyBuffer(allocator, buffer, allocation);
        };

        //
        virtual VmaBufferAllocation* construct(
            vkt::uni_arg<VmaAllocator> allocator,
            vkt::uni_arg<vkh::VkBufferCreateInfo> createInfo = vkh::VkBufferCreateInfo{},
            vkt::uni_arg<VmaMemoryInfo> memInfo = VmaMemoryInfo{ .memUsage = VMA_MEMORY_USAGE_GPU_ONLY }
        ) {
            VmaAllocationCreateInfo vmaInfo = {}; vmaInfo.usage = memInfo->memUsage;
            if (memInfo->memUsage == VMA_MEMORY_USAGE_CPU_TO_GPU || memInfo->memUsage == VMA_MEMORY_USAGE_GPU_TO_CPU) { vmaInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; };
            if (memInfo->memUsage == VMA_MEMORY_USAGE_GPU_ONLY) { createInfo->usage.eSharedDeviceAddress = 1; }; // NEEDS SHARED BIT!

            // 
            vkh::handleVk(vmaCreateBuffer(this->allocator = allocator.ref(), *createInfo, &vmaInfo, &reinterpret_cast<VkBuffer&>(this->buffer), &this->allocation, &this->allocationInfo));
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
            this->info.deviceDispatch = memInfo->deviceDispatch;
            this->info.instanceDispatch = memInfo->instanceDispatch;

            //
            if (!this->info.deviceDispatch) { this->info.deviceDispatch = vkGlobal::device; };
            if (!this->info.instanceDispatch) { this->info.instanceDispatch = vkGlobal::instance; };

            // when loader initialized
            if (vkt::vkGlobal::initialized) {
                if (!this->info.deviceDispatch) this->info.deviceDispatch = std::make_shared<xvk::Device>(this->info.instanceDispatch, info.device);
                if (!this->info.instanceDispatch) this->info.instanceDispatch = std::make_shared<xvk::Instance>(vkt::vkGlobal::loader, info.instance);
            };

            // reload device and instance
            if (!this->info.device) { this->info.device = info.device; };
            if (!this->info.instance) { this->info.instance = info.instance; };

            // 
            if (createInfo->queueFamilyIndexCount) {
                this->info.queueFamilyIndices = std::vector<uint32_t>(createInfo->queueFamilyIndexCount);
                memcpy(this->info.queueFamilyIndices.data(), createInfo->pQueueFamilyIndices, sizeof(uint32_t) * createInfo->queueFamilyIndexCount);
            };

            // 
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
         
        // Get mapped memory
        virtual void* map() { void* ptr = nullptr; vkh::handleVk(vmaMapMemory(allocator, allocation, &ptr)); return ptr; };
        virtual void* mapped() { if (!allocationInfo.pMappedData) { vkh::handleVk(vmaMapMemory(allocator, allocation, &allocationInfo.pMappedData)); }; return allocationInfo.pMappedData; };
        virtual void  unmap() { vmaUnmapMemory(allocator, allocation); allocationInfo.pMappedData = nullptr; };
        
        // Allocation
        virtual operator const VmaAllocation& () const { return allocation; };
        virtual operator const VmaAllocationInfo& () const { return allocationInfo; };

        // 
        virtual VmaBufferAllocation* address() { return this; };
        virtual const VmaBufferAllocation* address() const { return this; };
        
        //
        virtual vkh::VkDeviceOrHostAddressKHR deviceAddress() {
            if (!this->usage.eSharedDeviceAddress) {
                std::cerr << "Bad Device Address" << std::endl;
                assert(true);
            };
            return vkh::VkDeviceOrHostAddressKHR{ .deviceAddress = this->usage.eSharedDeviceAddress ? this->info.deviceDispatch->GetBufferDeviceAddress(vkh::VkBufferDeviceAddressInfo{.buffer = this->buffer}) : 0ull };
        };

        // 
        virtual vkh::VkDeviceOrHostAddressConstKHR deviceAddress() const {
            if (!this->usage.eSharedDeviceAddress) {
                std::cerr << "Bad Device Address" << std::endl;
                assert(true);
            };
            return vkh::VkDeviceOrHostAddressConstKHR{ .deviceAddress = this->usage.eSharedDeviceAddress ? const_cast<MemoryAllocationInfo&>(this->info).deviceDispatch->GetBufferDeviceAddress(vkh::VkBufferDeviceAddressInfo{.buffer = this->buffer}) : 0ull};
        };

    // 
    protected: friend VmaBufferAllocation; friend BufferAllocation; // 
        VmaAllocationInfo allocationInfo = {};
        VmaAllocation allocation = {};
        VmaAllocator allocator = {};
    };

    // Wrapper Class
    template<class T = uint8_t>
    class Vector : public std::enable_shared_from_this<Vector<T>> { public: //
        Vector() {};

        // 
        Vector(const vkt::uni_ptr<BufferAllocation>& allocation, vkt::uni_arg<VkDeviceSize> offset = 0ull, vkt::uni_arg<VkDeviceSize> size = VK_WHOLE_SIZE, vkt::uni_arg<VkDeviceSize> stride = sizeof(T)) : allocation(allocation), bufInfo({ allocation->buffer, offset, size }) { this->construct(allocation, offset, size, stride); };
        Vector(const vkt::uni_ptr<VmaBufferAllocation>& allocation, vkt::uni_arg<VkDeviceSize> offset = 0ull, vkt::uni_arg<VkDeviceSize> size = VK_WHOLE_SIZE, vkt::uni_arg<VkDeviceSize> stride = sizeof(T)) : allocation(allocation.dyn_cast<BufferAllocation>()), bufInfo({ allocation->buffer, offset, size }) { this->construct(allocation.dyn_cast<BufferAllocation>(), offset, size, stride); };

        // 
        Vector(const std::shared_ptr<BufferAllocation>& allocation, vkt::uni_arg<VkDeviceSize> offset = 0ull, vkt::uni_arg<VkDeviceSize> size = VK_WHOLE_SIZE, vkt::uni_arg<VkDeviceSize> stride = sizeof(T)) : allocation(allocation), bufInfo({ allocation->buffer, offset, size }) { this->construct(allocation, offset, size, stride); };
        Vector(const std::shared_ptr<VmaBufferAllocation>& allocation, vkt::uni_arg<VkDeviceSize> offset = 0ull, vkt::uni_arg<VkDeviceSize> size = VK_WHOLE_SIZE, vkt::uni_arg<VkDeviceSize> stride = sizeof(T)) : allocation(std::dynamic_pointer_cast<BufferAllocation>(allocation)), bufInfo({ allocation->buffer, offset, size }) { this->construct(std::dynamic_pointer_cast<BufferAllocation>(allocation), offset, size, stride); };

        // 
        ~Vector() {
            if (this->view) {
                this->allocation->info.deviceDispatch->DestroyBufferView(this->view, nullptr);
                this->view = VkBufferView{};
            };
        };

        // 
        template<class Tm = T> Vector(const Vector<Tm>& V) : allocation(V), bufInfo({ V.buffer(), V.offset(), V.range() }) { *this = V; };
        template<class Tm = T> inline Vector<T>& operator=(const Vector<Tm>& V) { 
            this->allocation = V.uniPtr();
            this->bufInfo = vkh::VkDescriptorBufferInfo{ static_cast<VkBuffer>(V.buffer()), V.offset(), V.range() };
            this->bufRegion = vkh::VkStridedBufferRegionKHR{ static_cast<VkBuffer>(V.buffer()), V.offset(), sizeof(T), V.ranged() / sizeof(T) };
            return *this;
        };
         
        // 
        //template<class Tm = T> Vector<Tm> cast() { return *this; };
        //template<class Tm = T> const vkt::uni_arg<Vector<Tm>>& cast() const { return Vector<Tm>(reinterpret_cast<Vector<T>&>(*this)); };
         
        //
        virtual Vector<T>* construct(vkt::uni_ptr<BufferAllocation> allocation, vkt::uni_arg<VkDeviceSize> offset = 0ull, vkt::uni_arg<VkDeviceSize> size = VK_WHOLE_SIZE, vkt::uni_arg<VkDeviceSize> stride = sizeof(T)) {
            this->allocation = allocation;
            this->bufInfo = vkh::VkDescriptorBufferInfo{ static_cast<VkBuffer>(allocation->buffer),offset,size };
            this->bufRegion = vkh::VkStridedBufferRegionKHR{ static_cast<VkBuffer>(allocation->buffer),offset,stride,this->ranged()/stride };
            return this;
        };

        // 
        virtual VkBufferView& createBufferView(const VkFormat& format = VkFormat::eUndefined) {
            vkh::handleVk(this->allocation->info.deviceDispatch->CreateBufferView(vkh::VkBufferViewCreateInfo{
                .buffer = static_cast<VkBuffer>(this->bufRegion.buffer),
                .format = static_cast<VkFormat>(format),
                .offset = this->bufRegion.offset,
                .range = this->bufInfo.range
            }, nullptr, &view));
            return view;
        };

        // alias Of getAllocation
        virtual vkt::uni_ptr<BufferAllocation>& uniPtr() { return allocation; };
        virtual vkt::uni_ptr<BufferAllocation> uniPtr() const { return allocation; };
         
        // 
        virtual operator    ::VkDescriptorBufferInfo& () { this->bufInfo.buffer = this->bufRegion.buffer = allocation->buffer; return bufInfo; };
        virtual operator vkh::VkDescriptorBufferInfo& () { this->bufInfo.buffer = this->bufRegion.buffer = allocation->buffer; return bufInfo; };
        virtual operator vkt::uni_ptr<BufferAllocation>& () { return allocation; };
        virtual operator std::shared_ptr<BufferAllocation>& () { return allocation; };
              
        //
        virtual operator const ::VkDescriptorBufferInfo &() const { return bufInfo; };
        virtual operator const vkh::VkDescriptorBufferInfo&() const { return bufInfo; };
        virtual operator const vkt::uni_ptr<BufferAllocation>&() const { return allocation; };
        virtual operator const std::shared_ptr<BufferAllocation>&() const { return allocation; };
        
        // 
        virtual operator BufferAllocation*() { return allocation; };
        virtual operator const BufferAllocation*() const { return allocation; };
           
        // 
        virtual operator VkBufferView&() { return view; };
        virtual operator VkDevice& () { return *allocation; };
        virtual operator VkBuffer&() { return *allocation; };
           
        // 
        virtual operator const VkBuffer& () const { return *allocation; };
        virtual operator const VkDevice& () const { return *allocation; };
        virtual operator const VkBufferView& () const { return view; };
         
         
        // 
        virtual VkDeviceSize& offset() { return this->bufRegion.offset; };
        virtual const VkDeviceSize& offset() const { return this->bufRegion.offset; };

        // LEGACY, used for constructor only
        virtual VkDeviceSize ranged() const { return (this->bufInfo.range != VK_WHOLE_SIZE ? std::min(VkDeviceSize(this->bufInfo.range), VkDeviceSize(this->allocation->range() - this->offset())) : VkDeviceSize(this->allocation->range() - this->offset())); };

        // Get static and cached value
        virtual VkDeviceSize& range() { return (this->bufInfo.range = (this->bufRegion.size * this->bufRegion.stride - 0u)); };
        virtual VkDeviceSize  range() const { return (this->bufRegion.size * this->bufRegion.stride - 0u); };

        //virtual VkDeviceSize size() const { return this->range() / this->stride; };
        virtual const VkDeviceSize& size() const { return this->bufRegion.size; };
        virtual VkDeviceSize& size() { return this->bufRegion.size; };

        // 
        virtual const VkDeviceSize& stride() const { return this->bufRegion.stride; };
        virtual VkDeviceSize& stride() { return this->bufRegion.stride; };

        // ALIAS
        virtual const VkBuffer& buffer() const { return this->getBuffer(); };
        virtual VkBuffer& buffer() { return this->getBuffer(); };

        // 
        virtual VkDevice& getDevice() { return reinterpret_cast<VkDevice&>(this->allocation->getDevice()); };
        virtual VkBuffer& getBuffer() { return reinterpret_cast<VkBuffer&>(this->allocation->getBuffer()); };

        // 
        virtual const VkDevice& getDevice() const { return reinterpret_cast<const VkDevice&>(this->allocation->getDevice()); };
        virtual const VkBuffer& getBuffer() const { return reinterpret_cast<const VkBuffer&>(this->allocation->getBuffer()); };

        // For JavaCPP and LWJGL-3
        virtual VkDevice& handleDevice() { return reinterpret_cast<VkDevice&>(this->getDevice()); };
        virtual VkBuffer& handleBuffer() { return reinterpret_cast<VkBuffer&>(this->getBuffer()); };

        // For JavaCPP and LWJGL-3
        virtual const VkDevice& handleDevice() const { return reinterpret_cast<const VkDevice&>(this->getDevice()); };
        virtual const VkBuffer& handleBuffer() const { return reinterpret_cast<const VkBuffer&>(this->getBuffer()); };

        // typed casting 
        template<class Tm = T> inline Vector<Tm>& cast() { return reinterpret_cast<Vector<Tm>&>(*this); };
        template<class Tm = T> inline const Vector<Tm>& cast() const { return reinterpret_cast<const Vector<Tm>&>(*this); };

        virtual unsigned& getGL() { return this->allocation->info.glID; };
        virtual const unsigned& getGL() const { return this->allocation->info.glID; };

        // 
        virtual bool has() const { return allocation ? true : false; };
        virtual bool has_value() const { return this->has(); };

        // 
        virtual BufferAllocation* operator->() { return allocation.ptr(); };
        virtual BufferAllocation& operator*() { return allocation.ref(); };
        virtual const BufferAllocation* operator->() const { return allocation.ptr(); };
        virtual const BufferAllocation& operator*() const { return allocation.ref(); };

        // 
        virtual VkDeviceSize& rangeInfo() { return bufInfo.range; };
        virtual const VkDeviceSize& rangeInfo() const { return bufInfo.range; };

        // 
        virtual Vector<T>* address() { return this; };
        virtual const Vector<T>* address() const { return this; };

        // 
        virtual vkh::VkDescriptorBufferInfo& getDescriptor() { return bufInfo; };
        virtual const vkh::VkDescriptorBufferInfo& getDescriptor() const { return bufInfo; };

        // alias of uniPtr
        virtual vkt::uni_ptr<BufferAllocation>& getAllocation() { return allocation; };
        virtual const vkt::uni_ptr<BufferAllocation>& getAllocation() const { return allocation; };

        // for JavaCPP
        virtual BufferAllocation* getAllocationPtr() { return allocation.ptr(); };
        virtual const BufferAllocation* getAllocationPtr() const { return allocation.ptr(); };
        
        // get deviceAddress with offset (currently, prefer unshifted)
        virtual vkh::VkDeviceOrHostAddressKHR deviceAddress() {
            return this->allocation->deviceAddress();
        };

        // get deviceAddress with offset (currently, prefer unshifted)
        virtual vkh::VkDeviceOrHostAddressConstKHR deviceAddress() const {
            return this->allocation->deviceAddress();
        };

        // 
        virtual std::vector<uint32_t>& getQueueFamilyIndices() { return this->allocation->getQueueFamilyIndices(); };
        virtual const std::vector<uint32_t>& getQueueFamilyIndices() const { return this->allocation->getQueueFamilyIndices(); };

        // 
        virtual vkh::VkStridedBufferRegionKHR& getRegion() { return bufRegion; };
        virtual const vkh::VkStridedBufferRegionKHR& getRegion() const { return bufRegion; };

        // getter by operator (for direct pass)
        virtual operator vkh::VkDeviceOrHostAddressKHR() { return this->deviceAddress(); };
        virtual operator vkh::VkDeviceOrHostAddressConstKHR() const { return this->deviceAddress(); };

        // 
        virtual void unmap() { allocation->unmap(); };
        virtual const T* map(const uintptr_t& i = 0u) const { auto mapc = reinterpret_cast<const uint8_t*>(allocation->map()) + offset(); return &(reinterpret_cast<const T*>(mapc))[i]; };
        virtual T* const map(const uintptr_t& i = 0u) { auto mapc = reinterpret_cast<uint8_t*>(allocation->map()) + offset(); return &(reinterpret_cast<T*>(mapc))[i]; };

        // 
        virtual const T* mapped(const uintptr_t& i = 0u) const { auto mapc = reinterpret_cast<const uint8_t*>(allocation->mapped()) + offset(); return &(reinterpret_cast<const T*>(mapc))[i]; };
        virtual T* const mapped(const uintptr_t& i = 0u) { auto mapc = reinterpret_cast<uint8_t*>(allocation->mapped()) + offset(); return &(reinterpret_cast<T*>(mapc))[i]; };

        // 
        virtual T* const data() { return mapped(); };
        virtual const T* data() const { return mapped(); };

        // at function 
        virtual const T& at(const uintptr_t& i = 0u) const { return *mapped(i); };
        virtual T& at(const uintptr_t& i = 0u) { return *mapped(i); };

        // array operator 
        virtual const T& operator [] (const uintptr_t& i) const { return at(i); };
        virtual T& operator [] (const uintptr_t& i) { return at(i); };

        // begin ptr
        virtual const T* begin() const { return data(); };
        virtual T* const begin() { return data(); };

        // end ptr
        virtual const T* end() const { return &at(size() - 1ul); };
        virtual T* const end() { return &at(size() - 1ul); };

        // 
        virtual unsigned& getGLBuffer() { return this->allocation->getGLBuffer(); };
        virtual unsigned& getGLMemory() { return this->allocation->getGLMemory(); };

        // 
        virtual const unsigned& getGLBuffer() const { return this->allocation->getGLBuffer(); };
        virtual const unsigned& getGLMemory() const { return this->allocation->getGLMemory(); };

        // 
        protected: friend Vector<T>; // 
        protected: vkh::VkDescriptorBufferInfo bufInfo = { {}, 0u, VK_WHOLE_SIZE }; // Cached Feature
        protected: vkh::VkStridedBufferRegionKHR bufRegion = { {}, 0u, sizeof(T), VK_WHOLE_SIZE };
        protected: VkBufferView view = {};
        protected: vkt::uni_ptr<BufferAllocation> allocation = {};
    };

    template<class T = uint8_t>
    Vector<T>* MakeVmaVector(vkt::uni_ptr<VmaBufferAllocation> allocation, vkt::uni_arg<VkDeviceSize> offset = 0ull, vkt::uni_arg<VkDeviceSize> size = VK_WHOLE_SIZE, vkt::uni_arg<VkDeviceSize> stride = sizeof(T)) { return new Vector<T>(allocation, offset, size, stride); };

};
