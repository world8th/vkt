#ifdef VKT_FORCE_VMA_IMPLEMENTATION
#ifndef VMA_IMPLEMENTATION
#define VMA_IMPLEMENTATION
#endif
#endif

#include "utils.hpp"
#include "structs.hpp"

namespace vkt {

    // 
    class VmaBufferAllocation : public std::enable_shared_from_this<VmaBufferAllocation> { public:
        //inline ~VmaBufferAllocation() { vmaDestroyBuffer(allocator, *this, allocation); };
         inline VmaBufferAllocation() {};
         inline VmaBufferAllocation(
            const VmaAllocator& allocator,
            const vkh::VkBufferCreateInfo& createInfo = {},
            VmaMemoryUsage vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY
        ) {
            VmaAllocationCreateInfo vmaInfo = {}; vmaInfo.usage = vmaUsage;
            if (vmaUsage == VMA_MEMORY_USAGE_CPU_TO_GPU || vmaUsage == VMA_MEMORY_USAGE_GPU_TO_CPU) { vmaInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; };
            assert(vmaCreateBuffer(this->allocator = allocator, createInfo, &vmaInfo, &(VkBuffer&)buffer, &allocation, &allocationInfo) == VK_SUCCESS);
            this->range = createInfo.size;
        };
         inline VmaBufferAllocation(const VmaBufferAllocation& allocation) : buffer(allocation.buffer), allocation(allocation.allocation), allocationInfo(allocation.allocationInfo), allocator(allocation.allocator), range(allocation.range) {};
         inline VmaBufferAllocation& operator=(const VmaBufferAllocation& allocation) {
            vmaDestroyBuffer(allocator, *this, allocation); // don't assign into already allocated
            this->buffer = allocation.buffer;
            this->allocation = allocation.allocation;
            this->allocationInfo = allocation.allocationInfo;
            this->allocator = allocation.allocator;
            this->range = allocation.range;
            return *this;
        };

        // Get mapped memory
        inline void* map() { void* ptr = nullptr; vmaMapMemory(allocator, allocation, &ptr); return ptr; };
        inline void* mapped() { if (!allocationInfo.pMappedData) { vmaMapMemory(allocator, allocation, &allocationInfo.pMappedData); }; return allocationInfo.pMappedData; };
        inline void unmap() { vmaUnmapMemory(allocator, allocation); allocationInfo.pMappedData = nullptr; };

        // 
        inline operator vk::Buffer& () { return buffer; };
        inline operator VkBuffer& () { return (VkBuffer&)buffer; };

        // 
        inline operator const vk::Buffer& () const { return buffer; };
        inline operator const VkBuffer& () const { return (VkBuffer&)buffer; };

        // VMA HACK FOR EXTRACT DEVICE
        inline operator const vk::Device& () const;
        inline operator const VkDevice& () const;

        //
        inline vk::Device& getDevice();
        inline const vk::Device& getDevice() const;

        // Allocation
        inline operator const VmaAllocation& () const { return allocation; };
        inline operator const VmaAllocationInfo& () const { return allocationInfo; };

    public: // in-variant 
        vk::Buffer buffer = {};
        vk::DeviceSize range = 0ull;
        //vk::DeviceSize stride = 1ull;
        //
    protected: friend VmaBufferAllocation; // 
        VmaAllocation allocation = {};
        VmaAllocationInfo allocationInfo = {};
        VmaAllocator allocator = {};
    };

