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


    // 
    class VmaBufferAllocation;
    class BufferAllocation : public std::enable_shared_from_this<BufferAllocation> { public:
        BufferAllocation() {};
        BufferAllocation(vkt::uni_arg<MemoryAllocationInfo> allocationInfo, vkt::uni_arg<vkh::VkBufferCreateInfo> createInfo = vkh::VkBufferCreateInfo{}) : info( allocationInfo) { this->construct( allocationInfo,  createInfo); };
        BufferAllocation(vkt::uni_ptr    <BufferAllocation> allocation) : buffer(allocation->buffer), info(allocation->info) { *this = allocation; };

        virtual BufferAllocation* construct(
            vkt::uni_arg<MemoryAllocationInfo> allocationInfo,
            vkt::uni_arg<vkh::VkBufferCreateInfo> createInfo = vkh::VkBufferCreateInfo{}
        ) {
            this->buffer = this->info.device.createBuffer(*createInfo);
            this->usage = createInfo->usage;

            vk::MemoryAllocateFlagsInfo allocFlags = {};
            allocFlags.flags = vk::MemoryAllocateFlagBits::eDeviceAddress;

            // 
            vk::MemoryRequirements memReqs = allocationInfo->device.getBufferMemoryRequirements(buffer);
            vk::ExportMemoryAllocateInfo exportAllocInfo{ vk::ExternalMemoryHandleTypeFlagBits::eOpaqueWin32 };

            // 
            vk::MemoryAllocateInfo memAllocInfo = {};
            memAllocInfo.pNext = &exportAllocInfo.setPNext(&allocFlags);
            memAllocInfo.allocationSize = memReqs.size;
            memAllocInfo.memoryTypeIndex = uint32_t(allocationInfo->getMemoryType(memReqs.memoryTypeBits, { .eDeviceLocal = 1 }));

            // 
            this->info.device.bindBufferMemory(buffer, info.memory = info.device.allocateMemory(memAllocInfo), 0);

#if defined(ENABLE_OPENGL_INTEROP) && defined(VK_USE_PLATFORM_WIN32_KHR)
            this->info.handle = info.device.getMemoryWin32HandleKHR({ info.memory, vk::ExternalMemoryHandleTypeFlagBits::eOpaqueWin32 }, this->info.dispatch);
#endif

            // 
            this->info.reqSize = memReqs.size;
            if (this->info.range == 0 || this->info.range == VK_WHOLE_SIZE) {
                this->info.range = createInfo->size;
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

        // 
        virtual BufferAllocation& operator=(vkt::uni_ptr<BufferAllocation> allocation) {
            this->buffer = allocation->buffer;
            this->info = allocation->info;
            return *this;
        };

        // 
        virtual void* map() { return info.pMapped; };
        virtual void* mapped() { return info.pMapped; };
        virtual const void* map() const { return info.pMapped; };
        virtual const void* mapped() const { return info.pMapped; };
        virtual void  unmap() {};

        // 
        virtual const vk::Buffer& getImage() const { return this->buffer; };
        virtual vk::Buffer& getImage() { return this->buffer; };

        // 
        virtual operator vk::Buffer& () { return buffer; };
        virtual operator VkBuffer& () { return reinterpret_cast<VkBuffer&>(buffer); };

        // 
        virtual operator const vk::Buffer& () const { return buffer; };
        virtual operator const VkBuffer& () const { return reinterpret_cast<const VkBuffer&>(buffer); };

        // VMA HACK FOR EXTRACT DEVICE
        virtual operator const vk::Device& () const { return info.device; };
        virtual operator const VkDevice& () const { return reinterpret_cast<const VkDevice&>(info.device); };

        // 
        virtual operator vk::Device& () { return info.device; };
        virtual operator VkDevice& () { return reinterpret_cast<VkDevice&>(info.device); };

        //
        virtual vk::Device& getDevice() { return info.device; };
        virtual const vk::Device& getDevice() const { return info.device; };

        // 
        virtual vk::DeviceSize& range() { return info.range; };
        virtual const vk::DeviceSize& range() const { return info.range; };

        // 
        virtual MemoryAllocationInfo& getAllocationInfo() { return info; };
        virtual const MemoryAllocationInfo& getAllocationInfo() const { return info; };

        // 
        virtual BufferAllocation* address() { return this; };
        virtual const BufferAllocation* address() const { return this; };

        // 
        virtual vkh::VkDeviceOrHostAddressKHR deviceAddress() {
            if (!this->usage.eSharedDeviceAddress) { std::cerr << "Bad Device Address" << std::endl; assert(true); };
            return vkh::VkDeviceOrHostAddressKHR{ .deviceAddress = this->usage.eSharedDeviceAddress ? getDevice().getBufferAddress(vkh::VkBufferDeviceAddressInfo{.buffer = this->buffer }.hpp(), this->info.dispatch) : 0ull };
        };

        // 
        virtual vkh::VkDeviceOrHostAddressConstKHR deviceAddress() const {
            if (!this->usage.eSharedDeviceAddress) { std::cerr << "Bad Device Address" << std::endl; assert(true); };
            return vkh::VkDeviceOrHostAddressConstKHR{ .deviceAddress = this->usage.eSharedDeviceAddress ? getDevice().getBufferAddress(vkh::VkBufferDeviceAddressInfo{.buffer = this->buffer }.hpp(), this->info.dispatch) : 0ull };
        };

        // getter by operator (for direct pass)
        virtual operator vkh::VkDeviceOrHostAddressKHR() { return this->deviceAddress(); };
        virtual operator vkh::VkDeviceOrHostAddressConstKHR() const { return this->deviceAddress(); };

    public: // in-variant 
        vk::Buffer buffer = {};
        vkh::VkBufferUsageFlags usage = {};
        vk::DeviceAddress cached = {};
        MemoryAllocationInfo info = {};

    protected: friend BufferAllocation; friend VmaBufferAllocation;
    };

    // 
    class VmaBufferAllocation : public BufferAllocation { public: 
        VmaBufferAllocation() {};
        VmaBufferAllocation(vkt::uni_arg<VmaAllocator> allocator, const vkt::uni_arg<vkh::VkBufferCreateInfo>& createInfo = vkh::VkBufferCreateInfo{}, VmaMemoryUsage vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY) { this->construct(*allocator,  createInfo, vmaUsage); };
        VmaBufferAllocation(vkt::uni_ptr<VmaBufferAllocation> allocation) : allocation(allocation->allocation), allocationInfo(allocation->allocationInfo), allocator(allocation->allocator) { *this = allocation; };
        VmaBufferAllocation(vkt::uni_ptr<BufferAllocation> allocation) { *this = dynamic_cast<const VmaBufferAllocation&>(*allocation); };
        //VmaBufferAllocation(std::shared_ptr<VmaBufferAllocation> allocation) : allocation(allocation->allocation), allocationInfo(allocation->allocationInfo), allocator(allocation->allocator) { *this = allocation; };
        //VmaBufferAllocation(std::shared_ptr<BufferAllocation> allocation) { *this = std::dynamic_pointer_cast<VmaBufferAllocation>(allocation); };

        //
        virtual VmaBufferAllocation* construct(
            vkt::uni_arg<VmaAllocator> allocator,
            vkt::uni_arg<vkh::VkBufferCreateInfo> createInfo = vkh::VkBufferCreateInfo{},
            vkt::uni_arg<VmaMemoryUsage> vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY
        ) {
            VmaAllocationCreateInfo vmaInfo = {}; vmaInfo.usage = vmaUsage;
            if (vmaUsage == VMA_MEMORY_USAGE_CPU_TO_GPU || vmaUsage == VMA_MEMORY_USAGE_GPU_TO_CPU) { vmaInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; };

            // 
            auto result = vmaCreateBuffer(this->allocator = allocator.ref(), *createInfo, &vmaInfo, &reinterpret_cast<VkBuffer&>(this->buffer), &this->allocation, &this->allocationInfo); //assert(result == VK_SUCCESS);
            this->info.range = createInfo->size;
            this->info.memUsage = vmaUsage;
            this->info.memory = allocationInfo.deviceMemory;
            this->info.offset = allocationInfo.offset;
            this->usage = createInfo->usage;

            // 
            //std::cout << "Allocation Code: " << result << std::endl;
            //std::cout << "Vma Allocation PTR:  " << this->allocation << std::endl;

            // Get Dispatch Loader From VMA Allocator Itself!
            VmaAllocatorInfo info = {};
            vmaGetAllocatorInfo(this->allocator = allocator.ref(), &info);
            this->info.dispatch = vk::DispatchLoaderDynamic(info.instance, vkGetInstanceProcAddr, this->info.device = info.device, vkGetDeviceProcAddr); // 

            // 
            return this;
        };

        // 
        virtual VmaBufferAllocation& operator=(vkt::uni_ptr<VmaBufferAllocation> allocation) {
            if (allocation->allocation) {
                vmaDestroyBuffer(allocator, *this, *allocation); // don't assign into already allocated
            };
            this->info = allocation->info;
            this->usage = allocation->usage;
            this->buffer = allocation->buffer;
            this->allocation = allocation->allocation;
            this->allocationInfo = allocation->allocationInfo;
            this->allocator = allocation->allocator;
            return *this;
        };

        // Get mapped memory
        virtual void* map() { void* ptr = nullptr; vmaMapMemory(allocator, allocation, &ptr); return ptr; };
        virtual void* mapped() { if (!allocationInfo.pMappedData) { vmaMapMemory(allocator, allocation, &allocationInfo.pMappedData); }; return allocationInfo.pMappedData; };
        virtual void  unmap() { vmaUnmapMemory(allocator, allocation); allocationInfo.pMappedData = nullptr; };

        // 
        /*virtual const vk::Device& _getDevice() const {
            VmaAllocatorInfo info = {};
            vmaGetAllocatorInfo(this->allocator, &info);
            return info.device;
        };// const override { return device; };
        */

        // Allocation
        virtual operator const VmaAllocation& () const { return allocation; };
        virtual operator const VmaAllocationInfo& () const { return allocationInfo; };

        // 
        virtual VmaBufferAllocation* address() { return this; };
        virtual const VmaBufferAllocation* address() const { return this; };

        // VMA-Based Version
        virtual vkh::VkDeviceOrHostAddressKHR deviceAddress() override {
            if (!this->usage.eSharedDeviceAddress) { std::cerr << "Bad Device Address" << std::endl; assert(true); };
            return vkh::VkDeviceOrHostAddressKHR{ .deviceAddress = this->usage.eSharedDeviceAddress ? getDevice().getBufferAddress(vkh::VkBufferDeviceAddressInfo{.buffer = this->buffer }.hpp(), this->info.dispatch) : 0ull };
        };

        // VMA-Based Version
        virtual vkh::VkDeviceOrHostAddressConstKHR deviceAddress() const override {
            if (!this->usage.eSharedDeviceAddress) { std::cerr << "Bad Device Address" << std::endl; assert(true); };
            return vkh::VkDeviceOrHostAddressConstKHR{ .deviceAddress = this->usage.eSharedDeviceAddress ? getDevice().getBufferAddress(vkh::VkBufferDeviceAddressInfo{.buffer = this->buffer }.hpp(), this->info.dispatch) : 0ull };
        };

    // 
    protected: friend VmaBufferAllocation; friend BufferAllocation; // 
        VmaAllocationInfo allocationInfo = {};
        VmaAllocation allocation = {};
        VmaAllocator allocator = {};
    };

    // Wrapper Class
    template<class T = uint8_t>
    class Vector { public: //
        ~Vector() {};
        Vector() {};
        Vector(vkt::uni_ptr<BufferAllocation> allocation, vkt::uni_arg<vk::DeviceSize> offset = 0ull, vkt::uni_arg<vk::DeviceSize> size = VK_WHOLE_SIZE) : allocation(allocation), bufInfo({ allocation->buffer, offset, size }) { this->construct(allocation, offset, size, sizeof(T)); };
        Vector(vkt::uni_ptr<VmaBufferAllocation> allocation, vkt::uni_arg<vk::DeviceSize> offset = 0ull, vkt::uni_arg<vk::DeviceSize> size = VK_WHOLE_SIZE) : allocation(allocation.dyn_cast<BufferAllocation>()), bufInfo({ allocation->buffer, offset, size }) { this->construct(allocation.dyn_cast<BufferAllocation>(), offset, size, sizeof(T)); };

        //Vector(vkt::uni_arg<MemoryAllocationInfo> allocationInfo, vkt::uni_arg<vkh::VkBufferCreateInfo> createInfo = {}) { this->construct(std::make_shared<BufferAllocation>(allocationInfo, createInfo)); };
        //Vector(vkt::uni_arg<VmaAllocator> allocator, vkt::uni_arg<vkh::VkBufferCreateInfo> createInfo = {}, vkt::uni_arg<VmaMemoryUsage> vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY) { this->construct(vkt::uni_ptr<BufferAllocation>(std::dynamic_pointer_cast<BufferAllocation>(std::make_shared<VmaBufferAllocation>(allocator, createInfo, vmaUsage)))); };

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
        virtual Vector<T>* construct(vkt::uni_ptr<BufferAllocation> allocation, vkt::uni_arg<vk::DeviceSize> offset = 0ull, vkt::uni_arg<vk::DeviceSize> size = VK_WHOLE_SIZE, vkt::uni_arg<vk::DeviceSize> stride = sizeof(T)) {
            this->allocation = allocation;
            this->bufInfo = vkh::VkDescriptorBufferInfo{ static_cast<VkBuffer>(allocation->buffer),offset,size };
            this->bufRegion = vkh::VkStridedBufferRegionKHR{ static_cast<VkBuffer>(allocation->buffer),offset,stride,this->ranged()/stride };
            return this;
        };

        // 
        virtual vk::BufferView& createBufferView(const vk::Format& format = vk::Format::eUndefined) {
            return (view = allocation->getDevice().createBufferView(vkh::VkBufferViewCreateInfo{
                .buffer = static_cast<VkBuffer>(this->bufRegion.buffer),
                .format = static_cast<VkFormat>(format),
                .offset = this->bufRegion.offset,
                .range = this->bufInfo.range
            }));
        };

        // 
        virtual vk::BufferView createBufferView(const vk::Format& format = vk::Format::eUndefined) const {
            return allocation->getDevice().createBufferView(vkh::VkBufferViewCreateInfo{
                .buffer = static_cast<VkBuffer>(this->bufRegion.buffer),
                .format = static_cast<VkFormat>(format),
                .offset = this->bufRegion.offset,
                .range = this->bufInfo.range
            });
        };

        // alias Of getAllocation
        virtual vkt::uni_ptr<BufferAllocation>& uniPtr() { return allocation; };
        virtual vkt::uni_ptr<BufferAllocation> uniPtr() const { return allocation; };

        // 
        virtual operator    ::VkDescriptorBufferInfo& () { this->bufInfo.buffer = this->bufRegion.buffer = allocation->buffer; return bufInfo; };
        virtual operator vkh::VkDescriptorBufferInfo& () { this->bufInfo.buffer = this->bufRegion.buffer = allocation->buffer; return bufInfo; };
        virtual operator vkt::uni_ptr<BufferAllocation>& () { return allocation; };
        virtual operator std::shared_ptr<BufferAllocation>& () { return allocation; };

        virtual operator vk::DescriptorBufferInfo& () { this->bufInfo.buffer = this->bufRegion.buffer = allocation->buffer; return bufInfo; };
        virtual operator vk::Buffer& () { return reinterpret_cast<vk::Buffer&>(this->bufInfo.buffer = this->bufRegion.buffer = allocation->buffer); };
        virtual operator vk::Device& () { return *allocation; };
        virtual operator vk::BufferView& () { return view; };
        
        virtual operator BufferAllocation*() { return allocation; };
        virtual operator VkBuffer& () { return reinterpret_cast<VkBuffer&>(bufInfo.buffer = allocation->buffer); };
        virtual operator VkDevice& () { return *allocation; };
        virtual operator VkBufferView& () { return reinterpret_cast<VkBufferView&>(view); };

        //
        virtual operator const    ::VkDescriptorBufferInfo& () const { return bufInfo; };
        virtual operator const vkh::VkDescriptorBufferInfo& () const { return bufInfo; };
        virtual operator vkt::uni_ptr<BufferAllocation> () const { return allocation; };
        virtual operator std::shared_ptr<BufferAllocation> () const { return allocation; };
        
        virtual operator const vk::DescriptorBufferInfo& () const { return bufInfo; };
        virtual operator const vk::Buffer& () const { return *allocation; };
        virtual operator const vk::Device& () const { return *allocation; };
        virtual operator const vk::BufferView& () const { return view; };

        virtual operator const BufferAllocation*() const { return allocation; };
        virtual operator const VkBuffer&() const { return *allocation; };
        virtual operator const VkDevice&() const { return *allocation; };
        virtual operator const VkBufferView& () const { return reinterpret_cast<const VkBufferView&>(view);; };

        // 
        virtual vk::DeviceSize& offset() { return this->bufRegion.offset; };
        //virtual vk::DeviceSize& stride() { return this->stride; };

        // 
        virtual const vk::DeviceSize& offset() const { return this->bufRegion.offset; };
        //virtual const vk::DeviceSize& stride() const { return this->stride; };

        // LEGACY, used for constructor only
        virtual vk::DeviceSize ranged() const { return (this->bufInfo.range != VK_WHOLE_SIZE ? std::min(vk::DeviceSize(this->bufInfo.range), vk::DeviceSize(this->allocation->range() - this->offset())) : vk::DeviceSize(this->allocation->range() - this->offset())); };

        // Get static and cached value
        virtual vk::DeviceSize& range() { return (this->bufInfo.range = (this->bufRegion.size * this->bufRegion.stride - 0u)); };
        virtual vk::DeviceSize  range() const { return (this->bufRegion.size * this->bufRegion.stride - 0u); };

        //virtual vk::DeviceSize size() const { return this->range() / this->stride; };
        virtual const vk::DeviceSize& size() const { return this->bufRegion.size; };
        virtual vk::DeviceSize& size() { return this->bufRegion.size; };

        // 
        virtual const vk::DeviceSize& stride() const { return this->bufRegion.stride; };
        virtual vk::DeviceSize& stride() { return this->bufRegion.stride; };

        // 
        virtual vk::Buffer& buffer() { return reinterpret_cast<vk::Buffer&>(allocation->buffer); };
        //virtual VkBuffer& buffer() { return reinterpret_cast<VkBuffer&>(allocation->buffer); };

        // 
        virtual const vk::Buffer& buffer() const { return reinterpret_cast<const vk::Buffer&>(allocation->buffer); };
        //virtual const VkBuffer& buffer() const { return reinterpret_cast<VkBuffer&>(allocation->buffer); };

        // typed casting 
        template<class Tm = T> inline Vector<Tm>& cast() { return reinterpret_cast<Vector<Tm>&>(*this); };
        template<class Tm = T> inline const Vector<Tm>& cast() const { return reinterpret_cast<const Vector<Tm>&>(*this); };

#ifdef ENABLE_OPENGL_INTEROP
        virtual GLuint& getGL() { return this->allocation->info.glID; };
        virtual const GLuint& getGL() const { return this->allocation->info.glID; };
#endif

        // 
        virtual bool has() const { return allocation ? true : false; };
        virtual bool has_value() const { return this->has(); };

        // 
        virtual BufferAllocation* operator->() { return allocation.ptr(); };
        virtual BufferAllocation& operator*() { return allocation.ref(); };
        virtual const BufferAllocation* operator->() const { return allocation.ptr(); };
        virtual const BufferAllocation& operator*() const { return allocation.ref(); };

        // 
        virtual vk::DeviceSize& rangeInfo() { return bufInfo.range; };
        virtual const vk::DeviceSize& rangeInfo() const { return bufInfo.range; };

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

        //
        virtual vk::Device& getDevice() { return allocation->getDevice(); };
        virtual const vk::Device& getDevice() const { return allocation->getDevice(); };

        // get deviceAddress with offset (currently, prefer unshifted)
        virtual vkh::VkDeviceOrHostAddressKHR deviceAddress() {
            //return vkh::VkDeviceOrHostAddressKHR{ .deviceAddress = getDevice().getBufferAddress(vkh::VkBufferDeviceAddressInfo{ .buffer = this->buffer() }.hpp()) + this->offset() };
            return this->allocation->deviceAddress();
        };

        // get deviceAddress with offset (currently, prefer unshifted)
        virtual vkh::VkDeviceOrHostAddressConstKHR deviceAddress() const {
            //return vkh::VkDeviceOrHostAddressConstKHR{ .deviceAddress = getDevice().getBufferAddress(vkh::VkBufferDeviceAddressInfo{ .buffer = this->buffer() }.hpp()) + this->offset() };
            return this->allocation->deviceAddress();
        };

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
        protected: friend Vector<T>; // 
        protected: vkh::VkDescriptorBufferInfo bufInfo = { {}, 0u, VK_WHOLE_SIZE }; // Cached Feature
        protected: vkh::VkStridedBufferRegionKHR bufRegion = { {}, 0u, sizeof(T), VK_WHOLE_SIZE };
        //public   : vk::DeviceSize stride = sizeof(T);
        protected: vk::BufferView view = {};
        protected: vkt::uni_ptr<BufferAllocation> allocation = {};
    };
};
