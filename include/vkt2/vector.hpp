#include "utils.hpp"
#include "structs.hpp"

namespace vkt {

    // 
    class VmaBufferAllocation : public std::enable_shared_from_this<VmaBufferAllocation> { public:
        //~VmaBufferAllocation() { vmaDestroyBuffer(allocator, *this, allocation); };
         VmaBufferAllocation() {};
         VmaBufferAllocation(
            const VmaAllocator& allocator,
            const vkh::VkBufferCreateInfo& createInfo = {},
            VmaMemoryUsage vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY
        ) {
            VmaAllocationCreateInfo vmaInfo = {}; vmaInfo.usage = vmaUsage;
            if (vmaUsage == VMA_MEMORY_USAGE_CPU_TO_GPU || vmaUsage == VMA_MEMORY_USAGE_GPU_TO_CPU) { vmaInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; };
            assert(vmaCreateBuffer(this->allocator = allocator, createInfo, &vmaInfo, &(VkBuffer&)buffer, &allocation, &allocationInfo) == VK_SUCCESS);
        };
        VmaBufferAllocation(const VmaBufferAllocation& allocation) : buffer(allocation.buffer), allocation(allocation.allocation), allocationInfo(allocation.allocationInfo), allocator(allocation.allocator) {};
        VmaBufferAllocation& operator=(const VmaBufferAllocation& allocation) {
            vmaDestroyBuffer(allocator, *this, allocation); // don't assign into already allocated
            this->buffer = allocation.buffer;
            this->allocation = allocation.allocation;
            this->allocationInfo = allocation.allocationInfo;
            this->allocator = allocation.allocator;
            return *this;
        };

        // Get mapped memory
        void* map() { void* ptr = nullptr; vmaMapMemory(allocator, allocation, &ptr); return ptr; };
        void* mapped() { if (!allocationInfo.pMappedData) { vmaMapMemory(allocator, allocation, &allocationInfo.pMappedData); }; return allocationInfo.pMappedData; };
        void unmap() { vmaUnmapMemory(allocator, allocation); allocationInfo.pMappedData = nullptr; };

        // 
        operator vk::Buffer& () { return buffer; };
        operator VkBuffer& () { return (VkBuffer&)buffer; };

        // 
        operator const vk::Buffer& () const { return buffer; };
        operator const VkBuffer& () const { return (VkBuffer&)buffer; };

        // VMA HACK FOR EXTRACT DEVICE
        operator const vk::Device& () const { return (vk::Device&)(allocator->m_hDevice); };
        operator const VkDevice& () const { return allocator->m_hDevice; };

        // Allocation
        operator const VmaAllocation& () const { return allocation; };
        operator const VmaAllocationInfo& () const { return allocationInfo; };

        // 
        vk::DeviceSize& range() { return allocationInfo.size; };
        const vk::DeviceSize& range() const { return allocationInfo.size; };

    // 
    protected: friend VmaBufferAllocation; // 
        vk::Buffer buffer = {};
        VmaAllocation allocation = {};
        VmaAllocationInfo allocationInfo = {};
        VmaAllocator allocator = {};
    };

    // 
    class VmaImageAllocation : public std::enable_shared_from_this<VmaImageAllocation> { public:
        //~VmaImageAllocation() { vmaDestroyImage(allocator, *this, allocation); };
         VmaImageAllocation() {};
         VmaImageAllocation(
            const VmaAllocator& allocator,
            const vkh::VkImageCreateInfo& createInfo = {},
            VmaMemoryUsage vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY
        ) {
            VmaAllocationCreateInfo vmaInfo = {}; vmaInfo.usage = vmaUsage;
            if (vmaUsage == VMA_MEMORY_USAGE_CPU_TO_GPU || vmaUsage == VMA_MEMORY_USAGE_GPU_TO_CPU) { vmaInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; };
            assert(vmaCreateImage(this->allocator = allocator, createInfo, &vmaInfo, &(VkImage&)image, &allocation, &allocationInfo) == VK_SUCCESS);
        };
        VmaImageAllocation(const VmaImageAllocation& allocation) : image(allocation.image), allocation(allocation.allocation), allocationInfo(allocation.allocationInfo), allocator(allocation.allocator) {};
        VmaImageAllocation& operator=(const VmaImageAllocation& allocation) {
            vmaDestroyImage(allocator, *this, allocation); // don't assign into already allocated
            this->image = allocation.image;
            this->allocation = allocation.allocation;
            this->allocationInfo = allocation.allocationInfo;
            this->allocator = allocation.allocator;
            return *this;
        };

        // Get mapped memory
        void* map() { void* ptr = nullptr; vmaMapMemory(allocator, allocation, &ptr); return ptr; };
        void* mapped() { if (!allocationInfo.pMappedData) { vmaMapMemory(allocator, allocation, &allocationInfo.pMappedData); }; return allocationInfo.pMappedData; };
        void unmap() { vmaUnmapMemory(allocator, allocation); allocationInfo.pMappedData = nullptr; };