    // 
    class VmaImageAllocation : public std::enable_shared_from_this<VmaImageAllocation> { public:
        //inline ~VmaImageAllocation() { vmaDestroyImage(allocator, *this, allocation); };
         inline VmaImageAllocation() {};
         inline VmaImageAllocation(
            const VmaAllocator& allocator,
            const vkh::VkImageCreateInfo& createInfo = {},
            VmaMemoryUsage vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY
        ) {
            VmaAllocationCreateInfo vmaInfo = {}; vmaInfo.usage = vmaUsage;
            if (vmaUsage == VMA_MEMORY_USAGE_CPU_TO_GPU || vmaUsage == VMA_MEMORY_USAGE_GPU_TO_CPU) { vmaInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; };
            assert(vmaCreateImage(this->allocator = allocator, createInfo, &vmaInfo, &(VkImage&)image, &allocation, &allocationInfo) == VK_SUCCESS);
        };
         inline VmaImageAllocation(const VmaImageAllocation& allocation) : image(allocation.image), allocation(allocation.allocation), allocationInfo(allocation.allocationInfo), allocator(allocation.allocator) {};
         inline VmaImageAllocation& operator=(const VmaImageAllocation& allocation) {
            vmaDestroyImage(allocator, *this, allocation); // don't assign into already allocated
            this->image = allocation.image;
            this->allocation = allocation.allocation;
            this->allocationInfo = allocation.allocationInfo;
            this->allocator = allocation.allocator;
            return *this;
        };

        // Get mapped memory
        inline void* map() { void* ptr = nullptr; vmaMapMemory(allocator, allocation, &ptr); return ptr; };
        inline void* mapped() { if (!allocationInfo.pMappedData) { vmaMapMemory(allocator, allocation, &allocationInfo.pMappedData); }; return allocationInfo.pMappedData; };
        inline void unmap() { vmaUnmapMemory(allocator, allocation); allocationInfo.pMappedData = nullptr; };

        // VMA HACK FOR EXTRACT DEVICE
        inline operator const vk::Image& () const { return image; };
        inline operator const vk::Device& () const;

        // 
        inline operator const VkImage& () const { return (VkImage&)image; };
        inline operator const VkDevice& () const;

        // 
        inline operator vk::Image& () { return image; };
        inline operator vk::Device& ();

        // 
        inline operator VkImage& () { return (VkImage&)image; };
        inline operator VkDevice& ();

        //
        inline vk::Device& getDevice();
        inline const vk::Device& getDevice() const;

        // Allocation
        inline operator const VmaAllocation& () const { return allocation; };
        inline operator const VmaAllocationInfo& () const { return allocationInfo; };

        // 
        inline operator VmaAllocation& () { return allocation; };
        inline operator VmaAllocationInfo& () { return allocationInfo; };

    // 
    protected: friend VmaImageAllocation; // 
        vk::Image image = {};
        VmaAllocation allocation = {};
        VmaAllocationInfo allocationInfo = {};
        VmaAllocator allocator = {};
    };

#ifdef VMA_IMPLEMENTATION // Fix Implementation Issue
    inline vk::Device& VmaBufferAllocation::getDevice() { return (vk::Device&)(allocator->m_hDevice); };
    inline const vk::Device& VmaBufferAllocation::getDevice() const { return allocator->m_hDevice; };
    inline VmaBufferAllocation::operator const vk::Device& () const { return (vk::Device&)(allocator->m_hDevice); };
    inline VmaBufferAllocation::operator const VkDevice& () const { return allocator->m_hDevice; };
    inline vk::Device& VmaImageAllocation::getDevice() { return (vk::Device&)(allocator->m_hDevice); };
    inline const vk::Device& VmaImageAllocation::getDevice() const { return allocator->m_hDevice; };
    inline VmaImageAllocation::operator const vk::Device& () const { return (vk::Device&)(allocator->m_hDevice); };
    inline VmaImageAllocation::operator const VkDevice& () const { return allocator->m_hDevice; };
    inline VmaImageAllocation::operator vk::Device& () { return (vk::Device&)(allocator->m_hDevice); };
    inline VmaImageAllocation::operator VkDevice& () { return allocator->m_hDevice; };
#endif

    // 
    class ImageRegion : public std::enable_shared_from_this<ImageRegion> { public: 
        inline ImageRegion(){};
        inline ImageRegion(const std::shared_ptr<VmaImageAllocation>& allocation, const vkh::VkImageViewCreateInfo& info = {}, const vk::ImageLayout& layout = vk::ImageLayout::eGeneral) {
            this->allocation = allocation;
            this->imgInfo.imageView = ((vk::Device&)(*allocation)).createImageView(vk::ImageViewCreateInfo(info).setImage(*allocation));
            this->imgInfo.imageLayout = VkImageLayout(layout);
            this->subresourceRange = info.subresourceRange;
        };
        inline ImageRegion(const ImageRegion& region) {
            this->allocation = region; 
            this->subresourceRange = (vk::ImageSubresourceRange&)region;
            this->imgInfo = (vk::DescriptorImageInfo&)(region); 
        };
        inline ImageRegion& operator=(const ImageRegion& region){
            this->allocation = region.allocation;
            this->subresourceRange = region.subresourceRange;
            this->imgInfo = region.imgInfo;
            return *this;
        };

