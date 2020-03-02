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

    struct MemoryAllocationInfo { // 
        uint32_t glMemory = 0u, glID = 0u;

        vk::Device device = {};
        vk::DeviceMemory memory = {};
        vk::DeviceSize range = 0ull;
        vk::DeviceSize reqSize = 0ull;
        vk::ImageLayout initialLayout = vk::ImageLayout::eUndefined;
        vk::DispatchLoaderDynamic dispatch = {};

        HANDLE handle = {};
        void* pMapped = nullptr;
        vkh::VkPhysicalDeviceMemoryProperties memoryProperties = {};
        VmaMemoryUsage vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY;

        // 
        int32_t getMemoryType(const uint32_t& memoryTypeBitsRequirement, const vkh::VkMemoryPropertyFlags& requiredProperties = { .eDeviceLocal = 1 }) const {
            const uint32_t memoryCount = memoryProperties.memoryTypeCount;
            for (uint32_t memoryIndex = 0; memoryIndex < memoryCount; ++memoryIndex) {
                const uint32_t memoryTypeBits = (1 << memoryIndex);
                const bool isRequiredMemoryType = memoryTypeBitsRequirement & memoryTypeBits;
                const auto properties = VkMemoryPropertyFlags(memoryProperties.memoryTypes[memoryIndex].propertyFlags);
                const bool hasRequiredProperties = (properties & VkMemoryPropertyFlags(requiredProperties)) == VkMemoryPropertyFlags(requiredProperties);
                if (isRequiredMemoryType && hasRequiredProperties) return static_cast<int32_t>(memoryIndex);
            }
            return -1;
        }
    };

    // 
    class VmaBufferAllocation;
    class BufferAllocation : public std::enable_shared_from_this<BufferAllocation> { public:
        BufferAllocation() {};
        BufferAllocation(const vkt::uni_arg<MemoryAllocationInfo>& allocationInfo, const vkt::uni_arg<vkh::VkBufferCreateInfo>& createInfo = {}) : info( allocationInfo) { this->construct( allocationInfo,  createInfo); };
        BufferAllocation(const vkt::uni_arg    <BufferAllocation>& allocation) : buffer(allocation->buffer), info(allocation->info) { *this = allocation; };

        virtual BufferAllocation* construct(
            const vkt::uni_arg<MemoryAllocationInfo>& allocationInfo,
            const vkt::uni_arg<vkh::VkBufferCreateInfo>& createInfo = {}
        ) {
            this->buffer = this->info.device.createBuffer(*createInfo);

            vk::MemoryRequirements memReqs = allocationInfo->device.getBufferMemoryRequirements(buffer);
            vk::MemoryAllocateInfo memAllocInfo = {};
            vk::ExportMemoryAllocateInfo exportAllocInfo{ vk::ExternalMemoryHandleTypeFlagBits::eOpaqueWin32 };
            memAllocInfo.pNext = &exportAllocInfo;
            memAllocInfo.allocationSize = memReqs.size;
            memAllocInfo.memoryTypeIndex = uint32_t(allocationInfo->getMemoryType(memReqs.memoryTypeBits, { .eDeviceLocal = 1 }));

            // 
            this->info.device.bindBufferMemory(buffer, info.memory = info.device.allocateMemory(memAllocInfo), 0);
            this->info.handle = info.device.getMemoryWin32HandleKHR({ info.memory, vk::ExternalMemoryHandleTypeFlagBits::eOpaqueWin32 }, this->info.dispatch);

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

        virtual BufferAllocation& operator=(const vkt::uni_arg<BufferAllocation>& allocation) {
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

    public: // in-variant 
        vk::Buffer buffer = {};
        MemoryAllocationInfo info = {};

    protected: friend BufferAllocation; friend VmaBufferAllocation;
    };

    // 
    class VmaBufferAllocation : public BufferAllocation { public: 
        VmaBufferAllocation() {};
        VmaBufferAllocation(const vkt::uni_arg<VmaAllocator>& allocator, const vkt::uni_arg<vkh::VkBufferCreateInfo>& createInfo = {}, VmaMemoryUsage vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY) { this->construct( allocator,  createInfo, vmaUsage); };
        VmaBufferAllocation(const vkt::uni_arg<VmaBufferAllocation>& allocation) : allocation(allocation->allocation), allocationInfo(allocation->allocationInfo), allocator(allocation->allocator) { *this = allocation; };
        VmaBufferAllocation(const vkt::uni_arg<BufferAllocation>& allocation) { *this = dynamic_cast<const VmaBufferAllocation&>(allocation); };

        //
        virtual VmaBufferAllocation* construct(
            const vkt::uni_arg<VmaAllocator>& allocator,
            const vkt::uni_arg<vkh::VkBufferCreateInfo>& createInfo = {},
            const vkt::uni_arg<VmaMemoryUsage>& vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY
        ) {
            VmaAllocationCreateInfo vmaInfo = {}; vmaInfo.usage = vmaUsage;
            if (vmaUsage == VMA_MEMORY_USAGE_CPU_TO_GPU || vmaUsage == VMA_MEMORY_USAGE_GPU_TO_CPU) { vmaInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; };
            auto result = vmaCreateBuffer(this->allocator = allocator, *createInfo, &vmaInfo, &reinterpret_cast<VkBuffer&>(buffer), &allocation, &allocationInfo);
            assert(result == VK_SUCCESS);
            this->info.range = createInfo->size;
            this->info.device = this->_getDevice();
            this->info.vmaUsage = vmaUsage;
            return this;
        };

        // 
        virtual VmaBufferAllocation& operator=(const vkt::uni_arg<VmaBufferAllocation>& allocation) {
            if (allocation->allocation) {
                vmaDestroyBuffer(allocator, *this, *allocation); // don't assign into already allocated
            };
            this->info = allocation->info;
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
    class ImageRegion;
    class VmaImageAllocation;
    class ImageAllocation : public std::enable_shared_from_this<ImageAllocation> { public: 
        ImageAllocation() {};
        ImageAllocation(const vkt::uni_arg<MemoryAllocationInfo>& allocationInfo, const vkt::uni_arg<vkh::VkImageCreateInfo>& createInfo = {}) : info( allocationInfo) { this->construct( allocationInfo,  createInfo); }
        ImageAllocation(const vkt::uni_arg<ImageAllocation>& allocation) : image(allocation->image), info(allocation->info) { *this = allocation; };


        virtual ImageAllocation* construct(
            const vkt::uni_arg<MemoryAllocationInfo>& allocationInfo,
            const vkt::uni_arg < vkh::VkImageCreateInfo>& createInfo = {}
        ) {
            vkh::VkImageUsageFlags usage = createInfo->usage;
            if (allocationInfo->vmaUsage == VMA_MEMORY_USAGE_CPU_TO_GPU) { usage.eTransferSrc = 1; };
            if (allocationInfo->vmaUsage == VMA_MEMORY_USAGE_GPU_TO_CPU) { usage.eTransferDst = 1; };
            if (allocationInfo->vmaUsage == VMA_MEMORY_USAGE_GPU_ONLY) { usage.eTransferDst = 1, usage.eTransferSrc = 1; };

            // 
            this->image = this->info.device.createImage(vk::ImageCreateInfo(*createInfo).setUsage(usage));

            // 
            vk::MemoryRequirements memReqs = allocationInfo->device.getImageMemoryRequirements(image);
            vk::MemoryAllocateInfo memAllocInfo = {};
            vk::ExportMemoryAllocateInfo exportAllocInfo{ vk::ExternalMemoryHandleTypeFlagBits::eOpaqueWin32 };
            memAllocInfo.pNext = &exportAllocInfo;
            memAllocInfo.allocationSize = memReqs.size;
            memAllocInfo.memoryTypeIndex = uint32_t(allocationInfo->getMemoryType(memReqs.memoryTypeBits, { .eDeviceLocal = 1 }));

            // 
            this->info.device.bindImageMemory(image, info.memory = info.device.allocateMemory(memAllocInfo), 0);
            this->info.initialLayout = vk::ImageLayout(createInfo->initialLayout);
            this->info.handle = info.device.getMemoryWin32HandleKHR({ info.memory, vk::ExternalMemoryHandleTypeFlagBits::eOpaqueWin32 }, this->info.dispatch);
            this->info.range = memReqs.size;
            this->info.reqSize = memReqs.size;

            // 
#ifdef ENABLE_OPENGL_INTEROP
            GLuint format = GL_RGBA8;
            if (createInfo.format == VK_FORMAT_R16G16B16A16_UNORM) { format = GL_RGBA16; };
            if (createInfo.format == VK_FORMAT_R32G32B32A32_SFLOAT) { format = GL_RGBA32F; };
            if (createInfo.format == VK_FORMAT_R16G16B16A16_SFLOAT) { format = GL_RGBA16F; };
            if (createInfo.format == VK_FORMAT_R32G32B32_SFLOAT) { format = GL_RGB32F; };
            if (createInfo.format == VK_FORMAT_R16G16B16_SFLOAT) { format = GL_RGB16F; };
            if (createInfo.format == VK_FORMAT_R32G32_SFLOAT) { format = GL_RG32F; };
            if (createInfo.format == VK_FORMAT_R16G16_SFLOAT) { format = GL_RG16F; };

            // Import Memory
            glCreateTextures(GL_TEXTURE_2D, 1, &this->info.glID);
            glCreateMemoryObjectsEXT(1, &this->info.glMemory);
            glImportMemoryWin32HandleEXT(this->info.glMemory, this->info.reqSize, GL_HANDLE_TYPE_OPAQUE_WIN32_EXT, this->info.handle);

            // Create GL Image
            if (createInfo.imageType == VK_IMAGE_TYPE_1D) {
                glTextureStorageMem1DEXT(this->info.glID, createInfo.mipLevels, format, createInfo.extent.width, this->info.glMemory, 0);
            }
            else
                if (createInfo.imageType == VK_IMAGE_TYPE_2D) {
                    glTextureStorageMem2DEXT(this->info.glID, createInfo.mipLevels, format, createInfo.extent.width, createInfo.extent.height, this->info.glMemory, 0);
                }
                else
                    if (createInfo.imageType == VK_IMAGE_TYPE_3D) {
                        glTextureStorageMem3DEXT(this->info.glID, createInfo.mipLevels, format, createInfo.extent.width, createInfo.extent.height, createInfo.extent.depth, this->info.glMemory, 0);
                    }
#endif
            return this;
        };

        virtual ImageAllocation& operator=(const vkt::uni_arg<ImageAllocation>& allocation) {
            this->image = allocation->image;
            this->info = allocation->info;
            return *this;
        };

        // 
        virtual operator const vk::Image& () const { return this->image; };
        virtual operator const VkImage& () const { return this->image; };

        // 
        virtual operator vk::Image& () { return this->image; };
        virtual operator VkImage& () { return reinterpret_cast<VkImage&>(this->image); };

        // 
        virtual operator const vk::Device& () const { return info.device; };
        virtual operator const VkDevice& () const { return reinterpret_cast<const VkDevice&>(info.device); };

        //
        virtual vk::Device& getDevice() { return info.device; };
        virtual const vk::Device& getDevice() const { return info.device; };

        // 
        virtual const vk::Device& _getDevice() const { return info.device; };

        // 
        virtual operator vk::Device& () { return info.device; };
        virtual operator VkDevice& () { return reinterpret_cast<VkDevice&>(info.device); };

        // Get mapped memory
        virtual void* map() { return (info.pMapped = info.device.mapMemory(info.memory, 0u, info.range, {})); };
        virtual void* mapped() { return info.pMapped; };
        virtual void  unmap() { info.device.unmapMemory(info.memory); };

        // 
        virtual MemoryAllocationInfo& getAllocationInfo() { return info; };
        virtual const MemoryAllocationInfo& getAllocationInfo() const { return info; };

    // 
    protected: friend VmaImageAllocation; friend ImageAllocation; friend ImageRegion;
        vk::Image image = {};
        MemoryAllocationInfo info = {};
    };

    // 
    class VmaImageAllocation : public ImageAllocation { public: 
        VmaImageAllocation() {};
        VmaImageAllocation(const vkt::uni_arg<VmaImageAllocation>& allocation) : allocation(allocation->allocation), allocationInfo(allocation->allocationInfo), allocator(allocation->allocator) { *this = allocation; };
        VmaImageAllocation(const vkt::uni_arg<VmaAllocator>& allocator, const vkt::uni_arg<vkh::VkImageCreateInfo>& createInfo = {}, const VmaMemoryUsage& vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY) { this->construct( allocator,  createInfo, vmaUsage); };

        // 
        virtual VmaImageAllocation* construct(
            const VmaAllocator& allocator,
            const vkh::VkImageCreateInfo& createInfo = {},
            const VmaMemoryUsage& vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY
        ) {
            VmaAllocationCreateInfo vmaInfo = {}; vmaInfo.usage = vmaUsage;
            if (vmaUsage == VMA_MEMORY_USAGE_CPU_TO_GPU || vmaUsage == VMA_MEMORY_USAGE_GPU_TO_CPU) { vmaInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; };
            auto result = vmaCreateImage(this->allocator = allocator, createInfo, &vmaInfo, &reinterpret_cast<VkImage&>(image), &allocation, &allocationInfo);
            assert(result == VK_SUCCESS);
            this->info.device = this->_getDevice();
            this->info.initialLayout = vk::ImageLayout(createInfo.initialLayout);
            this->info.range = allocationInfo.size;
            this->info.vmaUsage = vmaUsage;
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

        // Get mapped memory
        virtual void* map() override { void* ptr = nullptr; vmaMapMemory(allocator, allocation, &ptr); return ptr; };
        virtual void* mapped() override { if (!allocationInfo.pMappedData) { vmaMapMemory(allocator, allocation, &allocationInfo.pMappedData); }; return allocationInfo.pMappedData; };
        virtual void  unmap() override { vmaUnmapMemory(allocator, allocation); allocationInfo.pMappedData = nullptr; };

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
    struct ImageRegionCreateInfoAllocated {
        const MemoryAllocationInfo& allocationInfo = {};
        const vkh::VkImageCreateInfo& createInfo = {};
        const vkh::VkImageViewCreateInfo& info = {};
        const vk::ImageLayout& layout = vk::ImageLayout::eGeneral;
    };

    // 
    struct ImageRegionCreateInfoVMA {
        const VmaAllocator& allocator = {};
        const vkh::VkImageCreateInfo& createInfo = {};
        const VmaMemoryUsage& vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY;
        const vkh::VkImageViewCreateInfo& info = {};
        const vk::ImageLayout& layout = vk::ImageLayout::eGeneral;
    };

    // 
    class ImageRegion : public std::enable_shared_from_this<ImageRegion> { public: 
        ImageRegion() {};
        ImageRegion(const vkt::uni_arg<ImageRegion>& region) { *this = region; };
        ImageRegion(const std::shared_ptr<ImageAllocation>& allocation, const vkt::uni_arg<vkh::VkImageViewCreateInfo>& info = {}, const vkt::uni_arg<vk::ImageLayout>& layout = vk::ImageLayout::eGeneral) : allocation(allocation), subresourceRange(info->subresourceRange) { this->construct(allocation, info, layout); };
        ImageRegion(const vkt::uni_arg<MemoryAllocationInfo>& allocationInfo, const vkt::uni_arg<vkh::VkImageCreateInfo>& createInfo = {}, const vkt::uni_arg<vkh::VkImageViewCreateInfo>& info = {}, const vkt::uni_arg<vk::ImageLayout>& layout = vk::ImageLayout::eGeneral) { this->construct(std::make_shared<ImageAllocation>(allocationInfo, createInfo), info, layout); };
        ImageRegion(const vkt::uni_arg<VmaAllocator>& allocator, const vkt::uni_arg<vkh::VkImageCreateInfo>& createInfo = {}, const vkt::uni_arg<VmaMemoryUsage>& vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY, const vkt::uni_arg<vkh::VkImageViewCreateInfo>& info = {}, const vkt::uni_arg<vk::ImageLayout>& layout = vk::ImageLayout::eGeneral) { this->construct(vkt::uni_ptr<ImageAllocation>(std::make_shared<VmaImageAllocation>(allocator, createInfo, vmaUsage)), info, layout); };
        ~ImageRegion() {};

        // 
        virtual ImageRegion* construct(
            const vkt::uni_ptr<ImageAllocation>& allocation,
            const vkt::uni_arg<vkh::VkImageViewCreateInfo>& info = {},
            const vkt::uni_arg<vk::ImageLayout>& layout = vk::ImageLayout::eGeneral
        ) {
            this->allocation = allocation;
            this->imgInfo.imageView = this->allocation->getDevice().createImageView(vk::ImageViewCreateInfo(*info).setImage(*this->allocation));
            this->imgInfo.imageLayout = VkImageLayout(*layout);
            return this;
        };

        // 
        virtual ImageRegion& operator=(const vkt::uni_arg<ImageRegion>& region) {
            this->allocation = region->uniPtr();
            this->subresourceRange = region->subresourceRange;
            this->imgInfo = region->imgInfo;
            return *this;
        };

        // 
        virtual vkt::uni_ptr<ImageAllocation>& uniPtr() { return this->allocation; };
        virtual const vkt::uni_ptr<ImageAllocation>& uniPtr() const { return this->allocation; };

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
        virtual ImageRegion& transfer(vk::CommandBuffer& cmdBuf) {
            vkt::imageBarrier(cmdBuf, { 
                .image = *this->allocation, 
                .targetLayout = vk::ImageLayout(this->imgInfo.imageLayout), 
                .originLayout = vk::ImageLayout(this->allocation->info.initialLayout), 
                .subresourceRange = *this 
            });
            return *this;
        };

        // 
        virtual operator ImageAllocation*() { return this->allocation; };
        virtual operator vkt::uni_ptr<ImageAllocation>&() { return this->allocation; };
        virtual operator std::shared_ptr<ImageAllocation>& () { return this->allocation; };
        virtual operator vkh::VkImageSubresourceRange&() { return this->subresourceRange; };
        virtual operator vkt::MemoryAllocationInfo&() { return this->allocation->info; };
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
        virtual operator const ImageAllocation*() const { return this->allocation; };
        virtual operator const vkt::uni_ptr<ImageAllocation>&() const { return this->allocation; };
        virtual operator const std::shared_ptr<ImageAllocation>& () const { return this->allocation; };
        virtual operator const vkh::VkImageSubresourceRange&() const { return this->subresourceRange; };
        virtual operator const vkt::MemoryAllocationInfo&() const { return this->allocation->info; };
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
        virtual operator const VkDevice& () const { return reinterpret_cast<const VkDevice&>(this->allocation->getDevice()); };
        virtual operator const vk::ImageSubresourceLayers() const { return vk::ImageSubresourceLayers{ reinterpret_cast<const vk::ImageAspectFlags&>(subresourceRange.aspectMask), subresourceRange.baseMipLevel, subresourceRange.baseArrayLayer, subresourceRange.layerCount }; };

        // 
        virtual vk::Image& getImage() { return *this->allocation; };
        virtual vk::ImageView& getImageView() { return reinterpret_cast<vk::ImageView&>(this->imgInfo.imageView); };
        virtual vk::ImageLayout& getImageLayout() { return reinterpret_cast<vk::ImageLayout&>(this->imgInfo.imageLayout); };
        virtual vk::Sampler& getSampler() { return reinterpret_cast<vk::Sampler&>(this->imgInfo.sampler); };
        virtual vk::ImageSubresourceRange& getImageSubresourceRange() { return this->subresourceRange; };

#ifdef ENABLE_OPENGL_INTEROP
        virtual GLuint& getGL() { return this->allocation->info.glID; };
        virtual const GLuint& getGL() const { return this->allocation->info.glID; };
#endif

        // 
        virtual const vk::Image& getImage() const { return *this->allocation; };
        virtual const vk::ImageView& getImageView() const { return reinterpret_cast<const vk::ImageView&>(this->imgInfo.imageView); };
        virtual const vk::ImageLayout& getImageLayout() const { return reinterpret_cast<const vk::ImageLayout&>(this->imgInfo.imageLayout); };
        virtual const vk::Sampler& getSampler() const { return reinterpret_cast<const vk::Sampler&>(this->imgInfo.sampler); };
        virtual const vk::ImageSubresourceRange& getImageSubresourceRange() const { return this->subresourceRange; };

        // 
        virtual ImageAllocation* operator->() { return &(*allocation); };
        virtual ImageAllocation& operator*() { return (*allocation); };
        virtual const ImageAllocation* operator->() const { return &(*allocation); };
        virtual const ImageAllocation& operator*() const { return (*allocation); };

    protected: friend VmaImageAllocation; friend ImageAllocation; // 
        vkh::VkDescriptorImageInfo imgInfo = {};
        vkt::uni_ptr<ImageAllocation> allocation = {};

    public: // irrevalent sources
        vkh::VkImageSubresourceRange subresourceRange = {};
    };

    // Wrapper Class
    template<class T = uint8_t>
    class Vector { public: //
        Vector() {};
        Vector(const std::shared_ptr<BufferAllocation>& allocation, const vkt::uni_arg<vk::DeviceSize>& offset = 0ull, const vkt::uni_arg<vk::DeviceSize>& size = VK_WHOLE_SIZE) : allocation(allocation), bufInfo({ allocation->buffer,offset,size }), stride(sizeof(T)) {  this->construct(allocation, offset, size); };
        Vector(const vkt::uni_arg<MemoryAllocationInfo>& allocationInfo, const vkt::uni_arg<vkh::VkBufferCreateInfo>& createInfo = {}) { this->construct(std::make_shared<BufferAllocation>(allocationInfo, createInfo)); };
        Vector(const vkt::uni_arg<VmaAllocator>& allocator, const vkt::uni_arg<vkh::VkBufferCreateInfo>& createInfo = {}, const vkt::uni_arg<VmaMemoryUsage>& vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY) { this->construct(vkt::uni_ptr<BufferAllocation>(std::make_shared<VmaBufferAllocation>(allocator, createInfo, vmaUsage))); };
        ~Vector() {};

        //
        virtual Vector<T>* construct(const vkt::uni_ptr<BufferAllocation>& allocation, const vkt::uni_arg<vk::DeviceSize>& offset = 0ull, const vkt::uni_arg<vk::DeviceSize>& size = VK_WHOLE_SIZE, const vkt::uni_arg<vk::DeviceSize>& stride = sizeof(T)) {
            this->allocation = allocation;
            this->bufInfo = { allocation->buffer,offset,size };
            this->stride = stride;
            return this;
        };

        // 
        template<class Tm = T> Vector(const Vector<Tm>& V) : allocation(V), bufInfo({ V.buffer(), V.offset(), V.range() }), stride(sizeof(T)) { *this = V; };
        template<class Tm = T> inline Vector<T>& operator=(const Vector<Tm>& V) { this->allocation = V.uniPtr(), this->bufInfo = vk::DescriptorBufferInfo(V.buffer(), V.offset(), V.range()), this->stride = sizeof(T); return *this; };

        // But it doesn't working...(currently)
        template<class Tm = T> Vector(const vkt::uni_arg<Vector<Tm>>& V) : allocation(V), bufInfo({ V->buffer(), V->offset(), V->range() }), stride(sizeof(T)) { *this = V; };
        template<class Tm = T> inline Vector<T>& operator=(const vkt::uni_arg<Vector<Tm>>& V) { this->allocation = V->uniPtr(), this->bufInfo = vk::DescriptorBufferInfo(V->buffer(), V->offset(), V->range()), this->stride = sizeof(T); return *this; };

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

        // 
        virtual vk::BufferView& createBufferView(const vk::Format& format = vk::Format::eUndefined) {
            vkh::VkBufferViewCreateInfo info = {};
            info.buffer = bufInfo.buffer;
            info.offset = bufInfo.offset;
            info.range = bufInfo.range;
            info.format = VkFormat(format); // TODO: AUTO-FORMAT
            return (view = allocation->getDevice().createBufferView(info));
        };

        // 
        virtual vkt::uni_ptr<BufferAllocation>& uniPtr() { return allocation; };
        virtual const vkt::uni_ptr<BufferAllocation>& uniPtr() const { return allocation; };

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
        virtual operator BufferAllocation*() { return allocation; };
        virtual operator vkt::uni_ptr<BufferAllocation>& () { return allocation; };
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
        virtual operator const BufferAllocation*() const { return allocation; };
        virtual operator const vkt::uni_ptr<BufferAllocation>& () const { return allocation; };
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
        virtual vk::DeviceSize range() const { return (this->bufInfo.range != VK_WHOLE_SIZE ? std::min(this->bufInfo.range, this->allocation->range() - this->bufInfo.offset) : (this->allocation->range() - this->bufInfo.offset)); };
        virtual vk::DeviceSize size() const { return this->range() / this->stride; };

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
        protected: vk::DescriptorBufferInfo bufInfo = { {}, 0u, VK_WHOLE_SIZE };
        public   : vk::DeviceSize stride = sizeof(T);
        protected: vk::BufferView view = {};
        protected: vkt::uni_ptr<BufferAllocation> allocation = {};
    };
};
