#pragma once // #

#ifdef VKT_FORCE_VMA_IMPLEMENTATION
#ifndef VMA_IMPLEMENTATION
#define VMA_IMPLEMENTATION
#endif
#endif

#include "utils.hpp"
#include "structs.hpp"
#include <memory>

namespace vkt {

    // 
    class VmaBufferAllocation;
    class BufferAllocation : public std::enable_shared_from_this<BufferAllocation> { public:
        BufferAllocation() {};
        BufferAllocation(const BufferAllocation& allocation) : buffer(allocation.buffer), range(allocation.range), device(allocation.device), pMapped(allocation.pMapped) { *this = allocation; };
        BufferAllocation& operator=(const BufferAllocation& allocation) { 
            this->buffer = allocation.buffer;
            this->device = allocation.device;
            this->range = allocation.range;
            this->pMapped = allocation.pMapped;
            return *this;
        };

        // 
        virtual void* map() { return pMapped; };
        virtual void* mapped() { return pMapped; };
        virtual void  unmap() {};

        // 
        virtual operator vk::Buffer& () { return buffer; };
        virtual operator VkBuffer& () { return reinterpret_cast<VkBuffer&>(buffer); };

        // 
        virtual operator const vk::Buffer& () const { return buffer; };
        virtual operator const VkBuffer& () const { return reinterpret_cast<const VkBuffer&>(buffer); };

        // VMA HACK FOR EXTRACT DEVICE
        virtual operator const vk::Device& () const { return device; };
        virtual operator const VkDevice& () const { return reinterpret_cast<const VkDevice&>(device); };

        // 
        virtual operator vk::Device& () { return device; };
        virtual operator VkDevice& () { return reinterpret_cast<VkDevice&>(device); };

        //
        virtual vk::Device& getDevice() { return device; };
        virtual const vk::Device& getDevice() const { return device; };


    public: // in-variant 
        vk::Buffer buffer = {};
        vk::Device device = {};
        vk::DeviceSize range = 0ull;
        void* pMapped = nullptr;

    protected: friend BufferAllocation; friend VmaBufferAllocation;
    };

    // 
    class VmaBufferAllocation : public BufferAllocation { public:
        //virtual ~VmaBufferAllocation() { vmaDestroyBuffer(allocator, *this, allocation); };
        VmaBufferAllocation() {};
        VmaBufferAllocation(
            const VmaAllocator& allocator,
            const vkh::VkBufferCreateInfo& createInfo = {},
            VmaMemoryUsage vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY
        ) {
            VmaAllocationCreateInfo vmaInfo = {}; vmaInfo.usage = vmaUsage;
            if (vmaUsage == VMA_MEMORY_USAGE_CPU_TO_GPU || vmaUsage == VMA_MEMORY_USAGE_GPU_TO_CPU) { vmaInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; };
            auto result = vmaCreateBuffer(this->allocator = allocator, createInfo, &vmaInfo, &reinterpret_cast<VkBuffer&>(buffer), &allocation, &allocationInfo); 
            assert(result == VK_SUCCESS);
            this->range = createInfo.size;
            this->device = this->_getDevice();
        };

        // 
        VmaBufferAllocation(const VmaBufferAllocation& allocation) : allocation(allocation.allocation), allocationInfo(allocation.allocationInfo), allocator(allocation.allocator) { *this = allocation; };
        VmaBufferAllocation(const BufferAllocation& allocation) { *this = dynamic_cast<const VmaBufferAllocation&>(allocation); };

        // 
        VmaBufferAllocation& operator=(const VmaBufferAllocation& allocation) {
            if (allocation.allocation) {
                vmaDestroyBuffer(allocator, *this, allocation); // don't assign into already allocated
            };
            this->buffer = allocation.buffer;
            this->allocation = allocation.allocation;
            this->allocationInfo = allocation.allocationInfo;
            this->allocator = allocation.allocator;
            this->range = allocation.range;
            this->device = allocation.device;
            return *this;
        };