        //inline vkh::VkImageSubresourceRange& subresourceRange() { return this->subresourceRange; };
        inline vkh::VkImageSubresourceLayers subresourceLayers(const uint32_t mipLevel =  0u) const { return {
            .aspectMask = this->subresourceRange.aspectMask,
            .mipLevel = this->subresourceRange.baseMipLevel + mipLevel,
            .baseArrayLayer = this->subresourceRange.baseArrayLayer,
            .layerCount = this->subresourceRange.layerCount
        };};

        // set methods for direct control
        inline ImageRegion& setImageLayout(const VkImageLayout layout = {}) { imgInfo.imageLayout = layout; return *this; };
        inline ImageRegion& setSampler(const VkSampler& sampler = {}) { imgInfo.sampler = sampler; return *this; };
        inline ImageRegion& setImageLayout(const vk::ImageLayout layout = {}) { imgInfo.imageLayout = (const VkImageLayout&)layout; return *this; };
        inline ImageRegion& setSampler(const vk::Sampler& sampler = {}) { imgInfo.sampler = (const VkSampler&)sampler; return *this; };

        // 
        inline operator std::shared_ptr<VmaImageAllocation>&() { return this->allocation; };
        inline operator vkh::VkImageSubresourceRange&() { return this->subresourceRange; };
        inline operator vkh::VkDescriptorImageInfo&() { return this->imgInfo; };
        inline operator vk::DescriptorImageInfo&() { return this->imgInfo; };
        inline operator vk::ImageSubresourceRange&() { return this->subresourceRange; };
        inline operator vk::ImageView&() { return reinterpret_cast<vk::ImageView&>(this->imgInfo.imageView); };
        inline operator vk::ImageLayout&() { return reinterpret_cast<vk::ImageLayout&>(this->imgInfo.imageLayout); };
        inline operator vk::Image&() { return *allocation; };
        inline operator vk::Sampler&() { return (vk::Sampler&)imgInfo.sampler; };
        inline operator vk::Device&() { return *allocation; };
        inline operator ::VkDescriptorImageInfo&() { return this->imgInfo; };
        inline operator ::VkImageSubresourceRange&() { return this->subresourceRange; };
        inline operator VkImageView&() { return reinterpret_cast<VkImageView&>(this->imgInfo.imageView); };
        inline operator VkImageLayout&() { return reinterpret_cast<VkImageLayout&>(this->imgInfo.imageLayout); };
        inline operator VkImage&() { return *allocation; };
        inline operator VkSampler&() { return (VkSampler&)imgInfo.sampler; };
        inline operator VkDevice&() { return *allocation; };