        // 
        operator const vk::Image& () const { return image; };
        operator const VkImage& () const { return (VkImage&)image; };

        // VMA HACK FOR EXTRACT DEVICE
        operator const vk::Device& () const { return(vk::Device&)(allocator->m_hDevice); };
        operator const VkDevice& () const { return allocator->m_hDevice; };

        // Allocation
        operator const VmaAllocation& () const { return allocation; };
        operator const VmaAllocationInfo& () const { return allocationInfo; };

    // 
    protected: friend VmaImageAllocation; // 
        vk::Image image = {};
        VmaAllocation allocation = {};
        VmaAllocationInfo allocationInfo = {};
        VmaAllocator allocator = {};
    };

    // 
    template<class T = uint8_t>
    class BufferRegion : public std::enable_shared_from_this<BufferRegion<T>> {
    public: //using T = uint32_t;
        
        BufferRegion(){};
        BufferRegion(const std::shared_ptr<VmaBufferAllocation>& allocation, vk::DeviceSize offset = 0u, vk::DeviceSize size = VK_WHOLE_SIZE) : allocation(allocation) {
            bufInfo.buffer = (const vk::Buffer&)(*allocation);
            bufInfo.offset = offset;
            bufInfo.range = size * sizeof(T);
        };

        // re-assign buffer region (with another)
        template<class Tm = T> BufferRegion(const std::shared_ptr<BufferRegion<Tm>>& region = {}) : allocation(*region), bufInfo({*region,region->offset(),region->range()}) {};
        template<class Tm = T> BufferRegion<T>& operator=(const std::shared_ptr<BufferRegion<Tm>>& region) { this->allocation = *region; this->bufInfo = *region; return *this; };
        template<class Tm = T> BufferRegion<T>& operator=(const BufferRegion<Tm>& region) { this->allocation = *region; this->bufInfo = *region; return *this; };

        // 
        void unmap() { allocation->unmap(); };
        const T* map() const { return (T*)((uint8_t*)allocation->map()+offset()); };
        T* const map() { return (T*)((uint8_t*)allocation->map()+offset()); };

        // 
        const T* mapped(const uintptr_t& i = 0u) const { return &((T*)((uint8_t*)allocation->mapped()+offset()))[i]; };
        T* const mapped(const uintptr_t& i = 0u) { return &((T*)((uint8_t*)allocation->mapped()+offset()))[i]; };

        // 
        T* const data() { return mapped(); };
        const T* data() const { return mapped(); };

        // return corrected size
        vk::DeviceSize size() const {
            return (bufInfo.range != VK_WHOLE_SIZE ? std::min(bufInfo.range, allocation->range() - bufInfo.offset) : (allocation->range() - bufInfo.offset)) / sizeof(T);
        };

        // at function 
        const T& at(const uintptr_t& i = 0u) const { return *mapped(i); };
        T& at(const uintptr_t& i = 0u) { return *mapped(i); };

        // array operator 
        const T& operator [] (const uintptr_t& i) const { return at(i); };
        T& operator [] (const uintptr_t& i) { return at(i); };

        // begin ptr
        const T* begin() const { return data(); };
        T* const begin() { return data(); };

        // end ptr
        const T* end() const { return &at(size() - 1ul); };
        T* const end() { return &at(size() - 1ul); };

        // 
        operator std::shared_ptr<VmaBufferAllocation>& () { return allocation; };
        operator vkh::VkDescriptorBufferInfo& () { bufInfo.buffer = (vk::Buffer&)(*allocation); return bufInfo; };
        operator vk::DescriptorBufferInfo& () { bufInfo.buffer = (vk::Buffer&)(*allocation); return bufInfo; };
        operator vk::Buffer& () { return (bufInfo.buffer = (vk::Buffer&)*allocation); };
        operator vk::Device& () { return *allocation; };

        // 
        operator VkDescriptorBufferInfo& () { bufInfo.buffer = (vk::Buffer&)(*allocation); return reinterpret_cast<VkDescriptorBufferInfo&>(bufInfo); };
        operator VkBuffer& () { return (VkBuffer&)(bufInfo.buffer = (vk::Buffer&) * allocation); };
        operator VkDevice& () { return *allocation; };

        // 
        operator const std::shared_ptr<VmaBufferAllocation>& () const { return allocation; };
        operator const vkh::VkDescriptorBufferInfo& () const { return bufInfo; };
        operator const vk::DescriptorBufferInfo& () const { return bufInfo; };
        operator const vk::Buffer& () const { return *allocation; };
        operator const vk::Device& () const { return *allocation; };

        // 
        operator const VkDescriptorBufferInfo& () const { return reinterpret_cast<const VkDescriptorBufferInfo&>(bufInfo); };
        operator const VkBuffer&() const { return *allocation; };
        operator const VkDevice&() const { return *allocation; };

