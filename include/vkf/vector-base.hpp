#pragma once // #

//
#ifndef VKT_CORE_USE_VMA
#define VKT_CORE_USE_VMA
#endif

//
#include "./buffer-allocation.hpp"

#ifdef VKT_CORE_USE_VMA
#include "./vma-buffer-allocation.hpp"
#endif

// 
namespace vkf {

    template<class T = uint8_t> class Vector;

    class VectorBase : public std::enable_shared_from_this<VectorBase> {
    public: using T = uint8_t;
        VectorBase() {};
        VectorBase(const vkt::uni_ptr<BufferAllocation>& allocation, vkt::uni_arg<VkDeviceSize> offset = uint64_t(0ull), vkt::uni_arg<VkDeviceSize> size = VK_WHOLE_SIZE, vkt::uni_arg<VkDeviceSize> stride = 1u) : allocation(allocation), bufInfo({ allocation->buffer, offset, size }) { this->construct(allocation, offset, size, stride); };
        VectorBase(const std::shared_ptr<BufferAllocation>& allocation, vkt::uni_arg<VkDeviceSize> offset = uint64_t(0ull), vkt::uni_arg<VkDeviceSize> size = VK_WHOLE_SIZE, vkt::uni_arg<VkDeviceSize> stride = 1u) : allocation(allocation), bufInfo({ allocation->buffer, offset, size }) { this->construct(allocation, offset, size, stride); };

        // 
#ifdef VKT_CORE_USE_VMA
        VectorBase(const vkt::uni_ptr<VmaBufferAllocation>& allocation, vkt::uni_arg<VkDeviceSize> offset = uint64_t(0ull), vkt::uni_arg<VkDeviceSize> size = VK_WHOLE_SIZE, vkt::uni_arg<VkDeviceSize> stride = 1u) : allocation(allocation.dyn_cast<BufferAllocation>()), bufInfo({ allocation->buffer, offset, size }) { this->construct(allocation.dyn_cast<BufferAllocation>(), offset, size, stride); };
        VectorBase(const std::shared_ptr<VmaBufferAllocation>& allocation, vkt::uni_arg<VkDeviceSize> offset = uint64_t(0ull), vkt::uni_arg<VkDeviceSize> size = VK_WHOLE_SIZE, vkt::uni_arg<VkDeviceSize> stride = 1u) : allocation(std::dynamic_pointer_cast<BufferAllocation>(allocation)), bufInfo({ allocation->buffer, offset, size }) { this->construct(std::dynamic_pointer_cast<BufferAllocation>(allocation), offset, size, stride); };
#endif

        //
        virtual const uint8_t* mapv(const uintptr_t& i = 0u) const { const_cast<VectorBase*>(this)->pMapped = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(allocation->mapped()) + offset()); return &pMapped[i]; };
        virtual uint8_t* const mapv(const uintptr_t& i = 0u) { this->pMapped = reinterpret_cast<uint8_t*>(allocation->mapped()) + offset(); return &pMapped[i]; };