        // 
        inline operator const std::shared_ptr<VmaImageAllocation>&() const { return this->allocation; };
        inline operator const vkh::VkImageSubresourceRange&() const { return this->subresourceRange; };
        inline operator const vkh::VkDescriptorImageInfo&() const { return this->imgInfo; };
        inline operator const vk::DescriptorImageInfo&() const { return this->imgInfo; };
        inline operator const vk::ImageSubresourceRange&() const { return this->subresourceRange; };
        inline operator const vk::ImageView&() const { return reinterpret_cast<const vk::ImageView&>(this->imgInfo.imageView); };
        inline operator const vk::ImageLayout&() const { return reinterpret_cast<const vk::ImageLayout&>(this->imgInfo.imageLayout); };
        inline operator const vk::Image&() const { return *allocation; };
        inline operator const vk::Sampler&() const { return (vk::Sampler&)imgInfo.sampler; };
        inline operator const vk::Device&() const { return *allocation; };
        inline operator const ::VkDescriptorImageInfo&() const { return this->imgInfo; };
        inline operator const ::VkImageSubresourceRange&() const { return this->subresourceRange; };
        inline operator const VkImageView&() const { return reinterpret_cast<const VkImageView&>(this->imgInfo.imageView); };
        inline operator const VkImageLayout&() const { return reinterpret_cast<const VkImageLayout&>(this->imgInfo.imageLayout); };
        inline operator const VkImage&() const { return *allocation; };
        inline operator const VkSampler&() const { return (VkSampler&)imgInfo.sampler; };
        inline operator const VkDevice&() const { return *allocation; };
        inline operator const vk::ImageSubresourceLayers() const { return vk::ImageSubresourceLayers{ (vk::ImageAspectFlags&)subresourceRange.aspectMask, subresourceRange.baseMipLevel, subresourceRange.baseArrayLayer, subresourceRange.layerCount }; };

        // 
        inline VmaImageAllocation* operator->() { return &(*allocation); };
        inline VmaImageAllocation& operator*() { return (*allocation); };
        inline const VmaImageAllocation* operator->() const { return &(*allocation); };
        inline const VmaImageAllocation& operator*() const { return (*allocation); };

    protected: friend VmaImageAllocation; // 
        vkh::VkDescriptorImageInfo imgInfo = {};
        std::shared_ptr<VmaImageAllocation> allocation = {};

    public: // irrevalent sources
        vkh::VkImageSubresourceRange subresourceRange = {};
    };

    // Wrapper Class
    template<class T = uint8_t>
    class Vector { // 
    public:
        inline Vector() {};
        inline Vector(const std::shared_ptr<VmaBufferAllocation>& allocation, vk::DeviceSize offset = 0u, vk::DeviceSize size = VK_WHOLE_SIZE) : allocation(allocation), bufInfo({ allocation->buffer,offset,size }), stride(sizeof(T)) {};

        // 
        template<class Tm = T> inline Vector(const Vector<Tm>& V) : allocation(V), bufInfo({ V.buffer(), V.offset(), V.range() }), stride(sizeof(T)) {};
        template<class Tm = T> inline Vector<T>& operator=(const Vector<Tm>& V) { this->allocation = V, this->bufInfo = vk::DescriptorBufferInfo(V.buffer(), V.offset(), V.range()), this->stride = sizeof(T); return *this; };

        // 
        inline void unmap() { allocation->unmap(); };
        inline const T* map() const { return (T*)((uint8_t*)allocation->map()+offset()); };
        inline T* const map() { return (T*)((uint8_t*)allocation->map()+offset()); };

        // 
        inline const T* mapped(const uintptr_t& i = 0u) const { return &((T*)((uint8_t*)allocation->mapped()+offset()))[i]; };
        inline T* const mapped(const uintptr_t& i = 0u) { return &((T*)((uint8_t*)allocation->mapped()+offset()))[i]; };

        // 
        inline T* const data() { return mapped(); };
        inline const T* data() const { return mapped(); };

        // 
        inline vk::BufferView& createBufferView(const vk::Format& format = vk::Format::eUndefined) {
            vkh::VkBufferViewCreateInfo info = {};
            info.buffer = bufInfo.buffer;
            info.offset = bufInfo.offset;
            info.range = bufInfo.range;
            info.format = VkFormat(format); // TODO: AUTO-FORMAT
            return (view = allocation->getDevice().createBufferView(info));
        };

        // at function 
        inline const T& at(const uintptr_t& i = 0u) const { return *mapped(i); };
        inline T& at(const uintptr_t& i = 0u) { return *mapped(i); };

        // array operator 
        inline const T& operator [] (const uintptr_t& i) const { return at(i); };
        inline T& operator [] (const uintptr_t& i) { return at(i); };

        // begin ptr
        inline const T* begin() const { return data(); };
        inline T* const begin() { return data(); };

        // end ptr
        inline const T* end() const { return &at(size() - 1ul); };
        inline T* const end() { return &at(size() - 1ul); };