        // 
        const vk::DeviceSize& offset() const { return bufInfo.offset; };
        const vk::DeviceSize& range() const { return bufInfo.range; };

        // 
        bool has() const { return allocation ? true : false; };

    // 
    protected: friend BufferRegion<T>; // 
        vkh::VkDescriptorBufferInfo bufInfo = {};
        std::shared_ptr<VmaBufferAllocation> allocation = {};
    };

    // 
    template<class T = uint8_t>
    class Vector { // direct wrapper for indirect pointer `std::shared_ptr<BufferRegion<T>>`
    public:
        Vector() {};
        Vector(const std::shared_ptr<VmaBufferAllocation>& allocation, vk::DeviceSize offset = 0u, vk::DeviceSize size = VK_WHOLE_SIZE) { region = std::make_shared<BufferRegion<T>>(allocation, offset, size); };
        Vector(const std::shared_ptr<BufferRegion<T>>& region) : region(region) {};
        //Vector(const Vector<T>& vector) : region(vector.region) {};

        // 
        template<class Tm = T> Vector(const Vector<Tm>& vector) : region(vector.region) {};
        template<class Tm = T> Vector<T>& operator=(const Vector<Tm>& V) { this->region = V.cast<T>(); return *this; };
        template<class Tm = T> Vector<T>& operator=(const std::shared_ptr<BufferRegion<Tm>>& region) { this->region = std::static_pointer_cast<BufferRegion<T>>(region); return *this; };

        // map through
        T* const map() { return region->map(); };
        void unmap() { return region->unmap(); };

        // 
        T* const data() { return region->data(); };
        const T* data() const { return region->data(); };
        vk::DeviceSize size() const { return region->size(); };

        // at function 
        const T& at(const uintptr_t& i) const { return region->at(i); };
        T& at(const uintptr_t& i) { return region->at(i); };

        // array operator 
        const T& operator [] (const uintptr_t& i) const { at(i); };
        T& operator [] (const uintptr_t& i) { return at(i); };

        // begin ptr
        const T* begin() const { region->begin(); };
        T* const begin() { return region->begin(); };

        // end ptr
        const T* end() const { return region->end(); };
        T* const end() { return region->end(); };

        // 
        operator vk::DescriptorBufferInfo& () { return *region; };
        operator vk::Buffer& () { return *region; };
        operator vk::Device& () { return *region; };
        operator vk::BufferView& () const { return view; };

        // 
        operator const vk::DescriptorBufferInfo& () const { return *region; };
        operator const vk::Buffer& () const { return *region; };
        operator const vk::Device& () const { return *region; };
        operator const vk::BufferView& () const { return view; };

        // 
        operator vkh::VkDescriptorBufferInfo& () { return *region; };
        operator const vkh::VkDescriptorBufferInfo& () const { return *region; };

        // 
        operator VkDescriptorBufferInfo& () { return *region; };
        operator VkBuffer& () { return *region; };
        operator VkDevice& () { return *region; };
        operator VkBufferView& () const { return reinterpret_cast<VkBufferView&>(view); };

        // 
        operator const VkDescriptorBufferInfo& () const { return *region; };
        operator const VkBuffer& () const { return *region; };
        operator const VkDevice& () const { return *region; };
        operator const VkBufferView& () const { return reinterpret_cast<const VkBufferView&>(view); };

        // 
        operator std::shared_ptr<BufferRegion<T>>&() { return region; };
        operator const std::shared_ptr<BufferRegion<T>>&() const { return region; };

        // 
        const vk::DeviceSize& range() const { return region->range(); };
        const vk::DeviceSize& offset() const { return region->offset(); };

        // 
        BufferRegion<T>* operator->() { return &(*region); };
        BufferRegion<T>& operator*() { return *region; };

        // 
        const BufferRegion<T>* operator->() const { return &(*region); };
        const BufferRegion<T>& operator*() const { return *region; };

        // typed casting 
        template<class Tm = T> Vector<Tm>& cast() { return reinterpret_cast<Vector<Tm>&>(*this); };
        template<class Tm = T> const Vector<Tm>& cast() const { return reinterpret_cast<const Vector<Tm>&>(*this); };

        // 
        bool has() const { return region ? true : false; };

        // 
        vk::BufferView& createBufferView(const vk::Format& format = vk::Format::eUndefined) {
            vkh::VkBufferViewCreateInfo info = {};
            info.buffer = *region;
            info.offset = region->offset();
            info.range = region->range();
            info.format = format; // TODO: AUTO-FORMAT
            return (view = (const vk::Device&)(*region).createBufferView(info));
        };

    protected: friend Vector<T>; // 
        std::shared_ptr<BufferRegion<T>> region = {};
        vk::BufferView view = {};
    };

};