        // Get mapped memory
        virtual void* map() { void* ptr = nullptr; vmaMapMemory(allocator, allocation, &ptr); return ptr; };
        virtual void* mapped() { if (!allocationInfo.pMappedData) { vmaMapMemory(allocator, allocation, &allocationInfo.pMappedData); }; return allocationInfo.pMappedData; };
        virtual void  unmap() { vmaUnmapMemory(allocator, allocation); allocationInfo.pMappedData = nullptr; };

        // 
        virtual const vk::Device& _getDevice() const;// const override { return device; };

        // Allocation
        virtual operator const VmaAllocation& () const { return allocation; };
        virtual operator const VmaAllocationInfo& () const { return allocationInfo; };

    // 
    protected: friend VmaBufferAllocation; friend BufferAllocation; // 
        VmaAllocation allocation = {};
        VmaAllocationInfo allocationInfo = {};
        VmaAllocator allocator = {};
    };


    // 
    class VmaImageAllocation;
    class ImageAllocation : public std::enable_shared_from_this<ImageAllocation> { public: 
        ImageAllocation() {};
        ImageAllocation(const ImageAllocation& allocation) : image(allocation.image), device(allocation.device), pMapped(allocation.pMapped) { *this = allocation; };
        ImageAllocation& operator=(const ImageAllocation& allocation) {
            this->image = allocation.image;
            this->device = allocation.device;
            this->pMapped = allocation.pMapped;
            return *this;
        };

        // 
        virtual operator const vk::Image& () const { return this->image; };
        virtual operator const VkImage& () const { return this->image; };

        // 
        virtual operator vk::Image& () { return this->image; };
        virtual operator VkImage& () { return reinterpret_cast<VkImage&>(this->image); };

        // 
        virtual operator const vk::Device& () const { return device; };
        virtual operator const VkDevice& () const { return reinterpret_cast<const VkDevice&>(device); };

        //
        virtual vk::Device& getDevice() { return device; };
        virtual const vk::Device& getDevice() const { return device; };

        // 
        virtual const vk::Device& _getDevice() const { return device; };

        // 
        virtual operator vk::Device& () { return device; };
        virtual operator VkDevice& () { return reinterpret_cast<VkDevice&>(device); };

        // Get mapped memory
        virtual void* map() { return pMapped; };
        virtual void* mapped() { return pMapped; };
        virtual void  unmap() {};

    // 
    protected: friend VmaImageAllocation; friend ImageAllocation;
        vk::Image image = {};
        vk::Device device = {};
        void* pMapped = nullptr;
    };

    // 
    class VmaImageAllocation : public ImageAllocation { public:
        VmaImageAllocation() {};
        VmaImageAllocation(const VmaImageAllocation& allocation) : allocation(allocation.allocation), allocationInfo(allocation.allocationInfo), allocator(allocation.allocator) { *this = allocation; };
        VmaImageAllocation(
            const VmaAllocator& allocator,
            const vkh::VkImageCreateInfo& createInfo = {},
            VmaMemoryUsage vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY
        ) {
            VmaAllocationCreateInfo vmaInfo = {}; vmaInfo.usage = vmaUsage;
            if (vmaUsage == VMA_MEMORY_USAGE_CPU_TO_GPU || vmaUsage == VMA_MEMORY_USAGE_GPU_TO_CPU) { vmaInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; };
            auto result = vmaCreateImage(this->allocator = allocator, createInfo, &vmaInfo, &reinterpret_cast<VkImage&>(image), &allocation, &allocationInfo);
            assert(result == VK_SUCCESS);
            this->device = this->_getDevice();
        };

        // 
        VmaImageAllocation& operator=(const VmaImageAllocation& allocation) {
            if (allocation.allocation) {
                vmaDestroyImage(allocator, *this, allocation); // don't assign into already allocated
            };
            this->image = allocation.image;
            this->allocation = allocation.allocation;
            this->allocationInfo = allocation.allocationInfo;
            this->allocator = allocation.allocator;
            this->device = allocation.device;
            return *this;
        };