        //
        virtual const uint8_t* mappedv(const uintptr_t& i = 0u) const { const_cast<VectorBase*>(this)->pMapped = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(allocation->mapped()) + offset()); return &pMapped[i]; };
        virtual uint8_t* const mappedv(const uintptr_t& i = 0u) { this->pMapped = reinterpret_cast<uint8_t*>(allocation->mapped()) + offset(); return &pMapped[i]; };

        //
        virtual VectorBase* construct(vkt::uni_ptr<BufferAllocation> allocation, vkt::uni_arg<VkDeviceSize> offset = uint64_t(0ull), vkt::uni_arg<VkDeviceSize> size = VK_WHOLE_SIZE, vkt::uni_arg<VkDeviceSize> stride = 1u) {
            const auto striding = (*stride != 0ull ? *stride : 1ull);
            const auto rangeLim = this->ranged();
            this->allocation = allocation;
            this->bufInfo = vkh::VkDescriptorBufferInfo{ static_cast<VkBuffer>(allocation->buffer), offset, size };
            this->bufRegion = vkh::VkStridedDeviceAddressRegionKHR{ deviceAddress(), striding, (rangeLim / striding) * striding };
            this->bufInfo.range = rangeLim;
            return this;
        };

        // 
        virtual VkBufferView& createBufferView(const VkFormat& format = VK_FORMAT_UNDEFINED) {
            vkh::handleVk(this->allocation->info.deviceDispatch->CreateBufferView(vkh::VkBufferViewCreateInfo{
                .buffer = static_cast<VkBuffer>(this->bufInfo.buffer),
                .format = static_cast<VkFormat>(format),
                .offset = this->bufInfo.offset,
                .range = this->bufInfo.range
            }, nullptr, &view));
            return view;
        };

        // alias Of getAllocation
        virtual vkt::uni_ptr<BufferAllocation>& uniPtr() { return allocation; };
        virtual vkt::uni_ptr<BufferAllocation> uniPtr() const { return allocation; };

        //
        template<class Tm = T> VectorBase(const vkt::uni_arg<Vector<Tm>>& V) : allocation(V), bufInfo({ V.buffer(), V.offset(), V.ranged() }) { *this = V; };
        template<class Tm = T> inline VectorBase& operator=(const vkt::uni_arg<Vector<Tm>>& V) {
            this->allocation = V.uniPtr();
            this->bufInfo = vkh::VkDescriptorBufferInfo{ static_cast<VkBuffer>(V.buffer()), V.offset(), V.ranged() };
            this->bufRegion = vkh::VkStridedDeviceAddressRegionKHR{ V.deviceAddress(), V.stride(), (V.ranged() / V.stride())*V.stride() };
            this->bufInfo.range = this->bufRegion.size;
            return *this;
        };

        // 
        virtual operator    ::VkDescriptorBufferInfo& () { this->bufInfo.buffer = this->bufInfo.buffer = allocation->buffer; return bufInfo; };
        virtual operator vkh::VkDescriptorBufferInfo& () { this->bufInfo.buffer = this->bufInfo.buffer = allocation->buffer; return bufInfo; };
        virtual operator vkt::uni_ptr<BufferAllocation>& () { return allocation; };
        virtual operator std::shared_ptr<BufferAllocation>& () { return allocation; };

        //
        virtual operator const ::VkDescriptorBufferInfo& () const { return bufInfo; };
        virtual operator const vkh::VkDescriptorBufferInfo& () const { return bufInfo; };
        virtual operator const vkt::uni_ptr<BufferAllocation>& () const { return allocation; };
        virtual operator const std::shared_ptr<BufferAllocation>& () const { return allocation.get_shared(); };

        // 
        virtual operator BufferAllocation* () { return allocation; };
        virtual operator const BufferAllocation* () const { return allocation; };

        // 
        virtual operator VkBufferView& () { return view; };
        virtual operator VkDevice& () { return *allocation; };
        virtual operator VkBuffer& () { return *allocation; };

        // 
        virtual operator const VkBuffer& () const { return *allocation; };
        virtual operator const VkDevice& () const { return *allocation; };
        virtual operator const VkBufferView& () const { return view; };

        // typed casting 
        template<class Tm = T> inline Vector<Tm>& cast() { return dynamic_cast<Vector<Tm>&>(*this); };
        template<class Tm = T> inline const Vector<Tm>& cast() const { return dynamic_cast<const Vector<Tm>&>(*this); };

        // 
        virtual VkDeviceSize& offset() { return this->bufInfo.offset; };
        virtual const VkDeviceSize& offset() const { return this->bufInfo.offset; };

        // LEGACY, used for constructor only
        virtual VkDeviceSize ranged() const { return (this->bufInfo.range != VK_WHOLE_SIZE ? std::min(VkDeviceSize(this->bufInfo.range), VkDeviceSize(this->allocation->range() - this->offset())) : VkDeviceSize(this->allocation->range() - this->offset())); };

        // Get static and cached value
        virtual VkDeviceSize& range() { return (this->bufInfo.range = (this->size() * this->bufRegion.stride - 0u)); };
        virtual VkDeviceSize  range() const { return (this->size() * this->bufRegion.stride - 0u); };

        //
        virtual VkDeviceSize size() const { return (this->bufInfo.range/this->bufRegion.stride); };
        //virtual const VkDeviceSize& size() const { return this->bufRegion.size; };
        //virtual VkDeviceSize& size() { return this->bufRegion.size; };

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


        virtual unsigned& getGL() { return this->allocation->info.glID; };
        virtual const unsigned& getGL() const { return this->allocation->info.glID; };

        // 
        virtual bool has() const { return (allocation && allocation.has()) ? true : false; };
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
        virtual VectorBase* address() { return this; };
        virtual const VectorBase* address() const { return this; };

        // 
        virtual vkh::VkDescriptorBufferInfo& getDescriptor() { return bufInfo; };
        virtual const vkh::VkDescriptorBufferInfo& getDescriptor() const { return bufInfo; };

        // 
        virtual ::VkDescriptorBufferInfo* getDescriptorPtr() { return &bufInfo; };
        virtual const ::VkDescriptorBufferInfo* getDescriptorPtr() const { return &bufInfo; };

        // alias of uniPtr
        virtual vkt::uni_ptr<BufferAllocation>& getAllocation() { return allocation; };
        virtual const vkt::uni_ptr<BufferAllocation>& getAllocation() const { return allocation; };

        // for JavaCPP
        virtual BufferAllocation* getAllocationPtr() { return allocation.ptr(); };
        virtual const BufferAllocation* getAllocationPtr() const { return allocation.ptr(); };

        // get deviceAddress with offset
        virtual vkh::VkDeviceOrHostAddressKHR deviceAddress() {
            auto deviceAddress = this->allocation->deviceAddress();
            deviceAddress.deviceAddress += this->offset();
            return deviceAddress;
        };

        // get deviceAddress with offset
        virtual vkh::VkDeviceOrHostAddressConstKHR deviceAddress() const {
            auto deviceAddress = this->allocation->deviceAddress();
            deviceAddress.deviceAddress += this->offset();
            return deviceAddress;
        };

        // 
        virtual std::vector<uint32_t>& getQueueFamilyIndices() { return this->allocation->getQueueFamilyIndices(); };
        virtual const std::vector<uint32_t>& getQueueFamilyIndices() const { return this->allocation->getQueueFamilyIndices(); };

        // 
        virtual vkh::VkStridedDeviceAddressRegionKHR& getRegion() { return bufRegion; };
        virtual const vkh::VkStridedDeviceAddressRegionKHR& getRegion() const { return bufRegion; };

        // getter by operator (for direct pass)
        virtual operator vkh::VkDeviceOrHostAddressKHR() { return this->deviceAddress(); };
        virtual operator vkh::VkDeviceOrHostAddressConstKHR() const { return this->deviceAddress(); };

        // 
        virtual void unmap() { allocation->unmap(); this->pMapped = nullptr; };

        // 
        virtual unsigned& getGLBuffer() { return this->allocation->getGLBuffer(); };
        virtual unsigned& getGLMemory() { return this->allocation->getGLMemory(); };

        // 
        virtual const unsigned& getGLBuffer() const { return this->allocation->getGLBuffer(); };
        virtual const unsigned& getGLMemory() const { return this->allocation->getGLMemory(); };

        // 
        virtual vkh::VkBufferCreateInfo& getCreateInfo() { return allocation->getCreateInfo(); };
        virtual const vkh::VkBufferCreateInfo& getCreateInfo() const { return allocation->getCreateInfo(); };

        //
        virtual vkf::MemoryAllocationInfo& getAllocationInfo() { return allocation->getInfo(); };
        virtual const vkf::MemoryAllocationInfo& getAllocationInfo() const { return allocation->getInfo(); };

        // 
        //protected: friend Vector<T>; // 
        protected: VkBufferView view = VK_NULL_HANDLE;
        protected: vkh::VkDescriptorBufferInfo bufInfo = { VK_NULL_HANDLE, 0u, VK_WHOLE_SIZE }; // Cached Feature
        protected: vkh::VkStridedDeviceAddressRegionKHR bufRegion = { 0u, 1u, VK_WHOLE_SIZE };
        protected: vkt::uni_ptr<BufferAllocation> allocation = {};
        protected: uint8_t* pMapped = nullptr;
        //protected: T pMapped[8] = nullptr;
    };

};