        // 
        inline operator std::shared_ptr<VmaBufferAllocation>& () { return allocation; };
        inline operator vkh::VkDescriptorBufferInfo& () { bufInfo.buffer = (vk::Buffer&)(*allocation); return reinterpret_cast<vkh::VkDescriptorBufferInfo&>(bufInfo); };
        inline operator vk::DescriptorBufferInfo& () { bufInfo.buffer = (vk::Buffer&)(*allocation); return bufInfo; };
        inline operator vk::Buffer& () { return (vk::Buffer&)(bufInfo.buffer = (vk::Buffer&)*allocation); };
        inline operator vk::Device& () { return *allocation; };
        inline operator vk::BufferView& () { return view; };
        inline operator ::VkDescriptorBufferInfo& () { bufInfo.buffer = (vk::Buffer&)(*allocation); return reinterpret_cast<::VkDescriptorBufferInfo&>(bufInfo); };
        inline operator VkBuffer& () { return (VkBuffer&)(bufInfo.buffer = (vk::Buffer&) * allocation); };
        inline operator VkDevice& () { return *allocation; };
        inline operator VkBufferView& () { return view; };

        // 
        inline operator const std::shared_ptr<VmaBufferAllocation>& () const { return allocation; };
        inline operator const vkh::VkDescriptorBufferInfo& () const { return reinterpret_cast<const vkh::VkDescriptorBufferInfo&>(bufInfo); };
        inline operator const vk::DescriptorBufferInfo& () const { return bufInfo; };
        inline operator const vk::Buffer& () const { return *allocation; };
        inline operator const vk::Device& () const { return *allocation; };
        inline operator const vk::BufferView& () const { return view; };
        inline operator const ::VkDescriptorBufferInfo& () const { return reinterpret_cast<const ::VkDescriptorBufferInfo&>(bufInfo); };
        inline operator const VkBuffer&() const { return *allocation; };
        inline operator const VkDevice&() const { return *allocation; };
        inline operator const VkBufferView& () const { return reinterpret_cast<const VkBufferView&>(view); };

        // 
        inline vk::DeviceSize& offset() { return this->bufInfo.offset; };
        //inline vk::DeviceSize& stride() { return this->stride; };

        // 
        inline const vk::DeviceSize& offset() const { return this->bufInfo.offset; };
        //inline const vk::DeviceSize& stride() const { return this->stride; };

        // 
        inline vk::DeviceSize range() const { return (this->bufInfo.range != VK_WHOLE_SIZE ? std::min(this->bufInfo.range, this->allocation->range - this->bufInfo.offset) : (this->allocation->range - this->bufInfo.offset)); };
        inline vk::DeviceSize size() const { return this->range() / this->stride; };

        // 
        inline vk::Buffer& buffer() { return reinterpret_cast<vk::Buffer&>(allocation->buffer); };
        //VkBuffer& buffer() { return reinterpret_cast<VkBuffer&>(allocation->buffer); };

        // 
        inline const vk::Buffer& buffer() const { return reinterpret_cast<vk::Buffer&>(allocation->buffer); };
        //const VkBuffer& buffer() const { return reinterpret_cast<VkBuffer&>(allocation->buffer); };

        // typed casting 
        template<class Tm = T> inline Vector<Tm>& cast() { return reinterpret_cast<Vector<Tm>&>(*this); };
        template<class Tm = T> inline const Vector<Tm>& cast() const { return reinterpret_cast<const Vector<Tm>&>(*this); };

        // 
        inline bool has() const { return allocation ? true : false; };

        // 
        inline VmaBufferAllocation* operator->() { return &(*allocation); };
        inline VmaBufferAllocation& operator*() { return (*allocation); };
        inline const VmaBufferAllocation* operator->() const { return &(*allocation); };
        inline const VmaBufferAllocation& operator*() const { return (*allocation); };

        //
        protected: friend Vector<T>; // 
        protected: vk::DescriptorBufferInfo bufInfo = {};
        public   : vk::DeviceSize stride = sizeof(T);
        protected: vk::BufferView view = {};
        protected: std::shared_ptr<VmaBufferAllocation> allocation = {};
    };
};