        // Get mapped memory
        virtual void* map() override { void* ptr = nullptr; vmaMapMemory(allocator, allocation, &ptr); return ptr; };
        virtual void* mapped() override { if (!allocationInfo.pMappedData) { vmaMapMemory(allocator, allocation, &allocationInfo.pMappedData); }; return allocationInfo.pMappedData; };
        virtual void unmap() override { vmaUnmapMemory(allocator, allocation); allocationInfo.pMappedData = nullptr; };

        // Allocation
        virtual operator const VmaAllocation& () const { return allocation; };
        virtual operator const VmaAllocationInfo& () const { return allocationInfo; };

        // 
        virtual const vk::Device& _getDevice() const; //override;

        // 
        virtual operator VmaAllocation& () { return allocation; };
        virtual operator VmaAllocationInfo& () { return allocationInfo; };

    // 
    protected: friend VmaImageAllocation; friend ImageAllocation; // 
        VmaAllocation allocation = {};
        VmaAllocationInfo allocationInfo = {};
        VmaAllocator allocator = {};
    };

#ifdef VMA_IMPLEMENTATION // Fix Implementation Issue
    const vk::Device& VmaBufferAllocation::_getDevice() const { return this->allocator->m_hDevice; };
    const vk::Device& VmaImageAllocation::_getDevice() const { return this->allocator->m_hDevice; };
#endif

    // 
    class ImageRegion : public std::enable_shared_from_this<ImageRegion> { public: 
        ImageRegion(){};

        ImageRegion(const std::shared_ptr<ImageAllocation>& allocation, const vkh::VkImageViewCreateInfo& info = {}, const vk::ImageLayout& layout = vk::ImageLayout::eGeneral) : allocation(allocation), subresourceRange(info.subresourceRange) {
            this->imgInfo.imageView = allocation->getDevice().createImageView(vk::ImageViewCreateInfo(info).setImage(*allocation));
            this->imgInfo.imageLayout = VkImageLayout(layout);
        };

        ImageRegion(const ImageRegion& region) {
            this->allocation = region; 
            this->subresourceRange = region.getImageSubresourceRange();
            this->imgInfo = vk::DescriptorImageInfo(region);
        };

        virtual ImageRegion& operator=(const ImageRegion& region) {
            this->allocation = region.allocation;
            this->subresourceRange = region.subresourceRange;
            this->imgInfo = region.imgInfo;
            return *this;
        };

        //virtual vkh::VkImageSubresourceRange& subresourceRange() { return this->subresourceRange; };
        virtual vkh::VkImageSubresourceLayers subresourceLayers(const uint32_t mipLevel =  0u) const { return {
            .aspectMask = this->subresourceRange.aspectMask,
            .mipLevel = this->subresourceRange.baseMipLevel + mipLevel,
            .baseArrayLayer = this->subresourceRange.baseArrayLayer,
            .layerCount = this->subresourceRange.layerCount
        };};

        // set methods for direct control
        virtual ImageRegion& setImageLayout(const VkImageLayout layout = {}) { this->imgInfo.imageLayout = layout; return *this; };
        virtual ImageRegion& setSampler(const VkSampler& sampler = {}) { this->imgInfo.sampler = sampler; return *this; };
        virtual ImageRegion& setImageLayout(const vk::ImageLayout layout = {}) { this->imgInfo.imageLayout = reinterpret_cast<const VkImageLayout&>(layout); return *this; };
        virtual ImageRegion& setSampler(const vk::Sampler& sampler = {}) { this->imgInfo.sampler = reinterpret_cast<const VkSampler&>(sampler); return *this; };

        // 
        virtual operator std::shared_ptr<ImageAllocation>&() { return this->allocation; };
        virtual operator vkh::VkImageSubresourceRange&() { return this->subresourceRange; };
        virtual operator vkh::VkDescriptorImageInfo&() { return this->imgInfo; };
        virtual operator vk::DescriptorImageInfo&() { return this->imgInfo; };
        virtual operator vk::ImageSubresourceRange&() { return this->subresourceRange; };
        virtual operator vk::ImageView&() { return reinterpret_cast<vk::ImageView&>(this->imgInfo.imageView); };
        virtual operator vk::ImageLayout&() { return reinterpret_cast<vk::ImageLayout&>(this->imgInfo.imageLayout); };
        virtual operator vk::Image&() { return *this->allocation; };
        virtual operator vk::Sampler&() { return reinterpret_cast<vk::Sampler&>(this->imgInfo.sampler); };
        virtual operator vk::Device&() { return *this->allocation; };
        virtual operator ::VkDescriptorImageInfo&() { return this->imgInfo; };
        virtual operator ::VkImageSubresourceRange&() { return this->subresourceRange; };
        virtual operator VkImageView&() { return reinterpret_cast<VkImageView&>(this->imgInfo.imageView); };
        virtual operator VkImageLayout&() { return reinterpret_cast<VkImageLayout&>(this->imgInfo.imageLayout); };
        virtual operator VkImage&() { return *this->allocation; };
        virtual operator VkSampler&() { return this->imgInfo.sampler; };
        virtual operator VkDevice&() { return reinterpret_cast<VkDevice&>(this->allocation->getDevice()); };

        // 
        virtual operator const std::shared_ptr<ImageAllocation>&() const { return this->allocation; };
        virtual operator const vkh::VkImageSubresourceRange&() const { return this->subresourceRange; };
        virtual operator const vkh::VkDescriptorImageInfo&() const { return this->imgInfo; };
        virtual operator const vk::DescriptorImageInfo&() const { return this->imgInfo; };
        virtual operator const vk::ImageSubresourceRange&() const { return this->subresourceRange; };
        virtual operator const vk::ImageView&() const { return reinterpret_cast<const vk::ImageView&>(this->imgInfo.imageView); };
        virtual operator const vk::ImageLayout&() const { return reinterpret_cast<const vk::ImageLayout&>(this->imgInfo.imageLayout); };
        virtual operator const vk::Image&() const { return *this->allocation; };
        virtual operator const vk::Sampler&() const { return reinterpret_cast<const vk::Sampler&>(this->imgInfo.sampler); };
        virtual operator const vk::Device&() const { return *this->allocation; };
        virtual operator const ::VkDescriptorImageInfo&() const { return this->imgInfo; };
        virtual operator const ::VkImageSubresourceRange&() const { return this->subresourceRange; };
        virtual operator const VkImageView&() const { return this->imgInfo.imageView; };
        virtual operator const VkImageLayout&() const { return this->imgInfo.imageLayout; };
        virtual operator const VkImage&() const { return *this->allocation; };
        virtual operator const VkSampler&() const { return this->imgInfo.sampler; };
        virtual operator const VkDevice& () const { return reinterpret_cast<VkDevice&>(this->allocation->getDevice()); };
        virtual operator const vk::ImageSubresourceLayers() const { return vk::ImageSubresourceLayers{ reinterpret_cast<const vk::ImageAspectFlags&>(subresourceRange.aspectMask), subresourceRange.baseMipLevel, subresourceRange.baseArrayLayer, subresourceRange.layerCount }; };

        // 
        vk::Image& getImage() { return *this->allocation; };
        vk::ImageView& getImageView() { return reinterpret_cast<vk::ImageView&>(this->imgInfo.imageView); };
        vk::ImageLayout& getImageLayout() { return reinterpret_cast<vk::ImageLayout&>(this->imgInfo.imageLayout); };
        vk::Sampler& getSampler() { return reinterpret_cast<vk::Sampler&>(this->imgInfo.sampler); };
        vk::ImageSubresourceRange& getImageSubresourceRange() { return this->subresourceRange; };

        // 
        const vk::Image& getImage() const { return *this->allocation; };
        const vk::ImageView& getImageView() const { return reinterpret_cast<const vk::ImageView&>(this->imgInfo.imageView); };
        const vk::ImageLayout& getImageLayout() const { return reinterpret_cast<const vk::ImageLayout&>(this->imgInfo.imageLayout); };
        const vk::Sampler& getSampler() const { return reinterpret_cast<const vk::Sampler&>(this->imgInfo.sampler); };
        const vk::ImageSubresourceRange& getImageSubresourceRange() const { return this->subresourceRange; };

        // 
        virtual ImageAllocation* operator->() { return &(*allocation); };
        virtual ImageAllocation& operator*() { return (*allocation); };
        virtual const ImageAllocation* operator->() const { return &(*allocation); };
        virtual const ImageAllocation& operator*() const { return (*allocation); };

    protected: friend VmaImageAllocation; friend ImageAllocation; // 
        vkh::VkDescriptorImageInfo imgInfo = {};
        std::shared_ptr<ImageAllocation> allocation = {};

    public: // irrevalent sources
        vkh::VkImageSubresourceRange subresourceRange = {};
    };

    // Wrapper Class
    template<class T = uint8_t>
    class Vector { // 
    public:
        Vector() {};
        Vector(const std::shared_ptr<BufferAllocation>& allocation, vk::DeviceSize offset = 0u, vk::DeviceSize size = VK_WHOLE_SIZE) : allocation(allocation), bufInfo({ allocation->buffer,offset,size }), stride(sizeof(T)) {};

        //  
        template<class Tm = T> Vector(const Vector<Tm>& V) : allocation(V), bufInfo({ V.buffer(), V.offset(), V.range() }), stride(sizeof(T)) {};
        template<class Tm = T> Vector<T>& operator=(const Vector<Tm>& V) { this->allocation = V, this->bufInfo = vk::DescriptorBufferInfo(V.buffer(), V.offset(), V.range()), this->stride = sizeof(T); return *this; };

        // 
        virtual void unmap() { allocation->unmap(); };
        virtual const T* map(const uintptr_t& i = 0u) const { auto map = reinterpret_cast<const uint8_t*>(allocation->map()) + offset(); return &(reinterpret_cast<const T*>(map))[i]; };
        virtual T* const map(const uintptr_t& i = 0u) { auto map = reinterpret_cast<uint8_t*>(allocation->map()) + offset(); return &(reinterpret_cast<T*>(map))[i]; };

        // 
        virtual const T* mapped(const uintptr_t& i = 0u) const { auto map = reinterpret_cast<const uint8_t*>(allocation->mapped()) + offset(); return &(reinterpret_cast<const T*>(map))[i]; };
        virtual T* const mapped(const uintptr_t& i = 0u) { auto map = reinterpret_cast<uint8_t*>(allocation->mapped()) + offset(); return &(reinterpret_cast<T*>(map))[i]; };

        // 
        virtual T* const data() { return mapped(); };
        virtual const T* data() const { return mapped(); };

        // 
        virtual vk::BufferView& createBufferView(const vk::Format& format = vk::Format::eUndefined) {
            vkh::VkBufferViewCreateInfo info = {};
            info.buffer = bufInfo.buffer;
            info.offset = bufInfo.offset;
            info.range = bufInfo.range;
            info.format = VkFormat(format); // TODO: AUTO-FORMAT
            return (view = allocation->getDevice().createBufferView(info));
        };

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
        virtual operator std::shared_ptr<BufferAllocation>& () { return allocation; };
        virtual operator vkh::VkDescriptorBufferInfo& () { bufInfo.buffer = allocation->buffer; return reinterpret_cast<vkh::VkDescriptorBufferInfo&>(bufInfo); };
        virtual operator vk::DescriptorBufferInfo& () { bufInfo.buffer = allocation->buffer; return bufInfo; };
        virtual operator vk::Buffer& () { return reinterpret_cast<vk::Buffer&>(bufInfo.buffer = allocation->buffer); };
        virtual operator vk::Device& () { return *allocation; };
        virtual operator vk::BufferView& () { return view; };
        virtual operator ::VkDescriptorBufferInfo& () { bufInfo.buffer = allocation->buffer; return reinterpret_cast<::VkDescriptorBufferInfo&>(bufInfo); };
        virtual operator VkBuffer& () { return reinterpret_cast<VkBuffer&>(bufInfo.buffer = allocation->buffer); };
        virtual operator VkDevice& () { return *allocation; };
        virtual operator VkBufferView& () { return reinterpret_cast<VkBufferView&>(view); };

        //
        virtual operator const std::shared_ptr<BufferAllocation>& () const { return allocation; };
        virtual operator const vkh::VkDescriptorBufferInfo& () const { return reinterpret_cast<const vkh::VkDescriptorBufferInfo&>(bufInfo); };
        virtual operator const vk::DescriptorBufferInfo& () const { return bufInfo; };
        virtual operator const vk::Buffer& () const { return *allocation; };
        virtual operator const vk::Device& () const { return *allocation; };
        virtual operator const vk::BufferView& () const { return view; };
        virtual operator const ::VkDescriptorBufferInfo& () const { return reinterpret_cast<const ::VkDescriptorBufferInfo&>(bufInfo); };
        virtual operator const VkBuffer&() const { return *allocation; };
        virtual operator const VkDevice&() const { return *allocation; };
        virtual operator const VkBufferView& () const { return reinterpret_cast<const VkBufferView&>(view); };

        // 
        virtual vk::DeviceSize& offset() { return this->bufInfo.offset; };
        //virtual vk::DeviceSize& stride() { return this->stride; };

        // 
        virtual const vk::DeviceSize& offset() const { return this->bufInfo.offset; };
        //virtual const vk::DeviceSize& stride() const { return this->stride; };

        // 
        virtual vk::DeviceSize range() const { return (this->bufInfo.range != VK_WHOLE_SIZE ? std::min(this->bufInfo.range, this->allocation->range - this->bufInfo.offset) : (this->allocation->range - this->bufInfo.offset)); };
        virtual vk::DeviceSize size() const { return this->range() / this->stride; };

        // 
        virtual vk::Buffer& buffer() { return reinterpret_cast<vk::Buffer&>(allocation->buffer); };
        //virtual VkBuffer& buffer() { return reinterpret_cast<VkBuffer&>(allocation->buffer); };

        // 
        virtual const vk::Buffer& buffer() const { return reinterpret_cast<vk::Buffer&>(allocation->buffer); };
        //virtual const VkBuffer& buffer() const { return reinterpret_cast<VkBuffer&>(allocation->buffer); };

        // typed casting 
        template<class Tm = T> Vector<Tm>& cast() { return reinterpret_cast<Vector<Tm>&>(*this); };
        template<class Tm = T> const Vector<Tm>& cast() const { return reinterpret_cast<const Vector<Tm>&>(*this); };

        // 
        virtual bool has() const { return allocation ? true : false; };

        // 
        virtual BufferAllocation* operator->() { return &(*allocation); };
        virtual BufferAllocation& operator*() { return (*allocation); };
        virtual const BufferAllocation* operator->() const { return &(*allocation); };
        virtual const BufferAllocation& operator*() const { return (*allocation); };

        // 
        virtual vk::DeviceSize& rangeInfo() { return bufInfo.range; };
        virtual const vk::DeviceSize& rangeInfo() const { return bufInfo.range; };

        //
        protected: friend Vector<T>; // 
        protected: vk::DescriptorBufferInfo bufInfo = {};
        public   : vk::DeviceSize stride = sizeof(T);
        protected: vk::BufferView view = {};
        protected: std::shared_ptr<BufferAllocation> allocation = {};
    };
};
